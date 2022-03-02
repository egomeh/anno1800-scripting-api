#pragma once

#include "global_includes.h"
#include <vector>

class SocketHandler
{
public:

    SocketHandler() : m_socket(0)
    {
    }

    bool Initialize();

    bool Receive(std::vector<uint8_t>& buffer);
    bool Send(std::vector<uint8_t>& buffer);

private:

    bool CheckIfWinSocketIsInitialized();
    bool InitializeWinSocket();

    SOCKET m_socket;
};

