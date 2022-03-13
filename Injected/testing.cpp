#include "testing.h"
#include "serialization.gen.h"
#include "structs.gen.h"
#include "remote_call_handler_test.h"
#include "remote_call_handler_base.gen.h"
#include "serialization.gen.h"
#include "hook.h"

#include <vector>


void testing()
{
	SocketHandler socketHandler;
	RemoteCallHandlerTest callHandler;

	socketHandler.Initialize();

	HookManager::Get().ExecuteInHookAsync(HookedFunction::Any,
		[](HookData data) -> bool
		{
			return true; 
		});

	HookData data = {};
	HookManager::Get().ServiceHook(HookedFunction::Any, data);
}

