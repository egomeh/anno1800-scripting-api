#pragma once

#include "remote_call_handler_base.gen.h"

class RemoteCallHandlerTest : public RemoteCallHandlerBase
{
public:
	virtual bool DebugGetIslandChainFromAddress(const uint64_t& address, std::vector<IslandInfo>* islands) override;
};
