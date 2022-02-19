#pragma once
#include <vector>
#include <inttypes.h>

struct Coordinate
{
    float x;
    float y;
};


struct CargoSlot
{
    uint32_t item;
    int32_t volume;
};


struct SomeListyBoi
{
    std::vector<uint64_t> ids;
};


struct ShipCargo
{
    std::vector<CargoSlot> cargo;
};

