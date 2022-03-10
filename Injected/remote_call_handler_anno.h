#pragma once

#include "remote_call_handler_base.gen.h"

class RemoteCallHandlerAnno : public RemoteCallHandlerBase
{
public:
	virtual bool WriteMessageBox(const std::string& message) override;
	virtual bool GetGameTime(uint64_t* time, uint64_t* frame) override;
	virtual bool DebugGetResourceInfoFromAddress(const uint64_t& address, IslandResource* resource) override;
	virtual bool DebugGetResourceChainInfoFromAddress(const uint64_t& address, std::vector<IslandResource>* resource);
};
