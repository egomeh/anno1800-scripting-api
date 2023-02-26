// Auto generated code
#pragma once
#include <inttypes.h>
#include <string>
#include <vector>
#include "structs.gen.h"
#include "socket.h"

class RemoteCallHandlerBase
{
public:
    virtual bool WriteMessageBox(const std::string& message) {return true;};
    virtual bool GetGameTime(uint64_t* time, uint64_t* frame) {return true;};
    virtual bool GetWorldIslands(const uint32_t& area, const bool& mustBelongToThePlayer, std::vector<IslandInfo>* islands) {return true;};
    virtual bool GetAllAreas(std::vector<uint32_t>* areas) {return true;};
    virtual bool GetIslandResources(const uint32_t& areaCode, const uint32_t& islandId, std::vector<IslandResource>* resources) {return true;};
    virtual bool GetIslandResidentialConsumption(const uint32_t& areaCode, const uint32_t& islandId, std::vector<ResourceConsumption>* resources) {return true;};
    virtual bool GetIslandIndustrialConversion(const uint32_t& areaId, const uint32_t& islandId, std::vector<ResourceConsumption>* resources) {return true;};
    virtual bool MinMaxResourcesOnIsland(const uint32_t& areaId, const uint32_t& islandId, const uint32_t& lowerBound, const uint32_t& upperBound) {return true;};
    virtual bool GetShipsInRegion(const uint32_t& areaId, std::vector<ShipInfo>* ships) {return true;};
    virtual bool SetIslandResource(const uint32_t& world_id, const uint32_t& island_id, const uint32_t& resource_type, const uint32_t& target_value, uint32_t* previous_value) {return true;};
    virtual bool DebugGetResourceInfoFromAddress(const uint64_t& address, IslandResource* resource) {return true;};
    virtual bool DebugGetResourceChainInfoFromAddress(const uint64_t& address, std::vector<IslandResource>* resource) {return true;};
    virtual bool DebugGetIslandNameFromAddress(const uint64_t& address, std::string* name) {return true;};
    virtual bool DebugReadStringFromAddress(const uint64_t& address, std::string* name) {return true;};
    virtual bool DebugGetIslandResources(const uint64_t& address, std::vector<IslandResource>* resources) {return true;};
    virtual bool DebugGetIslandChainFromAddress(const uint64_t& address, const bool& mustBelongToThePlayer, std::vector<IslandInfo>* islands) {return true;};
    virtual bool DebugGetFirstAreaStructAddress(uint64_t* address) {return true;};
    virtual bool DebugGetAreaWithCode(const uint32_t& code, uint64_t* address) {return true;};
    virtual bool DebugGetNameFromGuid(const uint32_t& guid, std::string* name) {return true;};
    virtual bool DebugGetGuidFromName(const std::string& name, uint32_t* guid) {return true;};
    virtual bool DebugVirtualGetComponentFromAddress(const uint64_t& address, const uint64_t& componentId, uint64_t* componentAddress) {return true;};
    virtual bool DebugGetIslandBuildingAddresses(const uint32_t& areaId, const uint32_t& islandId, std::vector<uint64_t>* addresses) {return true;};
    virtual bool DebugTryEnqueueShipForTrade(const uint32_t& areaId, const uint32_t& islandId, const uint64_t& tradeComponent) {return true;};
    virtual bool DebugGetAreaAddress(const uint32_t& areaID, uint64_t* areaAddress) {return true;};
    virtual bool DebugGetVehicleLists(std::vector<uint64_t>* vehicleLists) {return true;};
    virtual bool DebugGetEntityComponets(const uint64_t& entityAddress, std::vector<DebugComponent>* components) {return true;};
};

bool HandleRemoteCall(SocketHandler& socketHandler, RemoteCallHandlerBase& callHandler);
