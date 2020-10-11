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

enum class Resource : uint64_t
{
    Invalid = 0,
    Grain = 1,
    Beef = 2,
    Hops = 3,
    Potatoes = 4,
    Wood = 5,
    Wool = 6,
    Pigs = 7,
    Fur = 8,
    Grapes = 9,
    Paprika = 10,
    Fish = 11,
    Salpeter = 12,
    QuartzSand = 13,
    Concrete = 14,
    Soap = 15,
    Timber = 16,
    Bricks = 17,
    FurCoats = 18,
    Window = 19,
    LightBulbs = 20,
    Sailcloth = 21,
    Chassis = 22,
    Clay = 23,
    SewingMachines = 24,
    Bread = 25,
    Beer = 26,
    Goulash = 27,
    CannedFood = 28,
    Schnapps = 29,
    Sausage = 30,
    Champagne = 31,
    Beams = 32,
    Steel = 33,
    Weapons = 34,
    Dynamite = 35,
    AdvancedWeapons = 36,
    SteamMotors = 37,
    SteamCarriages = 38,
    Brass = 39,
    Coal = 40,
    Iron = 41,
    Zinc = 42,
    Copper = 43,
    Cement = 44,
    GoldOre = 45,
    Tallow = 46,
    Flour = 47,
    Malt = 48,
    BasicClothes = 49,
    Glass = 50,
    Inlay = 51,
    CarbonFilament = 52,
    HighWheeler = 53,
    PocketWatch = 54,
    Glasses = 55,
    Phonographs = 56,
    Gold = 57,
    Jewelry = 58,
    SugarCane = 59,
    Tobacco = 60,
    Cotton = 61,
    Cocoa = 62,
    Caoutchouc = 63,
    CoffeeBeans = 64,
    Corn = 65,
    AlpacaWool = 66,
    Bananas = 67,
    Pearls = 68,
    FishOil = 69,
    Ponchos = 70,
    Felt = 71,
    Bombin = 72,
    Rum = 73,
    Chocolate = 74,
    Coffee = 75,
    FriedBananas = 76,
    Burrito = 77,
    Sugar = 78,
    CottonCloth = 79,
    Cigars = 80,
    StoneCoal = 81,
    Oil = 82,
    Electricity = 83,
    CaribouMeat = 84,
    GooseFeathers = 85,
    BearFur = 86,
    Husky = 87,
    WhaleOil = 88,
    SealSkin = 89,
    Pemmican = 90,
    OilLamps = 91,
    SleepingBags = 92,
    Parka = 93,
    SledFrames = 94,
    Sleds = 95,
    Gas = 96,
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
