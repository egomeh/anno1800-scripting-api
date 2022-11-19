#include "hook.h"
#include "tools.h"
#include "log.h"

HookManager::HookManager()
{
}

void HookManager::Initialize()
{
    if (!InitializeCriticalSectionAndSpinCount(&access_hook_requests_cs, 0x4000))
        abort();

    shutting_down = false;
}

void HookManager::ShutDown()
{
    shutting_down = true;

    EnterCriticalSection(&access_hook_requests_cs);
    for (auto it = hook_execution_requests.begin(); it != hook_execution_requests.end();)
    {
        ANNO_LOG("Removing request at shut-down");

        if (it->sync_event != NULL)
            SetEvent(it->sync_event);

        it = hook_execution_requests.erase(it);
    }
    LeaveCriticalSection(&access_hook_requests_cs);

    DeleteCriticalSection(&access_hook_requests_cs);
}

HookManager& HookManager::Get()
{
    static HookManager hook_manager;
    return hook_manager;
}

void HookManager::ServiceHook(HookedFunction current_hook, HookData hook_data)
{
    if (shutting_down)
        return;

    if (!TryEnterCriticalSection(&access_hook_requests_cs))
        return;

    for (auto it = hook_execution_requests.begin(); it != hook_execution_requests.end();)
    {
        HookExecutionRequest& current_request = *it;

        if (current_hook != current_request.hook && current_request.hook != HookedFunction::Any)
        {
            ++it;
            continue;
        }

        if (current_request.function(hook_data))
        {
            if (current_request.sync_event != NULL)
                SetEvent(current_request.sync_event);

            it = hook_execution_requests.erase(it);
            continue;
        }

        ++it;
    }

    LeaveCriticalSection(&access_hook_requests_cs);
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

    EnterCriticalSection(&access_hook_requests_cs);
    hook_execution_requests.push_back(request);
    LeaveCriticalSection(&access_hook_requests_cs);

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

uint64_t AnnoFunctionOffset(uint32_t binary_crc, HookedFunction function)
{
    if (binary_crc == 0)
    {
        return 0;
    }
    else if (binary_crc == 0x50F67B49) // Steam
    {
        switch (function)
        {
        case HookedFunction::Any:                       return 0;
        case HookedFunction::GameTimeHook:              return 0x4D17D;
        case HookedFunction::SessionTickHook:           return 0x5FB840;
        case HookedFunction::ConsumptionHook:           return 0;// 0xAD4CFD;
        }
    }
    else if (binary_crc == 0xDBB41535) // Epic store
    {
        switch (function)
        {
        case HookedFunction::Any:                       return 0;
        case HookedFunction::GameTimeHook:              return 0x08A0CD;
        case HookedFunction::SessionTickHook:           return 0xBE4E00;
        case HookedFunction::ConsumptionHook:           return 0xAD1BDD;
        }
    }

    return 0;
}

void HookManagerServiceHook(HookedFunction current_hook, HookData hook_data)
{
    HookManager::Get().ServiceHook(current_hook, hook_data);
}

