#include "testing.h"
#include "serialization.gen.h"
#include "structs.gen.h"
#include "remote_call_handler_test.h"
#include "remote_call_handler_base.gen.h"
#include "serialization.gen.h"
#include "hook.h"
#include "process.h"
#include "memory.h"
#include "tools.h"
#include "log.h"

#include <vector>
#include <iostream>

extern "C"
{
	void test_function_to_hook();
	void session_tick_hook_trampoline();
}

void Servicer()
{
	while (true)
	{
		test_function_to_hook();
	}
}

void testing()
{
	ANNO_LOG("Running teting code");

	SocketHandler socketHandler;
	RemoteCallHandlerTest callHandler;

	socketHandler.Initialize();

	HookManager::Get().Initialize();

	ANNO_LOG("Here 1");

	MemoryReplacement seesion_tick_hook;
	seesion_tick_hook.SetMemory
	({
		0x48, 0xb8,                                             // movabs rax, [imm64]
		EIGHT_BYTES((uint64_t)session_tick_hook_trampoline),    // hook address
		0xff, 0xd0,                                             // call rax
		0x90, 0x90, 0x90, 0x90                                  // 5 nops
	});

	 for (int i = 0; i < 4; ++i)
		_beginthread((_beginthread_proc_type)&Servicer, 0, nullptr);

	void* addressofjump = (void*)&test_function_to_hook;
	uint32_t offset = *(uint32_t*)((uint64_t)addressofjump + 1);
	uint64_t finalAddress = (uint64_t)((uint64_t)addressofjump + offset + 5);
	seesion_tick_hook.Emplace((void*)(finalAddress));

	ANNO_LOG("Here 2");

	while (true);
}

