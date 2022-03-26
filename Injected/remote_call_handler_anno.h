#pragma once

#include "remote_call_handler_base.gen.h"

class RemoteCallHandlerAnno : public RemoteCallHandlerBase
{
public:
	virtual bool WriteMessageBox(const std::string& message) override;
	virtual bool GetGameTime(uint64_t* time, uint64_t* frame) override;
	virtual bool DebugGetResourceInfoFromAddress(const uint64_t& address, IslandResource* resource) override;
	virtual bool DebugGetResourceChainInfoFromAddress(const uint64_t& address, std::vector<IslandResource>* resource) override;
	virtual bool DebugGetIslandNameFromAddress(const uint64_t& address, std::string* name) override;
	virtual bool DebugReadStringFromAddress(const uint64_t& address, std::string* result) override;
	virtual bool DebugGetIslandResources(const uint64_t& address, std::vector<IslandResource>* resources) override;
	virtual bool DebugGetIslandChainFromAddress(const uint64_t& address, std::vector<IslandInfo>* islands) override;
	virtual bool DebugGetFirstAreaStructAddress(uint64_t* address) override;
	virtual bool DebugGetAreaWithCode(const uint32_t& areaCode, uint64_t* address) override;
};
