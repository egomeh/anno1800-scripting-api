#pragma once

#include "global_includes.h"
#include <list>
#include <functional>

enum class HookedFunction
{
    Any = 0,
    GameTimeHook = 1,
    SessionTickHook = 2,
    ConsumptionHook = 3,
    VehicleSortingHook = 4,
};

enum class DataOffset : uint64_t
{
    Invalid = 0,
    TimeStructOffset = 1,
    GameStateOffset = 2,
    AssetNameDatabase = 3,
    FunctionGUIDToName = 4
};

enum class BinaryCRC32 : uint32_t
{
    Invalid = 0,
    Steam = 0x5DDC0A59,
    EpicStore = 0xC6565241,
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
    HANDLE sync_event = nullptr;
    HookedFunction hook;
    std::function<bool(HookData&)> function = {};
};

class HookManager
{
public:
    HookManager();
    
    void ShutDown();

    static HookManager& Get();

    void Initialize();

    void ServiceHook(HookedFunction current_hook, HookData hook_data);

    bool ExecuteInHookBase(HookedFunction hook_to_execute, std::function<bool(HookData)> function, bool async);
    bool ExecuteInHookSync(HookedFunction hook_to_execute, std::function<bool(HookData)> function);
    bool ExecuteInHookAsync(HookedFunction hook_to_execute, std::function<bool(HookData)> function);

private:

    bool shutting_down;
    CRITICAL_SECTION access_hook_requests_cs;
    std::list<HookExecutionRequest> hook_execution_requests;
};

uint64_t AnnoFunctionOffset(BinaryCRC32 binary_crc, HookedFunction function);
uint64_t AnnoDataOffset(BinaryCRC32 binary_crc, DataOffset function);

// To have an interface for assembly
extern "C"
{
void HookManagerServiceHook(HookedFunction current_hook, HookData hook_data);
}
