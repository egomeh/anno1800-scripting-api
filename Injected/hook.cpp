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

uint64_t AnnoFunctionOffset(BinaryCRC32 binary_crc, HookedFunction function)
{
    if (binary_crc == BinaryCRC32::Invalid)
    {
        return 0;
    }
    else if (binary_crc == BinaryCRC32::Steam)
    {
        switch (function)
        {
        case HookedFunction::Any:                       return 0;
        case HookedFunction::GameTimeHook:              return 0x09B0F2;
        case HookedFunction::SessionTickHook:           return 0x25B270;
        case HookedFunction::ConsumptionHook:           return 0x0; // 0xAD4CFD;
        case HookedFunction::VehicleSortingHook:        return 0x0;
        }
    }
    else if (binary_crc == BinaryCRC32::EpicStore)
    {
        switch (function)
        {
        case HookedFunction::Any:                       return 0;
        case HookedFunction::GameTimeHook:              return 0x08A0CD;
        case HookedFunction::SessionTickHook:           return 0xBE4E00;
        case HookedFunction::ConsumptionHook:           return 0xAD1BDD;
        case HookedFunction::VehicleSortingHook:        return 0; // Have not found this yet 
        }
    }

    return 0;
}

uint64_t AnnoDataOffset(BinaryCRC32 binary_crc, DataOffset offset)
{
    if (binary_crc == BinaryCRC32::Invalid)
    {
        return 0;
    }
    else if (binary_crc == BinaryCRC32::Steam)
    {
        switch (offset)
        {
        case DataOffset::Invalid:                       return 0x0;
        case DataOffset::GameStateOffset:               return 0x62ABBC0;
        case DataOffset::AssetNameDatabase:             return 0x62C5490;
        case DataOffset::FunctionGUIDToName:            return 0x40200E0;
        }
    }
    else if (binary_crc == BinaryCRC32::EpicStore)
    {
        switch (offset)
        {
        case DataOffset::Invalid:                       return 0x0;
        case DataOffset::GameStateOffset:               return 0x627DAB8;
        case DataOffset::AssetNameDatabase:             return 0x627DD50;
        case DataOffset::FunctionGUIDToName:            return 0x3FFB570;
        }
    }
    return 0;
}


void HookManagerServiceHook(HookedFunction current_hook, HookData hook_data)
{
    HookManager::Get().ServiceHook(current_hook, hook_data);
}

