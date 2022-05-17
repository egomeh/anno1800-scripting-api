#include "remote_call_handler_test.h"
#include "tools.h"
#include "anno_tools.h"

bool RemoteCallHandlerTest::DebugGetIslandChainFromAddress(const uint64_t& address, const bool& mustBelongToThePlayer, std::vector<IslandInfo>* islands)
{
	IslandInfo info;
	info.name = "hello world";
	info.debug_address = 0x1122334455667788;
	info.island_id = 0x2202;

	islands->push_back(info);

	return true;
}

bool RemoteCallHandlerTest::DebugGetNameFromGuid(const uint32_t& guid, std::string* name)
{
	bool known;
	*name = GetNameFromGUID(guid, known);
	return false;
}

bool RemoteCallHandlerTest::DebugGetGuidFromName(const std::string& name, uint32_t* guid)
{
	*guid = (uint32_t)GetGuidFromName("wool");
	return false;
}

