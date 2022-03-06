#include "hook.h"

HookManager::HookManager()
{
    if (!InitializeCriticalSectionAndSpinCount(&submit_function_for_hook_cs, 0x4000))
        abort();
}

HookManager& HookManager::Get()
{
    static HookManager hook_manager;
    return hook_manager;
}

void HookManager::ServiceHook(HookedFunction current_hook, HookData hook_data)
{
    for (auto it = hook_execution_requests.begin(); it != hook_execution_requests.end();)
    {
        HookExecutionRequest& current_request = *it;

        if (current_hook != current_request.hook && current_request.hook != HookedFunction::Any)
        {
            ++it;
            continue;
        }

        // if the function is not satisifed, we exit.
        if (!current_request.function(hook_data))
        {
            ++it;
            continue;
        }

        // at this point, the function is happy and we can remove the request

        // and we can also signal the caller if its sync
        if (current_request.sync_event)
        {
            // set to awake the callee
            SetEvent(current_request.sync_event);

            // wait for callee to awake and move on
            WaitForSingleObject(current_request.sync_event, INFINITE);

            // now close as we konw the callee has moved on
            CloseHandle(current_request.sync_event);
        }

        it = hook_execution_requests.erase(it);
    }
}

bool HookManager::ExecuteInHookBase(HookedFunction hook_to_execute, std::function<bool(HookData)> function, bool async)
{
    HookExecutionRequest request;
    request.hook = hook_to_execute;
    request.function = function;

    if (!async)
        request.sync_event = CreateEventA(NULL, FALSE, FALSE, "hook_request_event");
    else
        request.sync_event = NULL;

    EnterCriticalSection(&submit_function_for_hook_cs);
    hook_execution_requests.push_back(request);
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

