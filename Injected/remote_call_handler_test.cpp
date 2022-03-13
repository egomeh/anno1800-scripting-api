#include "remote_call_handler_test.h"


bool RemoteCallHandlerTest::DebugGetIslandChainFromAddress(const uint64_t& address, std::vector<IslandInfo>* islands)
{
	IslandInfo info;
	info.name = "hello world";
	info.debug_address = 0x1122334455667788;
	info.island_id = 0x2202;

	islands->push_back(info);

	return true;
}
