#pragma once
#include <windows.h>
#include "Q/Tools.h"
#include "Q/autocomms.h"
#include "Q/remote_call_shared.h"
#include "globals.h"

namespace AutoComms
{

bool TargetCall_GetGameTime(GameTime* gameTime);
bool TargetCall_WriteLineToLog(const std::string& message);
bool TargetCall_WriteLinesToLog(const std::vector<std::string>& messages);
bool TargetCall_GetShipById(const uint64_t& id, ShipData* shipData);
bool TargetCall_GetShipComponentAddress(const uint64_t& shipID, const uint64_t& componentID, uint64_t* address);
bool TargetCall_GetShipMoveData(const uint64_t& id, ShipMoveData* moveData);
bool TargetCall_GetBuildingComponentAddress(const uint64_t& islandID, const uint64_t& buildingID, const uint64_t& componentID, uint64_t* address);
bool TargetCall_GetAllShips(const Area& area, std::vector<ShipData>* ships);
bool TargetCall_GetShipAddress(const uint64_t& shipID, uint64_t* address);
bool TargetCall_GetShipCargo(const uint64_t& shipID, std::vector<ShipCargoSlot>* cargo);
bool TargetCall_AddWaypoint(const std::vector<uint64_t>& IDs, const Coordinate& waypoint);
bool TargetCall_GetAllIslands(std::vector<IslandData>* islands);
bool TargetCall_GetIslandsByName(const std::string& name, std::vector<IslandData>* islands);
bool TargetCall_GetIslandResources(const uint64_t& islandID, std::vector<IslandResourceRecord>* resources);
bool TargetCall_GetIslandBuildings(const uint64_t& islandID, std::vector<BuildingData>* buildings);
bool TargetCall_ShipDumpCargo(const uint64_t& shipID, const uint64_t& slotIndex);
bool TargetCall_ShipDumpCargo(const uint64_t& shipID, const uint64_t& slotIndex, const Coordinate& position);
bool TargetCall_LoadCargoToShip(const uint64_t& islandID, const uint64_t& shipID, const Resource& resource, const uint64_t& amount, const uint64_t& slotIndex);
bool TargetCall_OffloadCargoFromShip(const uint64_t& islandID, const uint64_t& shipID, const uint64_t& amount, const uint64_t& slotIndex);
bool TargetCall_GetBuildingProduction(const uint64_t& islandID, const uint64_t& buildingID, ProductionNode* production);
bool TargetCall_GetIslandConsumption(const uint64_t& islandID, std::vector<ConsumptionNode>* consumption);

inline bool HandleScriptCall(SOCKET socket)
{
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> outputBuffer;

    if (!ReadMessageFromSocket(socket, buffer))
        return false;

    outputBuffer.clear();

    size_t offset = 0;

    uint64_t message_size = 0;
    uint64_t function_id = 0;

    if (!AutoComms::Deserialize(&message_size, buffer, &offset))
        return false;

    if (!AutoComms::Deserialize(&function_id, buffer, &offset))
        return false;

    bool success = false;

    switch (function_id)
    {
        case 1: // GetGameTime
        {
            GameTime gameTime;

            success = TargetCall_GetGameTime(&gameTime);

            if (!AutoComms::Serialize(gameTime, outputBuffer))
                return false;

            break;
        }

        case 2: // WriteLineToLog
        {
            std::string message;
            if (!AutoComms::Deserialize(&message, buffer, &offset))
                return false;


            success = TargetCall_WriteLineToLog(message);


            break;
        }

        case 3: // WriteLinesToLog
        {
            std::vector<std::string> messages;
            if (!AutoComms::Deserialize(&messages, buffer, &offset))
                return false;


            success = TargetCall_WriteLinesToLog(messages);


            break;
        }

        case 4: // GetShipById
        {
            uint64_t id;
            if (!AutoComms::Deserialize(&id, buffer, &offset))
                return false;

            ShipData shipData;

            success = TargetCall_GetShipById(id, &shipData);

            if (!AutoComms::Serialize(shipData, outputBuffer))
                return false;

            break;
        }

        case 5: // GetShipComponentAddress
        {
            uint64_t shipID;
            if (!AutoComms::Deserialize(&shipID, buffer, &offset))
                return false;

            uint64_t componentID;
            if (!AutoComms::Deserialize(&componentID, buffer, &offset))
                return false;

            uint64_t address;

            success = TargetCall_GetShipComponentAddress(shipID, componentID, &address);

            if (!AutoComms::Serialize(address, outputBuffer))
                return false;

            break;
        }

        case 6: // GetShipMoveData
        {
            uint64_t id;
            if (!AutoComms::Deserialize(&id, buffer, &offset))
                return false;

            ShipMoveData moveData;

            success = TargetCall_GetShipMoveData(id, &moveData);

            if (!AutoComms::Serialize(moveData, outputBuffer))
                return false;

            break;
        }

        case 7: // GetBuildingComponentAddress
        {
            uint64_t islandID;
            if (!AutoComms::Deserialize(&islandID, buffer, &offset))
                return false;

            uint64_t buildingID;
            if (!AutoComms::Deserialize(&buildingID, buffer, &offset))
                return false;

            uint64_t componentID;
            if (!AutoComms::Deserialize(&componentID, buffer, &offset))
                return false;

            uint64_t address;

            success = TargetCall_GetBuildingComponentAddress(islandID, buildingID, componentID, &address);

            if (!AutoComms::Serialize(address, outputBuffer))
                return false;

            break;
        }

        case 8: // GetAllShips
        {
            Area area;
            if (!AutoComms::Deserialize(&area, buffer, &offset))
                return false;

            std::vector<ShipData> ships;

            success = TargetCall_GetAllShips(area, &ships);

            if (!AutoComms::Serialize(ships, outputBuffer))
                return false;

            break;
        }

        case 9: // GetShipAddress
        {
            uint64_t shipID;
            if (!AutoComms::Deserialize(&shipID, buffer, &offset))
                return false;

            uint64_t address;

            success = TargetCall_GetShipAddress(shipID, &address);

            if (!AutoComms::Serialize(address, outputBuffer))
                return false;

            break;
        }

        case 10: // GetShipCargo
        {
            uint64_t shipID;
            if (!AutoComms::Deserialize(&shipID, buffer, &offset))
                return false;

            std::vector<ShipCargoSlot> cargo;

            success = TargetCall_GetShipCargo(shipID, &cargo);

            if (!AutoComms::Serialize(cargo, outputBuffer))
                return false;

            break;
        }

        case 11: // AddWaypoint
        {
            std::vector<uint64_t> IDs;
            if (!AutoComms::Deserialize(&IDs, buffer, &offset))
                return false;

            Coordinate waypoint;
            if (!AutoComms::Deserialize(&waypoint, buffer, &offset))
                return false;


            success = TargetCall_AddWaypoint(IDs, waypoint);


            break;
        }

        case 12: // GetIslandsByName
        {
            std::string name;
            if (!AutoComms::Deserialize(&name, buffer, &offset))
                return false;

            std::vector<IslandData> islands;

            success = TargetCall_GetIslandsByName(name, &islands);

            if (!AutoComms::Serialize(islands, outputBuffer))
                return false;

            break;
        }

        case 13: // GetIslandResources
        {
            uint64_t islandID;
            if (!AutoComms::Deserialize(&islandID, buffer, &offset))
                return false;

            std::vector<IslandResourceRecord> resources;

            success = TargetCall_GetIslandResources(islandID, &resources);

            if (!AutoComms::Serialize(resources, outputBuffer))
                return false;

            break;
        }

        case 14: // GetIslandBuildings
        {
            uint64_t islandID;
            if (!AutoComms::Deserialize(&islandID, buffer, &offset))
                return false;

            std::vector<BuildingData> buildings;

            success = TargetCall_GetIslandBuildings(islandID, &buildings);

            if (!AutoComms::Serialize(buildings, outputBuffer))
                return false;

            break;
        }

        case 15: // ShipDumpCargo
        {
            uint64_t shipID;
            if (!AutoComms::Deserialize(&shipID, buffer, &offset))
                return false;

            uint64_t slotIndex;
            if (!AutoComms::Deserialize(&slotIndex, buffer, &offset))
                return false;


            success = TargetCall_ShipDumpCargo(shipID, slotIndex);


            break;
        }

        case 16: // ShipDumpCargo
        {
            uint64_t shipID;
            if (!AutoComms::Deserialize(&shipID, buffer, &offset))
                return false;

            uint64_t slotIndex;
            if (!AutoComms::Deserialize(&slotIndex, buffer, &offset))
                return false;

            Coordinate position;
            if (!AutoComms::Deserialize(&position, buffer, &offset))
                return false;


            success = TargetCall_ShipDumpCargo(shipID, slotIndex, position);


            break;
        }

        case 17: // LoadCargoToShip
        {
            uint64_t islandID;
            if (!AutoComms::Deserialize(&islandID, buffer, &offset))
                return false;

            uint64_t shipID;
            if (!AutoComms::Deserialize(&shipID, buffer, &offset))
                return false;

            Resource resource;
            if (!AutoComms::Deserialize(&resource, buffer, &offset))
                return false;

            uint64_t amount;
            if (!AutoComms::Deserialize(&amount, buffer, &offset))
                return false;

            uint64_t slotIndex;
            if (!AutoComms::Deserialize(&slotIndex, buffer, &offset))
                return false;


            success = TargetCall_LoadCargoToShip(islandID, shipID, resource, amount, slotIndex);


            break;
        }

        case 18: // OffloadCargoFromShip
        {
            uint64_t islandID;
            if (!AutoComms::Deserialize(&islandID, buffer, &offset))
                return false;

            uint64_t shipID;
            if (!AutoComms::Deserialize(&shipID, buffer, &offset))
                return false;

            uint64_t amount;
            if (!AutoComms::Deserialize(&amount, buffer, &offset))
                return false;

            uint64_t slotIndex;
            if (!AutoComms::Deserialize(&slotIndex, buffer, &offset))
                return false;


            success = TargetCall_OffloadCargoFromShip(islandID, shipID, amount, slotIndex);


            break;
        }

        case 19: // GetBuildingProduction
        {
            uint64_t islandID;
            if (!AutoComms::Deserialize(&islandID, buffer, &offset))
                return false;

            uint64_t buildingID;
            if (!AutoComms::Deserialize(&buildingID, buffer, &offset))
                return false;

            ProductionNode production;

            success = TargetCall_GetBuildingProduction(islandID, buildingID, &production);

            if (!AutoComms::Serialize(production, outputBuffer))
                return false;

            break;
        }

        case 20: // GetIslandConsumption
        {
            uint64_t islandID;
            if (!AutoComms::Deserialize(&islandID, buffer, &offset))
                return false;

            std::vector<ConsumptionNode> consumption;

            success = TargetCall_GetIslandConsumption(islandID, &consumption);

            if (!AutoComms::Serialize(consumption, outputBuffer))
                return false;

            break;
        }

        case 21: // GetAllIslands
        {
            std::vector<IslandData> islands;

            success = TargetCall_GetAllIslands(&islands);

            if (!AutoComms::Serialize(islands, outputBuffer))
                return false;

            break;
        }

    default:
        break;
    }

    std::vector<uint8_t> finalBuffer;
    uint64_t payloadSize = outputBuffer.size();
    uint64_t totalSize = payloadSize + 8 + 1;

    if (!AutoComms::Serialize(totalSize, finalBuffer))
        return false;

    if (!AutoComms::Serialize(success, finalBuffer))
        return false;

    finalBuffer.insert(finalBuffer.end(), outputBuffer.begin(), outputBuffer.end());

    int bytesSent = send(socket, (char*)finalBuffer.data(), (int)finalBuffer.size(), 0);

    if (bytesSent == SOCKET_ERROR)
        return false;

    return true;
}

}

