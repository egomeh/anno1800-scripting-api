#include "serialization.gen.h"
#include "structs.gen.h"
#include "remote_call_handler_anno.h"
#include "remote_call_handler_base.gen.h"
#include "serialization.gen.h"
#include "tools.h"
#include "memory.h"
#include "hook.h"

#include <Windows.h>
#include <string>
#include <sstream>
#include <ios>

extern "C"
{
    void game_time_hook_trampoline();
    void session_tick_hook_trampoline();
}

void injected()
{
	SocketHandler socketHandler;
	RemoteCallHandlerAnno callHandler;

    socketHandler.Initialize();

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

    // Scope for memory placements.
    // When exiting this scope, all native code should
    // be as before the injection
    {
        MemoryReplacement timeAndFrameHook;
        timeAndFrameHook.SetMemory
        ({
            0x48, 0xb9,                                             // movabs rcx, [imm64]
            EIGHT_BYTES((uint64_t)game_time_hook_trampoline),       // hook address
            0xff, 0xd1,                                             // call rcx
            0x90, 0x90, 0x90, 0x90, 0x90                            // 5 nops
            });
        timeAndFrameHook.Emplace((void*)(moduleBase + 0x4A36D));


        MemoryReplacement seesion_tick_hook;
        seesion_tick_hook.SetMemory
        ({
            0x48, 0xb8,                                             // movabs rax, [imm64]
            EIGHT_BYTES((uint64_t)session_tick_hook_trampoline),    // hook address
            0xff, 0xd0,                                             // call rax
            0x90, 0x90, 0x90, 0x90                                  // 5 nops
            });
        // seesion_tick_hook.Emplace((void*)(moduleBase + 0xBF3750));

        // Handle remote calls until we fail
        while (HandleRemoteCall(socketHandler, callHandler));
    }

    Sleep(500);
    HookManager::Get().ShutDown();
}


