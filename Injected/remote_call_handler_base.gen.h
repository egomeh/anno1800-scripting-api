// Auto generated code
#pragma once
#include <inttypes.h>
#include <string>
#include <vector>
#include "structs.gen.h"
#include "socket.h"

class RemoteCallHandlerBase
{
public:
    virtual bool WriteMessageBox(const std::string& message) {return true;};
    virtual bool GetGameTime(uint64_t* time, uint64_t* frame) {return true;};
    virtual bool DebugGetResourceInfoFromAddress(const uint64_t& address, IslandResource* resource) {return true;};
    virtual bool DebugGetResourceChainInfoFromAddress(const uint64_t& address, std::vector<IslandResource>* resource) {return true;};
};

bool HandleRemoteCall(SocketHandler& socketHandler, RemoteCallHandlerBase& callHandler);
