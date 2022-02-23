#pragma once

#include "remote_call_handler_base.gen.h"

class RemoteCallHandlerTest : public RemoteCallHandlerBase
{
	// Inherited via RemoteCallHandlerBase
	virtual bool WriteToLog(const std::string& message) override;
	virtual bool GetGameTime(uint64_t* time, uint64_t* frame) override;
	virtual bool GetShipWaypoints(const uint64_t& shipID, std::vector<Coordinate>* waypoints) override;
	virtual bool GetIslandIds(std::vector<uint64_t>* ids) override;
	virtual bool GetIslandNames(const int32_t& Area, std::vector<std::string>* names) override;
};
