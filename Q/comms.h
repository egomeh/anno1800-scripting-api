#pragma once

#include <windows.h>
#include <vector>
#include <string>

struct ShipWaypointSetInputData
{
    std::vector<uint64_t> shipIds;
    float x;
    float y;
};

struct ShipTransferRegionInputData
{
    std::vector<uint64_t> shipIds;
    uint64_t regionId;
};

struct IslandResourceRecordRequst
{
    uint64_t islandId = 0;
    uint64_t resourceType = 0;
};

struct IslandResourceRecordResult
{
    uint64_t capacity = 0;
    uint64_t amount = 0;
};

struct ShipCargoSlot
{
    uint64_t hasCargo;
    uint64_t type;
    uint64_t amount;
};

struct ShipCargoResult
{
    std::vector<ShipCargoSlot> slots;
};

struct ShipCargoDumpRequest
{
    uint64_t shipId;
    uint64_t cargoSlotIndex;
    float x;
    float y;
};

struct ShipCargoLoadRequest
{
    uint64_t islandId;
    uint64_t shipId;
    uint64_t resourceId;
    uint64_t amount;
    uint64_t slotIndex;
    uint64_t offloading;
};

struct ShipMoveData
{
    uint64_t moves;
    float x;
    float y;
    float rotation;
};

struct BuildingBaseData
{
    uint64_t buildingType;
    uint64_t buildingId;
    float xCoord;
    float yCoord;
};

struct IslandBuildingData
{
    std::vector<BuildingBaseData> buildings;
};

bool Serialize(uint64_t data, std::vector<uint8_t>& stream);
bool Deserialize(uint64_t *data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(float data, std::vector<uint8_t>& stream);
bool Deserialize(float* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const ShipWaypointSetInputData& data, std::vector<uint8_t>& stream);
bool Deserialize(ShipWaypointSetInputData* data, std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const std::string& data, std::vector<uint8_t>& stream);
bool Deserialize(std::string* data, std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const std::vector<uint64_t>& data, std::vector<uint8_t>& stream);
bool Deserialize(std::vector<uint64_t>* data, std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const IslandResourceRecordRequst& data, std::vector<uint8_t>& stream);
bool Deserialize(IslandResourceRecordRequst* data, std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const IslandResourceRecordResult& data, std::vector<uint8_t>& stream);
bool Deserialize(IslandResourceRecordResult* data, std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const ShipCargoSlot& data, std::vector<uint8_t>& stream);
bool Deserialize(ShipCargoSlot* data, std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const ShipCargoResult& data, std::vector<uint8_t>& stream);
bool Deserialize(ShipCargoResult* data, std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const ShipCargoDumpRequest& data, std::vector<uint8_t>& stream);
bool Deserialize(ShipCargoDumpRequest* data, std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const ShipCargoLoadRequest& data, std::vector<uint8_t>& stream);
bool Deserialize(ShipCargoLoadRequest* data, std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const ShipMoveData& data, std::vector<uint8_t>& stream);
bool Deserialize(ShipMoveData* data, std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const BuildingBaseData& data, std::vector<uint8_t>& stream);
bool Deserialize(BuildingBaseData* data, std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const IslandBuildingData& data, std::vector<uint8_t>& stream);
bool Deserialize(IslandBuildingData* data, std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const ShipTransferRegionInputData& data, std::vector<uint8_t>& stream);
bool Deserialize(ShipTransferRegionInputData* data, std::vector<uint8_t>& stream, size_t* offset);
