#include "testing.h"
#include "serialization.gen.h"
#include "structs.gen.h"
#include "remote_call_handler_test.h"
#include "remote_call_handler_base.gen.h"
#include "serialization.gen.h"

#include <Windows.h>

void injected()
{
	SocketHandler socketHandler;
	RemoteCallHandlerTest callHandler;

	socketHandler.Initialize();

	MessageBoxA(NULL, "Hello injection", "Injectinator", MB_OK);
}

