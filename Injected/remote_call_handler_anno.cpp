#include "remote_call_handler_anno.h"
#include <windows.h>

bool RemoteCallHandlerAnno::WriteMessageBox(const std::string& message)
{
	MessageBoxA(NULL, message.c_str(), "Write message to log", MB_OK);
	return true;
}

bool RemoteCallHandlerAnno::GetGameTime(uint64_t* time, uint64_t* frame)
{
	*time = 4;
	*frame = 9;
	return true;
}
