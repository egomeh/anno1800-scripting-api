#pragma once

#include "remote_call_handler_base.gen.h"

class RemoteCallHandlerTest : public RemoteCallHandlerBase
{
public:
	virtual bool DebugGetIslandChainFromAddress(const uint64_t& address, const bool& mustBelongToThePlayer, std::vector<IslandInfo>* islands) override;
	virtual bool DebugGetNameFromGuid(const uint32_t& guid, std::string* name);
	virtual bool DebugGetGuidFromName(const std::string& name, uint32_t* guid);
};
