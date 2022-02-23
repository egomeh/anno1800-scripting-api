#pragma once
#include <vector>
#include <inttypes.h>
#include <string>

struct Coordinate
{
    float x;
    float y;
};


struct CargoSlot
{
    uint64_t itemID;
    uint32_t volume;
};


struct SomeListyBoi
{
    std::vector<uint64_t> ids;
};


struct ShipCargo
{
    std::vector<CargoSlot> cargo;
};


struct Ship
{
    std::string name;
    ShipCargo cargo;
};

