#include "pch.h"
#include "autocomms_native.h"
#include "native_callbacks.h"
#include "native_calls.h"
#include "native_traversal.h"
#include "globals.h"

#include <unordered_map>


namespace
{
    BuildingCache buildingCache;
}

bool AutoComms::TargetCall_GetGameTime(GameTime* gameTime)
{
    gameTime->frame = g_FrameNumber;
    gameTime->time = g_GameTime;

    return true;
}

bool AutoComms::TargetCall_WriteLineToLog(const std::string& message)
{
    std::string headed = "[script] " + message;
    SendMessageToHost(headed.c_str());
    return true;
}

bool AutoComms::TargetCall_WriteLinesToLog(const std::vector<std::string>& messages)
{
    for (const std::string& message : messages)
    {
        std::string headed = "[script] " + message;
        SendMessageToHost(headed.c_str());
    }

    return true;
}

bool AutoComms::TargetCall_GetShipById(const uint64_t& id, ShipData* shipData)
{
    if (!GetShipName(id, &shipData->name))
        return false;

    shipData->shipId = id;

    return true;
}

bool AutoComms::TargetCall_GetShipComponentAddress(const uint64_t& shipID, const uint64_t& componentID, uint64_t* address)
{
    uint64_t shipAddress = 0;
    if (!GetShipById(shipID, &shipAddress))
        return false;

    uint64_t componentAddress = VirtualShipGetComponent(shipAddress, componentID);

    *address = componentAddress;

    return true;
}

bool AutoComms::TargetCall_GetShipMoveData(const uint64_t& id, ShipMoveData* moveData)
{
    uint64_t shipAddress;

    SCOPE_LOCK(&shipServiceLock);

    if (!GetShipById(id, &shipAddress))
        FAIL_MESSAGE("Failed to get ship address from id %llx", id);

    uint64_t component = VirtualShipGetComponent(shipAddress, ComponentIdMoveData);

    if (!component)
        FAIL_MESSAGE("Failed to get movedata component from ship id %llx address %llx", id, shipAddress);

    moveData->moving = ReadU32(component + 0x28);
    moveData->position.x = ReadF32(component + 0x34);
    moveData->position.y = ReadF32(component + 0x38);
    // output->rotation = ReadF32(component + 0x3c);

    return true;
}

bool AutoComms::TargetCall_GetBuildingComponentAddress(const uint64_t& islandID, const uint64_t& buildingID, const uint64_t& componentID, uint64_t* address)
{
    uint64_t buildingAddress = 0;
    
    if (!GetBuildingAddress(islandID, buildingID, &buildingCache, &buildingAddress))
        return false;

    *address = VirtualShipGetComponent(buildingAddress, componentID);

    return true;
}

bool AutoComms::TargetCall_GetAllShips(const Area& area, std::vector<ShipData>* ships)
{
    ships->clear();

    std::vector<uint64_t> shipIds;

    uint64_t areaId = AreaToAreaID(area);

    if (areaId == -1)
        FAIL_MESSAGE("Invalid area id");

    if (!GetAllShipsInWorld(areaId, &shipIds))
        FAIL_MESSAGE("Could not get all messages");

    for (uint64_t shipId : shipIds)
    {
        ShipData data;
        data.shipId = shipId;

        data.shipType = ShipType::Gunboat;

        std::string name;

        if (!GetShipName(shipId, &name))
            FAIL_MESSAGE("Failed to get name from ship with id %lld", shipId);

        uint64_t address = 0;

        if (!GetShipAddress(shipId, &address))
            FAIL_MESSAGE("Failed to get address from ship with id %lld", shipId);

        uint64_t shipTypeId = *((uint64_t*)(address) + 1);

        data.shipType = IDToShipType(shipTypeId);

        data.name = name;

        ships->push_back(data);
    }

    return true;
}

bool AutoComms::TargetCall_GetShipAddress(const uint64_t& shipID, uint64_t* address)
{
    return GetShipById(shipID, address);
}

bool AutoComms::TargetCall_GetShipCargo(const uint64_t& shipID, std::vector<ShipCargoSlot>* cargo)
{
    cargo->clear();
    uint64_t shipBase = 0;
 
    if (!GetShipById(shipID, &shipBase))
    {
        SendMessageToHost("Faield at ship getAddress");
        return false;
    }

    uint64_t cargoPtr = VirtualShipGetComponent(shipBase, ComponentIdCargo);

    if (!cargoPtr)
        return false;

    uint64_t goodsPtr = cargoPtr + 0x28;

    uint64_t goodsAddress = ReadU64(goodsPtr);

    uint64_t slotsPtr = goodsAddress + 0x10;
    uint64_t nSlotsPtr = goodsAddress + 0x18;

    uint32_t nSlots = ReadU32(nSlotsPtr);
    uint64_t slotsBaseAddress = ReadU64(slotsPtr);

    for (uint64_t i = 0; i < nSlots; ++i)
    {
        uint64_t slotBase = slotsBaseAddress + i * 0x28;

        uint64_t hasCargoPtr = slotBase + 0x10;

        uint64_t hasCargo = ReadU64(hasCargoPtr) & 0x00000000000000FF;

        AutoComms::ShipCargoSlot slot = { 0 };

        slot.hasCargo = hasCargo;

        uint64_t amountPtr = slotBase + 0x20;

        uint64_t amount = ReadU64(amountPtr) & 0x00000000FFFFFFFF;

        uint64_t typeStructPtr = slotBase + 0x8;

        uint64_t typeStructBase = ReadU64(typeStructPtr);

        if (typeStructBase)
        {
            uint64_t typePtr = typeStructBase + 0x8;
            uint64_t type = ReadU64(typePtr) & 0x00000000FFFFFFFF;

            if (hasCargo)
                slot.resourceType = IDToResourceType(type);

            slot.amount = amount;
        }

        cargo->push_back(slot);
    }

    return true;
}

bool AutoComms::TargetCall_AddWaypoint(const std::vector<uint64_t>& IDs, const Coordinate& waypoint)
{
    std::string error;
    SetWaypointForShips(IDs, waypoint.x, waypoint.y, error);
    return true;
}

bool AutoComms::TargetCall_GetIslandsByName(const std::string& name, std::vector<IslandData>* islands)
{
    islands->clear();

    std::vector<uint64_t> islandIDs;

    if (!GetIslandIDsByName(name, &islandIDs))
        return false;

    for (uint64_t id : islandIDs)
    {
        IslandData island;
        island.id = id;
        island.name = name;

        islands->push_back(island);
    }

    return true;
}

bool AutoComms::TargetCall_GetAllIslands(std::vector<IslandData>* islands)
{
    islands->clear();

    if (!GetAllIslands(islands)) 
        return false;

    return true;
}

bool AutoComms::TargetCall_GetIslandResources(const uint64_t& islandID, std::vector<IslandResourceRecord>* resources)
{
    resources->clear();

    uint64_t islandBaseAddress = 0;

    if (!GetIslandById(islandID, &islandBaseAddress))
        return false;

    uint64_t resourceStructPtr = islandBaseAddress + 0x1D0;

    if (!IsReadable((void*)resourceStructPtr, 8))
        return false;

    uint64_t resourceStructBase = ReadU64(resourceStructPtr);

    uint64_t resourceTableSizePtr = resourceStructBase + 0x50;
    uint64_t resourceTableBasePtr = resourceStructBase + 0x58;

    if (!IsReadable((void*)resourceTableSizePtr, 8))
        return false;

    if (!IsReadable((void*)resourceTableBasePtr, 8))
        return false;

    uint64_t size = ReadU64(resourceTableSizePtr);
    uint64_t tableBase = ReadU64(resourceTableBasePtr);

    for (uint64_t k = 0; k <= size; ++k)
    {
        uint64_t recordBase = ReadU64(tableBase + k * 8);

        for (; recordBase; recordBase = ReadU64(recordBase))
        {
            uint64_t capacityPtr = recordBase + 0x1C;
            uint64_t typePtr = recordBase + 0x10;
            uint64_t amountPtr = recordBase + 0x20;

            uint32_t amount = ReadU32(amountPtr);
            uint32_t type = ReadU32(typePtr);
            uint32_t capacity = ReadU32(capacityPtr);

            IslandResourceRecord record;
            record.amount = amount;
            record.resourceType = IDToResourceType(type);
            record.capacity = capacity;

            bool alreadyIn = false;
            for (auto r : *resources)
            {
                if (r.resourceType == record.resourceType)
                    alreadyIn = true;
            }

            if (!alreadyIn)
                resources->push_back(record);
        }
    }

    return true;
}

bool AutoComms::TargetCall_GetIslandBuildings(const uint64_t& islandID, std::vector<BuildingData>* buildings)
{
    if (!GetIslandBuildings(islandID, buildings, &buildingCache))
        return false;

    return true;
}

bool AutoComms::TargetCall_ShipDumpCargo(const uint64_t& shipID, const uint64_t& slotIndex)
{
    Coordinate zero = { 0.f, 0.f };
    return TargetCall_ShipDumpCargo(shipID, slotIndex, zero);
}

bool AutoComms::TargetCall_ShipDumpCargo(const uint64_t& shipID, const uint64_t& slotIndex, const Coordinate& position)
{
    return DumpShipCargo(shipID, slotIndex, position.x, position.y);
}

bool AutoComms::TargetCall_LoadCargoToShip(const uint64_t& islandID, const uint64_t& shipID, const Resource& resource, const uint64_t& amount, const uint64_t& slotIndex)
{
    uint64_t resourceTypeID = ResourceToID(resource);
    
    if (resourceTypeID == -1)
        return false;

    return LoadShipCargo(islandID, shipID, resourceTypeID, amount, slotIndex, 0);
}

bool AutoComms::TargetCall_OffloadCargoFromShip(const uint64_t& islandID, const uint64_t& shipID, const uint64_t& amount, const uint64_t& slotIndex)
{
    return LoadShipCargo(islandID, shipID, 0, amount, slotIndex, 1);
}

bool AutoComms::TargetCall_GetBuildingProduction(const uint64_t& islandID, const uint64_t& buildingID, ProductionNode* production)
{
    // SEND_FORMATTED("get buildings for island id %llx", islandID);

    production->input.clear();

    uint64_t buildingAddress = 0;

    SCOPE_LOCK(&shipServiceLock);

    //SEND_FORMATTED("Before getting an address");

    if (!GetBuildingAddress(islandID, buildingID, &buildingCache, &buildingAddress))
        return false;

    if (!buildingAddress)
        return false;

    // SEND_FORMATTED("Before getting building component");

    uint64_t productionComponentAddress = VirtualShipGetComponent(buildingAddress, ComponentIdBuildingProduction);

    bool producing = true;

    if (!productionComponentAddress)
    {
        producing = false;
        productionComponentAddress = VirtualShipGetComponent(buildingAddress, ComponentIdBuildingProductionNoOuput);

        if (!productionComponentAddress)
            return false;
    }

    uint64_t consumptionPtr = ReadU64(productionComponentAddress + 0x130);
    uint64_t consumptionSize = ReadU64(productionComponentAddress + 0x138) & 0xFF;
    uint64_t productionType = ReadU32(productionComponentAddress + 0x148) & 0xFFFFFFFF;

    uint64_t baseProcessTime = ReadU32(productionComponentAddress + 0x1b0);
    float processingModifier = ReadF32(productionComponentAddress + 0x1a8);

    // Not sure if there is some valid case for this
    // but we'll just skip that in any case for now.
    if (consumptionSize > 0 && !consumptionPtr)
        return false;

    for (uint64_t i = 0; i < consumptionSize; ++i)
    {
        uint64_t resourceTypeId = ReadU32(consumptionPtr + i * 8);
        Resource resource = IDToResourceType(resourceTypeId);
        production->input.push_back(resource);
    }

    Resource productionResource = producing ? IDToResourceType(productionType) : Resource::Invalid;
    production->output = productionResource;

    // baseProcessTime: under neutral conditions, this is how many miliseconds processing takes
    // processingModifier = The processing time is converted to a floating point count down, after
    //                      this conversion, the remaining processing time gets a subtraction
    //                      that is the 100ms (1 tick) times the processing modifier
    //                      Under normal conditions, the modifier is 1.0, working conditions, trade unions etc. can alter this.
    production->rate = ((1. / ((double)(baseProcessTime) / 1000.)) * 60.) * processingModifier;

    return true;
}

bool AutoComms::TargetCall_GetIslandConsumption(const uint64_t& islandID, std::vector<ConsumptionNode>* consumption)
{
    consumption->clear();

    SCOPE_LOCK(&shipServiceLock);

    auto addressIterator = islandConsumptionMap.find(islandID);

    if (addressIterator == islandConsumptionMap.end())
        return false;

    uint64_t address = islandConsumptionMap[islandID];

    uint64_t consumerListBase = ReadU64(address + 0x28);
    uint64_t consumerListSize = ReadU64(address + 0x30) & 0xFFFF;

    if (!consumerListBase)
        return false;

    for (uint64_t i = 0; i < consumerListSize; ++i)
    {
        ConsumptionNode node;

        node.resourceType = IDToResourceType(ReadU32(consumerListBase + i * 0x30));
        node.rate = 60. * ReadF32(consumerListBase + i * 0x30 + 0xC);

        consumption->push_back(node);
    }

    return true;
}

