#include "serialization.gen.h"
#include "structs.gen.h"
#include "remote_call_handler_anno.h"
#include "remote_call_handler_base.gen.h"
#include "serialization.gen.h"
#include "tools.h"
#include "memory.h"

#include <Windows.h>
#include <string>
#include <sstream>
#include <ios>

extern "C"
{
    void game_time_hook_trampoline();
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

    MemoryReplacement timeAndFrameHook;
    timeAndFrameHook.SetMemory
    ({
        0x48, 0xb9,                                         // movabs rcx, [imm64]
        EIGHT_BYTES((uint64_t)game_time_hook_trampoline),   // hook address
        0xff, 0xd1,                                         // call rcx
        0x90, 0x90, 0x90, 0x90, 0x90                        // 5 nops
    });
    timeAndFrameHook.Emplace((void*)(moduleBase + 0x4A36D));

	bool stillGoing = true;
	while (stillGoing)
		stillGoing = HandleRemoteCall(socketHandler, callHandler);
}

