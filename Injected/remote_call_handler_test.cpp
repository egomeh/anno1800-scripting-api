#include "remote_call_handler_test.h"

bool RemoteCallHandlerTest::WriteToLog(const std::string& message)
{
	return false;
}

bool RemoteCallHandlerTest::GetGameTime(uint64_t* time, uint64_t* frame)
{
	return false;
}

bool RemoteCallHandlerTest::GetShipWaypoints(const uint64_t& shipID, std::vector<Coordinate>* waypoints)
{
	return false;
}

bool RemoteCallHandlerTest::GetIslandIds(std::vector<uint64_t>* ids)
{
	return false;
}

bool RemoteCallHandlerTest::GetIslandNames(const int32_t& Area, std::vector<std::string>* names)
{
	return false;
}
