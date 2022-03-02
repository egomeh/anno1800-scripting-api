#include "testing.h"
#include "serialization.gen.h"
#include "structs.gen.h"
#include "remote_call_handler_test.h"
#include "remote_call_handler_base.gen.h"
#include "serialization.gen.h"

#include <vector>

void testing()
{
	SocketHandler socketHandler;
	RemoteCallHandlerTest callHandler;

	socketHandler.Initialize();

	bool handled = HandleRemoteCall(socketHandler, callHandler);
}

