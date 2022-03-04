#include "memory.h"

MemoryReplacement::MemoryReplacement() : placement_address(nullptr)
{
}

MemoryReplacement::~MemoryReplacement()
{
    Undo();
}

void MemoryReplacement::SetMemory(const std::vector<uint8_t>& bytes)
{
    if (memory.size() > 0)
        return;

    memory.resize(bytes.size());

    for (size_t i = 0; i < bytes.size(); ++i)
        memory[i] = bytes[i];
}

void MemoryReplacement::Emplace(void* address)
{
    DWORD old_protection;
    DWORD dummy_protection;
    VirtualProtect(address, 0x1000, PAGE_EXECUTE_READWRITE, &old_protection);

    old_memory.resize(memory.size());

    // Copy the current data to our local storage
    memcpy(old_memory.data(), address, memory.size());

    // Write the data to the location we want
    memcpy(address, memory.data(), memory.size());

    placement_address = address;

    VirtualProtect(address, 0x1000, old_protection, &dummy_protection);
}

void MemoryReplacement::Undo()
{
    DWORD old_protection;
    DWORD dummy_protection;
    VirtualProtect(placement_address, 0x1000, PAGE_EXECUTE_READWRITE, &old_protection);

    // Write the data to the location we want
    memcpy(placement_address, old_memory.data(), memory.size());

    VirtualProtect(placement_address, 0x1000, old_protection, &dummy_protection);
}
