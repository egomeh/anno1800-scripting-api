#include "testing.h"
#include "serialization.gen.h"
#include "structs.gen.h"
#include "remote_call_handler_test.h"
#include "remote_call_handler_base.gen.h"
#include "serialization.gen.h"
#include "hook.h"

#include <vector>

DWORD Servicer(HMODULE module)
{
	while (true)
	{
		HookData test_hook_data = {};
		HookManager::Get().ServiceHook(HookedFunction::GameTimeHook, test_hook_data);
	}
}

void testing()
{
	SocketHandler socketHandler;
	RemoteCallHandlerTest callHandler;

	socketHandler.Initialize();

	HookManager::Get();

	HookManager::Get().ShutDown();

	CRITICAL_SECTION cs;

	EnterCriticalSection(&cs);
}

