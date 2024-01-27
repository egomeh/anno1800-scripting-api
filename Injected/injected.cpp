#include "structs.gen.h"
#include "tools.h"
#include "memory.h"
#include "hook.h"
#include "log.h"
#include "ui.h"

#include <Windows.h>
#include <string>
#include <sstream>
#include <ios>
#include <array>

extern "C"
{
    void game_time_hook_trampoline();
    void session_tick_hook_trampoline();
    void consumption_hook_trampoline();
    void vehicle_sorting_hook_trampoline();
    void building_hook_trampoline();
    uint64_t get_area_from_tls();
}

void injected(BinaryCRC32 binary_crc)
{
    ANNO_LOG("Running injected code in Anno 1800");

    HookManager::Get().Initialize();

    HANDLE anno_process = GetCurrentProcess();

    // Get the modules that are loaded in the exe.
    HMODULE* modules = nullptr;
    uint64_t numberOfModules = 0;
    if (!GetProcessModules(anno_process, &modules, numberOfModules))
        return;

    HMODULE annoModule;
    if (!FindModule(anno_process, modules, numberOfModules, "Anno1800.exe", annoModule))
        return;

    MODULEINFO moduleInfo;
    if (!GetModuleInformation(anno_process, annoModule, &moduleInfo, sizeof(moduleInfo)))
        return;

    uint64_t moduleBase = (uint64_t)(moduleInfo.lpBaseOfDll);
    uint64_t entryPoint = (uint64_t)(moduleInfo.EntryPoint);
    uint64_t moduleSize = (uint64_t)(moduleInfo.SizeOfImage);
    uint64_t exeSectionSize = moduleSize - (entryPoint - moduleBase);

    g_ModuleBase = moduleBase;
    g_BinaryCRC = binary_crc;

    UI::Get().EnableHook();

    //MemoryReplacement timeAndFrameHook;
    //timeAndFrameHook.SetMemory
    //({
    //    0x48, 0xb9,                                             // movabs rcx, [imm64]
    //    EIGHT_BYTES((uint64_t)game_time_hook_trampoline),       // hook address
    //    0x90, 0x90, 0x90, 0x90, 0x90,                           // 5 nops
    //    0xff, 0xd1                                              // call rcx
    //    });
    //uint64_t time_and_frame_offset = AnnoFunctionOffset(binary_crc, HookedFunction::GameTimeHook);
    //timeAndFrameHook.Emplace((void*)(moduleBase + time_and_frame_offset));

    MemoryReplacement session_tick_hook;
    session_tick_hook.SetMemory
    ({
        0x48, 0xb8,                                             // movabs rax, [imm64]
        EIGHT_BYTES((uint64_t)session_tick_hook_trampoline),    // hook address
        0x90, 0x90, 0x90, 0x90,                                 // 4 nops
        0xff, 0xd0                                              // call rax
        });
    uint64_t session_tick_hook_offset = AnnoFunctionOffset(binary_crc, HookedFunction::SessionTickHook);
    session_tick_hook.Emplace((void*)(moduleBase + session_tick_hook_offset));

    //MemoryReplacement consumption_hook;
    //consumption_hook.SetMemory
    //({
    //    0x48, 0xbb,                                             // movabs rbx, [imm64]
    //    EIGHT_BYTES((uint64_t)consumption_hook_trampoline),     // hook address
    //    0x90, 0x90, 0x90, 0x90, 0x90,                           // 5 nops
    //    0x90, 0x90, 0x90, 0x90, 0x90,                           // 5 nops
    //    0x90, 0x90, 0x90, 0x90,                                 // 4 nops
    //    0xff, 0xd3                                              // call rbx
    //    });
    //uint64_t consumption_hook_offset = AnnoFunctionOffset(binary_crc, HookedFunction::ConsumptionHook);
    //consumption_hook.Emplace((void*)(moduleBase + consumption_hook_offset));

    // Add a hook for when the game is sorting controllable vehicles (on steam: Anno1800.exe+D52550)
    //MemoryReplacement vehicle_sort_hook;
    //vehicle_sort_hook.SetMemory
    //({
    //    0x48, 0xb8,                                             // movabs rax, [imm64]
    //    EIGHT_BYTES((uint64_t)vehicle_sorting_hook_trampoline), // hook address
    //    0x90, 0x90, 0x90, 0x90, 0x90,                           // 5 nops
    //    0xff, 0xd0                                              // call rax
    //    });
    //uint64_t vehicle_sort_hook_offset = AnnoFunctionOffset(binary_crc, HookedFunction::VehicleSortingHook);
    //vehicle_sort_hook.Emplace((void*)(moduleBase + vehicle_sort_hook_offset));

    // Handle remote calls until we fail

    WaitForSingleObject(UI::Get().ShutdownEvent, INFINITE);
    ANNO_LOG("Done waiting, we're leaving");

    UI::Get().DisableHook();

    CloseHandle(anno_process);

    HookManager::Get().ShutDown();
}

