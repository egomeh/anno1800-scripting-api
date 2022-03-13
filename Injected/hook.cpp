#include "hook.h"

HookManager::HookManager()
{
    if (!InitializeCriticalSectionAndSpinCount(&submit_function_for_hook_cs, 0x4000))
        abort();

    if (!InitializeCriticalSectionAndSpinCount(&handle_request_cs, 0x4000))
        abort();
}

void HookManager::ShutDown()
{
    DeleteCriticalSection(&submit_function_for_hook_cs);
    DeleteCriticalSection(&handle_request_cs);
}

HookManager& HookManager::Get()
{
    static HookManager hook_manager;
    return hook_manager;
}

void HookManager::ServiceHook(HookedFunction current_hook, HookData hook_data)
{
    bool request_pending_delete = false;

    for (auto it = hook_execution_requests.begin(); it != hook_execution_requests.end();)
    {
        HookExecutionRequest& current_request = *it;

        if (current_hook != current_request.hook && current_request.hook != HookedFunction::Any)
        {
            ++it;
            continue;
        }

        // if the function is not satisifed, we exit.
        if (current_request.satisfied)
        {
            ++it;
            continue;
        }

        if (current_request.function(hook_data))
        {
            // If one or more threads determine success
            // then mark the function as satisfied
            current_request.satisfied = true;

            // Also indicate that there is now a request that is pending kill
        }

        ++it;
    }

    for (auto it = hook_execution_requests.begin(); it != hook_execution_requests.end();)
    {
        HookExecutionRequest& current_request = *it;

        // Request was not satisfied so don't delete it
        if (!current_request.satisfied)
        {
            ++it;
            continue;
        }

        // and we can also signal the caller if its sync
        if (current_request.sync_event != NULL)
        {
            // set to awake the callee
            SetEvent(current_request.sync_event);

            // wait for callee to awake and move on
            WaitForSingleObject(current_request.sync_event, INFINITE);

            // now close as we konw the callee has moved on
            CloseHandle(current_request.sync_event);

            current_request.sync_event = NULL;
        }

        ++it;
        // it = hook_execution_requests.erase(it);
    }
}

bool HookManager::ExecuteInHookBase(HookedFunction hook_to_execute, std::function<bool(HookData)> function, bool async)
{
    EnterCriticalSection(&submit_function_for_hook_cs);

    hook_execution_requests.emplace_back();
    HookExecutionRequest& request = hook_execution_requests.back();

    if (!async)
        request.sync_event = CreateEventA(NULL, FALSE, FALSE, "hook_request_event");
    else
        request.sync_event = NULL;

    request.hook = hook_to_execute;
    request.function = function;
    request.satisfied = false;

    LeaveCriticalSection(&submit_function_for_hook_cs);

    if (!async && request.sync_event != NULL)
    {
        WaitForSingleObject(request.sync_event, INFINITE);

        // set to allow the hook manager know we can remove the event
        SetEvent(request.sync_event);
    }

    return true;
}

bool HookManager::ExecuteInHookSync(HookedFunction hook_to_execute, std::function<bool(HookData)> function)
{
    return ExecuteInHookBase(hook_to_execute, function, false);
}

bool HookManager::ExecuteInHookAsync(HookedFunction hook_to_execute, std::function<bool(HookData)> function)
{
    return ExecuteInHookBase(hook_to_execute, function, true);
}

void HookManagerServiceHook(HookedFunction current_hook, HookData hook_data)
{
    HookManager::Get().ServiceHook(current_hook, hook_data);
}

