#pragma once

#include <stdint.h>
#include <functional>
#include <list>
#include <windows.h>

enum class HookedFunction
{
    Any,
    GameTimeHook
};

struct HookData
{
    uint32_t ebp;
    uint32_t eax;
};

struct HookExecutionRequest
{
    // Sync event used if the client caller wants to wait for the hook to finish
    HANDLE sync_event = NULL;
    HookedFunction hook;
    std::function<bool(const HookData)> function = {};
};

class HookManager
{
public:
    HookManager();

    static HookManager& Get();

    void SetEbp(int ebp);

    void SetEax(int eax);

    void ServiceHook(HookedFunction current_hook);

    bool ExecuteInHookBase(HookedFunction hook_to_execute, std::function<bool(const HookData&)> function, bool async);
    bool ExecuteInHookSync(HookedFunction hook_to_execute, std::function<bool(const HookData&)> function);
    bool ExecuteInHookAsync(HookedFunction hook_to_execute, std::function<bool(const HookData&)> function);

    int ebp;

private:
    CRITICAL_SECTION submit_function_for_hook_cs;
    HookData hook_data;
    std::list<HookExecutionRequest> hook_execution_requests;
};

