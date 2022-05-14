#pragma once

#include "remote_call_handler_base.gen.h"

class RemoteCallHandlerAnno : public RemoteCallHandlerBase
{
public:
	uint64_t module_base = 0;

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
	virtual bool GetPlayerIslandsInWorld(const uint32_t& area, std::vector<IslandInfo>* islands) override;
	virtual bool GetAllIslandsOfWorld(const uint32_t& area, std::vector<IslandInfo>* islands) override;
	virtual bool GetAllAreas(std::vector<uint32_t>* areas) override;
	virtual bool DebugGetNameFromGuid(const uint32_t& guid, std::string* name) override;
	virtual bool DebugGetGuidFromName(const std::string& name, uint32_t* guid) override;
	virtual bool GetIslandResources(const uint32_t& areaCode, const uint32_t& islandId, std::vector<IslandResource>* resources) override;
	virtual bool DebugVirtualGetComponentFromAddress(const uint64_t& address, const uint64_t& componentId, uint64_t* componentAddress) override;
	virtual bool GetIslandResidentialConsumption(const uint32_t& areaCode, const uint32_t& islandId, std::vector<ResourceConsumption>* resources) override;
	virtual bool DebugGetIslandBuildingAddresses(const uint32_t& areaId, const uint32_t& islandId, std::vector<uint64_t>* addresses) override;
	virtual bool GetIslandIndustrialConversion(const uint32_t& areaId, const uint32_t& islandId, std::vector<ResourceConsumption>* resources) override;
	virtual bool DebugTryEnqueueShipForTrade(const uint32_t& areaId, const uint32_t& islandId, const uint64_t& tradeComponent) override;
	virtual bool MinMaxResourcesOnIsland(const uint32_t& areaId, const uint32_t& islandId, const uint32_t& lowerBound, const uint32_t& upperBound) override;
};
