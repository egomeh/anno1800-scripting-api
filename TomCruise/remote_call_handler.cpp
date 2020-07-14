#include "pch.h"
#include "remote_call_handler.h"
#include "native_callbacks.h"
#include "native_traversal.h"
#include "native_calls.h"
#include "Q/comms.h"
#include "globals.h"

//bool RemoteInGetTime(uint64_t* output)
//{
//    *output = g_GameTime;
//    return true;
//}
//
//bool RemoteInSetShipsWaypoint(const ShipWaypointSetInputData& input)
//{
//    std::string error;
//
//    SEND_FORMATTED("Set ship waypiont: %llu ships -> (%g, %g)",  (uint64_t)input.shipIds.size(), input.x, input.y);
//
//    SetWaypointForShips(input.shipIds, input.x, input.y, error);
//
//    return true;
//}
//
//bool RemoteInExit()
//{
//    // Exit and stuff
//    KeepGoing = false;
//    return true;
//}
//
//bool RemoteInGetShipName(const uint64_t& input, std::string* output)
//{
//    SEND_FORMATTED("Get shipname command: %llx");
//    bool result = GetShipName(input, output);
//    SEND_FORMATTED("Return: %s", output->c_str());
//    return result;
//}
//
//bool RemoteInGetShipsByName(const std::string& input, std::vector<uint64_t>* output)
//{
//    SEND_FORMATTED("Getting ships with names \"%s\"", input.c_str());
//    bool result = GetShipsByName(input, output);
//
//    if (output->empty())
//        SEND_FORMATTED("No ships found");
//
//    for (uint64_t id : *output)
//    {
//        SEND_FORMATTED("Found ship %llx", id);
//    }
//
//    return result;
//}
//
//bool RemoteInGetAllShipsInWorld(const uint64_t& input, std::vector<uint64_t>* output)
//{
//    return GetAllShipsInWorld(input, output);
//}
//
//bool RemoteInGetIslandsByName(const std::string& input, std::vector<uint64_t>* output)
//{
//    output->clear();
//
//    EnterCriticalSection(&shipServiceLock);
//    bool success = GetIslandIDsByName(input, output);
//    LeaveCriticalSection(&shipServiceLock);
//
//    return success;
//}
//
//bool RemoteInGetResourceFromIsland(const IslandResourceRecordRequst& input, IslandResourceRecordResult* output)
//{
//    EnterCriticalSection(&shipServiceLock);
//    bool success = GetIslandResource(input, output);
//    LeaveCriticalSection(&shipServiceLock);
//
//    return success;
//}
//
//bool RemoteInGetShipCargo(const uint64_t& input, ShipCargoResult* output)
//{
//    output->slots.clear();
//
//    uint64_t shipBase = 0;
// 
//    if (!GetShipById(input, &shipBase))
//        return false;
//
//    uint64_t cargoPtr = VirtualShipGetComponent(shipBase, ComponentIdCargo);
//
//    if (!cargoPtr)
//        return false;
//
//    uint64_t goodsPtr = cargoPtr + 0x28;
//
//    uint64_t goodsAddress = ReadU64(goodsPtr);
//
//    uint64_t slotsPtr = goodsAddress + 0x10;
//    uint64_t nSlotsPtr = goodsAddress + 0x18;
//
//    uint32_t nSlots = ReadU32(nSlotsPtr);
//    uint64_t slotsBaseAddress = ReadU64(slotsPtr);
//
//    SEND_FORMATTED("%d", nSlots);
//
//    SEND_FORMATTED("slots base %llx", slotsBaseAddress);
//
//    for (uint64_t i = 0; i < nSlots; ++i)
//    {
//        uint64_t slotBase = slotsBaseAddress + i * 0x28;
//
//        uint64_t hasCargoPtr = slotBase + 0x10;
//
//        uint64_t hasCargo = ReadU64(hasCargoPtr) & 0x00000000000000FF;
//
//        ShipCargoSlot slot = { 0 };
//
//        slot.hasCargo = hasCargo;
//
//        if (slot.hasCargo == 0)
//        {
//            SEND_FORMATTED("Cargo %d is empty", i);
//            output->slots.push_back(slot);
//            continue;
//        }
//
//        uint64_t amountPtr = slotBase + 0x20;
//
//        uint64_t amount = ReadU64(amountPtr) & 0x00000000FFFFFFFF;
//
//        uint64_t typeStructPtr = slotBase + 0x8;
//
//        uint64_t typeStructBase = ReadU64(typeStructPtr);
//
//        if (!typeStructBase)
//            return false;
//
//        uint64_t typePtr = typeStructBase + 0x8;
//        uint64_t type = ReadU64(typePtr) & 0x00000000FFFFFFFF;
//
//        slot.type = type;
//        slot.amount = amount;
//
//        SEND_FORMATTED("Cargo %llx, %llx, %llx", slot.hasCargo, slot.type, slot.amount);
//
//        output->slots.push_back(slot);
//    }
//
//    return true;
//}
//
//bool RemoteInDumpCargo(const ShipCargoDumpRequest& input)
//{
//    DumpShipCargo(input.shipId, input.cargoSlotIndex, input.x, input.y);
//    return true;
//}
//
//bool RemoteInLoadCargo(const ShipCargoLoadRequest& input)
//{
//    LoadShipCargo(input.islandId, input.shipId, input.resourceId, input.amount, input.slotIndex, input.offloading);
//    return true;
//}
//
//bool RemoteInGetShipMoveData(const uint64_t& input, ShipMoveData* output)
//{
//    uint64_t shipAddress;
//
//    EnterCriticalSection(&shipServiceLock);
//    SCOPE_GUARD(LeaveCriticalSection(&shipServiceLock));
//
//    if (!GetShipById(input, &shipAddress))
//        return false;
//
//    uint64_t component = VirtualShipGetComponent(shipAddress, ComponentIdMoveData);
//
//    if (!component)
//        return false;
//
//    output->moves = ReadU32(component + 0x28);
//    output->x = ReadF32(component + 0x34);
//    output->y = ReadF32(component + 0x38);
//    output->rotation = ReadF32(component + 0x3c);
//
//    return true;
//}
//
//bool RemoteInGetIslandBuildings(const uint64_t& input, IslandBuildingData* output)
//{
//    output->buildings.clear();
//
//    BuildingBaseData data;
//
//    // SCOPE_LOCK(&shipServiceLock);
//
//    uint64_t islandBaseAddress = 0;
//
//    SEND_FORMATTED("Looking for island %llx", input);
//
//    if (!GetIslandById(input, &islandBaseAddress))
//        return false;
//
//    uint64_t buildingListSizePtr = islandBaseAddress + 0x88;
//    uint64_t buildingListPtr = islandBaseAddress + 0x90;
//
//    uint64_t listSize = ReadU64(buildingListSizePtr);
//    uint64_t buildingListBase = ReadU64(buildingListPtr);
//
//    SEND_FORMATTED("found base of island %llx", islandBaseAddress);
//
//    SEND_FORMATTED("listSize %llu", listSize);
//    SEND_FORMATTED("listPtr %llx", buildingListPtr);
//
//    std::set<uint64_t> preCollectionList;
//
//    for (uint64_t i = 0; i < listSize; ++i)
//    {
//        uint64_t ptr = ReadU64(buildingListBase + i * 8);
//
//        while (ptr)
//        {
//            preCollectionList.insert(ptr);
//            ptr = ReadU64(ptr);
//        }
//    }
//
//    std::set<uint64_t> secondCollectionList;
//
//    for (uint64_t address : preCollectionList)
//    {
//        uint64_t secondSize = ReadU32(address + 0x38);
//        uint64_t secondPassList = ReadU64(address + 0x40);
//
//        if (!secondPassList)
//            continue;
//
//        SEND_FORMATTED("%llx", secondPassList);
//
//        if (!IsReadablePointer(secondPassList))
//            continue;
//
//        for (uint64_t i = 0; i < secondSize; ++i)
//        {
//            if (!IsReadablePointer(secondPassList + i * 8))
//                continue;
//
//            uint64_t ptr = ReadU64(secondPassList + i * 8);
//
//            //nt64_t counter = 0;
//            while (ptr)// && counter < 128)
//            {
//               //+counter;
//                if (!IsReadablePointer(ptr + 0x10))
//                    break;
//
//                uint64_t test1 = ReadU64(ptr + 0x10);
//                uint64_t currentPtr = ptr;
//                ptr = ReadU64(ptr);
//
//                test1 = (0x0000FFFF00000000 & test1) >> 0x20;
//
//                if (test1 != input)
//                    continue;
//
//                uint64_t baseAddressPtr = currentPtr + 0x18;
//
//                if (!IsReadablePointer(baseAddressPtr))
//                {
//                    SEND_FORMATTED("could not read at address %lls", baseAddressPtr);
//                    continue;
//                }
//
//                uint64_t buildingAddress = ReadU64(baseAddressPtr);
//                secondCollectionList.insert(buildingAddress);
//            }
//        }
//    }
//
//    for (uint64_t buildingAddress : secondCollectionList)
//    {
//        BuildingBaseData data;
//
//        data.buildingType = ReadU32(buildingAddress + 0x8);
//        data.buildingId = ReadU32(buildingAddress + 0x28) & 0xFFFFFFFF;
//        data.xCoord = ReadF32(buildingAddress + 0x3c);
//        data.yCoord = ReadF32(buildingAddress + 0x44);
//
//        // END_FORMATTED("%llx - %llx %llx %f %f", buildingAddress, data.buildingId, data.buildingType, data.xCoord, data.yCoord);
//
//        output->buildings.push_back(data);
//    }
//
//    return true;
//}
//
//bool RemoteInTransferShipsToWorld(const ShipTransferRegionInputData& input)
//{
//    std::string error;
//
//    SEND_FORMATTED("Moving ships to new region world: %llu ships -> %llu", (uint64_t)input.shipIds.size(), input.regionId);
//
//    return TransferShipsToWorlds(input.shipIds, input.regionId);
//}
