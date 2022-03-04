#include "testing.h"
#include "serialization.gen.h"
#include "structs.gen.h"
#include "remote_call_handler_anno.h"
#include "remote_call_handler_base.gen.h"
#include "serialization.gen.h"

#include <Windows.h>

void injected()
{
	SocketHandler socketHandler;
	RemoteCallHandlerAnno callHandler;

	socketHandler.Initialize();

	bool stillGoing = true;

	while (stillGoing)
		stillGoing = HandleRemoteCall(socketHandler, callHandler);
}

