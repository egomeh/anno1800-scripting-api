#pragma once
#include <vector>
#include <inttypes.h>
#include <string>

struct IslandResource
{
    int32_t type_id;
    int32_t amount;
    int32_t capacity;
};


struct IslandInfo
{
    uint32_t island_id;
    std::string name;
    uint64_t debug_address;
};

