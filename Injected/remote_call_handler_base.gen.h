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
    virtual bool DebugGetIslandNameFromAddress(const uint64_t& address, std::string* name) {return true;};
    virtual bool DebugReadStringFromAddress(const uint64_t& address, std::string* name) {return true;};
    virtual bool DebugGetIslandResources(const uint64_t& address, std::vector<IslandResource>* resources) {return true;};
    virtual bool DebugGetIslandChainFromAddress(const uint64_t& address, std::vector<IslandInfo>* islands) {return true;};
    virtual bool DebugGetFirstAreaStructAddress(uint64_t* address) {return true;};
    virtual bool DebugGetAreaWithCode(const uint32_t& code, uint64_t* address) {return true;};
    virtual bool GetPlayerIslandsInWorld(const uint32_t& area, std::vector<IslandInfo>* islands) {return true;};
};

bool HandleRemoteCall(SocketHandler& socketHandler, RemoteCallHandlerBase& callHandler);
