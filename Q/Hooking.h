#pragma once

#include <inttypes.h>
#include "BytePattern.h"
#include "Structs.h"

struct Hook
{
    bool prepared = false;
    bool located = false;
    bool enabled = false;
    uint64_t targetAddress;
    HookSequences sequences;
    std::vector<uint64_t> allocations;
    std::vector<uint8_t> originalCode;
    std::string name;
    MemoryRegion region;
};

struct LocationResult
{
    uint64_t foundAddress;
    MemoryRegion region;
};

bool LocatePatternInRegions(ByteSequence* pattern, MemoryRegion* regions, size_t nRegions, uint64_t expectedAddress, LocationResult* result);

bool PrepareHook(Hook* hook, uint64_t targetFunction, const char* hookScript, std::string& error);
bool LocateHook(Hook* hook, MemoryRegion* regions, size_t nRegions, uint64_t expectedAddress);
bool EnableHook(Hook* hook, HANDLE processHandle);
bool DisableHook(Hook* hook, HANDLE processHandle, std::string& error);

struct NativeCallSetup
{
    bool prepared;
    std::string name;
    uint64_t allocation;
    ByteSequence nativeCodePattern;
    ByteSequence setupCode;
};
