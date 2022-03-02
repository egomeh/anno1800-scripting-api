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
    virtual bool WriteToLog(const std::string& message) = 0;
    virtual bool GetGameTime(uint64_t* time, uint64_t* frame) = 0;
    virtual bool GetShipWaypoints(const std::string& something, const uint64_t& shipID, std::vector<Coordinate>* waypoints) = 0;
    virtual bool GetIslandIds(std::vector<uint64_t>* ids) = 0;
    virtual bool GetIslandNames(const int32_t& Area, std::vector<std::string>* names) = 0;
};

bool HandleRemoteCall(SocketHandler& socketHandler, RemoteCallHandlerBase& callHandler);
