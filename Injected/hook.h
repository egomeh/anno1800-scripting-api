#pragma once

#include <stdint.h>
#include <functional>
#include <list>
#include <windows.h>

enum class HookedFunction
{
    Any = 0,
    GameTimeHook = 1
};

struct HookData
{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t r8;
    uint64_t r9;
};

struct HookExecutionRequest
{
    // Sync event used if the client caller wants to wait for the hook to finish
    HANDLE sync_event = NULL;
    HookedFunction hook;
    std::function<bool(HookData&)> function = {};
};

class HookManager
{
public:
    HookManager();

    static HookManager& Get();

    void ServiceHook(HookedFunction current_hook, HookData hook_data);

    bool ExecuteInHookBase(HookedFunction hook_to_execute, std::function<bool(HookData)> function, bool async);
    bool ExecuteInHookSync(HookedFunction hook_to_execute, std::function<bool(HookData)> function);
    bool ExecuteInHookAsync(HookedFunction hook_to_execute, std::function<bool(HookData)> function);

private:
    CRITICAL_SECTION submit_function_for_hook_cs;
    std::list<HookExecutionRequest> hook_execution_requests;
};

// To have an interface for assembly
extern "C"
{
void HookManagerServiceHook(HookedFunction current_hook, HookData hook_data);
}
