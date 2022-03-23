#pragma once

#include "global_includes.h"
#include "tools.h"
#include <vector>

class MemoryReplacement
{
public:
    MemoryReplacement();
    ~MemoryReplacement();

    void SetMemory(const std::vector<uint8_t>& bytes);
    void Emplace(void* address);
    void Undo();

private:
    std::vector<uint8_t> memory;
    std::vector<uint8_t> old_memory;
    void* placement_address;
    bool placed;
};
