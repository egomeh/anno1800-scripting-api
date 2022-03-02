#include "remote_call_handler_test.h"

bool RemoteCallHandlerTest::WriteToLog(const std::string& message)
{
	return false;
}

bool RemoteCallHandlerTest::GetGameTime(uint64_t* time, uint64_t* frame)
{
	return false;
}

bool RemoteCallHandlerTest::GetShipWaypoints(const std::string& something, const uint64_t& shipID, std::vector<Coordinate>* waypoints)
{
	Coordinate c;
	c.x = 5.2f;
	c.y = 6.2f;
	waypoints->push_back(c);
	return true;
}

bool RemoteCallHandlerTest::GetIslandIds(std::vector<uint64_t>* ids)
{
	return false;
}

bool RemoteCallHandlerTest::GetIslandNames(const int32_t& Area, std::vector<std::string>* names)
{
	return false;
}
