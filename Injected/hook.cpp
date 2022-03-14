#include "hook.h"
#include "tools.h"

HookManager::HookManager()
{
    if (!InitializeCriticalSectionAndSpinCount(&submit_function_for_hook_cs, 0x4000))
        abort();

    if (!InitializeCriticalSectionAndSpinCount(&handle_request_cs, 0x4000))
        abort();

    accept_handling = true;
    handling_thread_count = 0;
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

    // When we pass this, the thread count bumps one up
    while (!AcceptingNewThread());

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
            request_pending_delete = true;
        }

        ++it;
    }

    // Down the thread count by one, making 
    LeaveThread();

    if (!request_pending_delete)
        return;

    // Block new threads and wait for all to finish
    SetAllowThreads(false);
    WaitForZeroThreads();

    EnterCriticalSection(&submit_function_for_hook_cs);

    // We're now a thread that want's to delete a 
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
        }

        it = hook_execution_requests.erase(it);
    }

    // Leave and allow threads to process again
    LeaveCriticalSection(&submit_function_for_hook_cs);
    SetAllowThreads(true);
}

bool HookManager::ExecuteInHookBase(HookedFunction hook_to_execute, std::function<bool(HookData)> function, bool async)
{
    HookExecutionRequest request;

    HANDLE sync_event = NULL;

    if (!async)
        sync_event = CreateEventA(NULL, FALSE, FALSE, "hook_request_event");
    else
        sync_event = NULL;

    request.sync_event = sync_event;
    request.hook = hook_to_execute;
    request.function = function;
    request.satisfied = false;

    // Block new threads and wait for all to finish
    SetAllowThreads(false);
    WaitForZeroThreads();

    EnterCriticalSection(&submit_function_for_hook_cs);
    hook_execution_requests.push_back(request);
    LeaveCriticalSection(&submit_function_for_hook_cs);

    SetAllowThreads(true)

    if (!async && sync_event != NULL)
    {
        WaitForSingleObject(sync_event, INFINITE);
        CloseHandle(sync_event);
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

bool HookManager::AcceptingNewThread()
{
    EnterCriticalSection(&handle_request_cs);
    ON_EXIT
    {
        LeaveCriticalSection(&handle_request_cs);
    };

    if (!accept_handling)
        return false;

    ++handling_thread_count;

    return true;
}

void HookManager::LeaveThread()
{
    EnterCriticalSection(&handle_request_cs);
    --handling_thread_count;
    LeaveCriticalSection(&handle_request_cs);
}

void HookManager::SetAllowThreads(bool does_allow)
{
    EnterCriticalSection(&handle_request_cs);
    accept_handling = does_allow;
    LeaveCriticalSection(&handle_request_cs);
}

void HookManager::WaitForZeroThreads()
{
    bool is_zero = false;
    while (is_zero)
    {
        EnterCriticalSection(&handle_request_cs);
        is_zero = handling_thread_count == 0;
        LeaveCriticalSection(&handle_request_cs);
    }
}

void HookManagerServiceHook(HookedFunction current_hook, HookData hook_data)
{
    HookManager::Get().ServiceHook(current_hook, hook_data);
}

