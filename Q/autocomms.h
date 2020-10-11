#pragma once
#include <inttypes.h>
#include <vector>
#include <string>
#include "rda.h"

namespace AutoComms
{

enum class Area : uint64_t
{
    Invalid = 0,
    OldWorld = 1,
    NewWorld = 2,
    Arctic = 3,
};

enum class ShipType : uint64_t
{
    Invalid = 0,
    Schooner = 1,
    Gunboat = 2,
    Frigate = 3,
    Clipper = 4,
    ShipOfTheLine = 5,
    OilTanker = 6,
    CargoShip = 7,
    Monitor = 8,
    Battlecruiser = 9,
    CommandShip = 10,
};

struct GameTime
{
    uint64_t time;
    uint64_t frame;
};

struct Coordinate
{
    float x;
    float y;
};

struct ShipMoveData
{
    bool moving;
    Coordinate position;
    std::vector<Coordinate> waypoints;
};

struct ShipData
{
    uint64_t shipId;
    ShipType shipType;
    std::string name;
};

struct ShipCargoSlot
{
    bool hasCargo;
    uint64_t amount;
    Resource resourceType;
};

struct IslandResourceRecord
{
    Resource resourceType;
    uint64_t capacity;
    uint64_t amount;
};

struct IslandData
{
    uint64_t id;
    std::string name;
};

struct BuildingData
{
    uint64_t id;
    Building buidlingType;
    uint64_t rawBuildingTypeID;
    Coordinate position;
    uint64_t debug_address;
};

struct ProductionNode
{
    std::vector<Resource> input;
    Resource output;
    double rate;
};

struct ConsumptionNode
{
    Resource resourceType;
    double rate;
};


bool Serialize(const bool& data, std::vector<uint8_t>& stream);
bool Deserialize(bool *data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const uint8_t& data, std::vector<uint8_t>& stream);
bool Deserialize(uint8_t *data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const uint64_t& data, std::vector<uint8_t>& stream);
bool Deserialize(uint64_t *data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const float& data, std::vector<uint8_t>& stream);
bool Deserialize(float* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const double& data, std::vector<uint8_t>& stream);
bool Deserialize(double* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const double& data, std::vector<uint8_t>& stream);
bool Deserialize(double* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const std::string& data, std::vector<uint8_t>& stream);
bool Deserialize(std::string* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const Area& data, std::vector<uint8_t>& stream);
bool Deserialize(Area *data, const std::vector<uint8_t>& stream, size_t* offset);
bool Serialize(const ShipType& data, std::vector<uint8_t>& stream);
bool Deserialize(ShipType *data, const std::vector<uint8_t>& stream, size_t* offset);
bool Serialize(const Resource& data, std::vector<uint8_t>& stream);
bool Deserialize(Resource *data, const std::vector<uint8_t>& stream, size_t* offset);
bool Serialize(const Building& data, std::vector<uint8_t>& stream);
bool Deserialize(Building *data, const std::vector<uint8_t>& stream, size_t* offset);
bool Serialize(const GameTime& data, std::vector<uint8_t>& stream);
bool Deserialize(GameTime *data, const std::vector<uint8_t>& stream, size_t* offset);
bool Serialize(const Coordinate& data, std::vector<uint8_t>& stream);
bool Deserialize(Coordinate *data, const std::vector<uint8_t>& stream, size_t* offset);
bool Serialize(const ShipMoveData& data, std::vector<uint8_t>& stream);
bool Deserialize(ShipMoveData *data, const std::vector<uint8_t>& stream, size_t* offset);
bool Serialize(const ShipData& data, std::vector<uint8_t>& stream);
bool Deserialize(ShipData *data, const std::vector<uint8_t>& stream, size_t* offset);
bool Serialize(const ShipCargoSlot& data, std::vector<uint8_t>& stream);
bool Deserialize(ShipCargoSlot *data, const std::vector<uint8_t>& stream, size_t* offset);
bool Serialize(const IslandResourceRecord& data, std::vector<uint8_t>& stream);
bool Deserialize(IslandResourceRecord *data, const std::vector<uint8_t>& stream, size_t* offset);
bool Serialize(const IslandData& data, std::vector<uint8_t>& stream);
bool Deserialize(IslandData *data, const std::vector<uint8_t>& stream, size_t* offset);
bool Serialize(const BuildingData& data, std::vector<uint8_t>& stream);
bool Deserialize(BuildingData *data, const std::vector<uint8_t>& stream, size_t* offset);
bool Serialize(const ProductionNode& data, std::vector<uint8_t>& stream);
bool Deserialize(ProductionNode *data, const std::vector<uint8_t>& stream, size_t* offset);
bool Serialize(const ConsumptionNode& data, std::vector<uint8_t>& stream);
bool Deserialize(ConsumptionNode *data, const std::vector<uint8_t>& stream, size_t* offset);

template<typename T>
bool Serialize(const std::vector<T>& data, std::vector<uint8_t> &stream)
{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (const T& item : data)
    {
        if (!Serialize(item, stream))
            return false;
    }

    return true;
}


template<typename T>
bool Deserialize(std::vector<T>* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {
        T item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back(item);
    }

    return true;
}

}
