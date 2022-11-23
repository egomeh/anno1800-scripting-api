#pragma once
#include <vector>
#include <inttypes.h>
#include <string>

struct IslandResource
{
    int32_t type_id;
    std::string name;
    int32_t amount;
    int32_t capacity;
    uint64_t amount_ptr;
};


struct ResourceConsumption
{
    int32_t type_id;
    std::string name;
    float rate;
};


struct IslandInfo
{
    uint32_t island_id;
    std::string name;
    uint64_t debug_address;
};


struct ShipInfo
{
    uint32_t id;
    uint64_t debug_address;
};

