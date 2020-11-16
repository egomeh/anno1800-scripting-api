#include "Q/Tools.h"
#include <iostream>

SOCKET g_TargetCallSocket = NULL;

bool HandleScriptRequest(SOCKET socket)
{
    std::vector<uint8_t> buffer;

    // Read the request from Python
    if (!ReadMessageFromSocket(socket, buffer))
        return false;

    // Relay that resquest to the target code
    int to_target = send(g_TargetCallSocket, (char*)buffer.data(), (int)buffer.size(), 0);
    if (to_target == SOCKET_ERROR)
        return false;

    // Get the response back from the target
    if (!ReadMessageFromSocket(g_TargetCallSocket, buffer))
        return false;

    // Send the response back to python
    int to_python = send(socket, (char*)buffer.data(), (int)buffer.size(), 0);
    if (to_python == SOCKET_ERROR)
        return false;

    return true;
}
