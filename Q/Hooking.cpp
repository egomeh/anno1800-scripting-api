#include "pch.h"
#include "Hooking.h"
#include "BytePattern.h"

bool PrepareHook(Hook* hook, uint64_t targetFunction, const char* hookScript, std::string& error)
{
    if (!ParseHookAndDetour(&hook->sequences, hookScript, error))
        return false;

    if (!Bind(&hook->sequences.detour, "targetAddress", (uint8_t*)&targetFunction))
    {
        error = "Could not bind target addres";
        return false;
    }

    hook->prepared = true;
    return true;
}

bool LocatePatternInRegions(ByteSequence* pattern, MemoryRegion* regions, size_t nRegions, uint64_t expectedAddress, LocationResult* result)
{
    bool found = false;

    // Fast track, if we think we know where the code is, we might be able to shortcut the search.
    for (size_t i = 0; i < nRegions; ++i)
    {
        uint64_t from = regions[i].baseAddress;
        uint64_t size = regions[i].size;
        uint64_t to = from + size;

        // The expected address is in this region
        if (from <= expectedAddress && expectedAddress <= to)
        {
            DWORD oldProtect;
            DWORD newProtect = PAGE_EXECUTE_READWRITE;

            if (!VirtualProtect((LPVOID)from, size, newProtect, &oldProtect))
                return false;

            size_t offset;
            found = FindAndFill(pattern, (uint8_t*)expectedAddress, to - expectedAddress, &offset);

            if (found)
            {
                result->foundAddress = expectedAddress + offset;
                result->region = regions[i];
            }

            if (!found)
            {
                found = FindAndFill(pattern, (uint8_t*)expectedAddress, to - expectedAddress, &offset);
                result->foundAddress = expectedAddress + offset;
                result->region = regions[i];
            }

            if (!VirtualProtect((LPVOID)from, size, newProtect, &oldProtect))
                return false;
        }
    }

    if (found)
        return true;

    // Search all the executable memory
    for (size_t i = 0; i < nRegions && !found; ++i)
    {
        uint64_t from = regions[i].baseAddress;
        uint64_t size = regions[i].size;
        uint64_t to = from + size;

        DWORD oldProtect;
        DWORD newProtect = PAGE_EXECUTE_READWRITE;

        if (!VirtualProtect((LPVOID)from, size, newProtect, &oldProtect))
            return false;

        size_t offset;
        found = FindAndFill(pattern, (uint8_t*)from, size, &offset);

        if (found)
        {
            result->foundAddress = from + offset;
            result->region = regions[i];
        }

        if (!VirtualProtect((LPVOID)from, size, newProtect, &oldProtect))
            return false;
    }

    return found;
}

bool LocateHook(Hook* hook, MemoryRegion* regions, size_t nRegions, uint64_t expectedAddress)
{
    LocationResult result;
    if (!LocatePatternInRegions(&hook->sequences.pattern, regions, nRegions, expectedAddress, &result))
        return false;

    hook->located = true;
    hook->targetAddress = result.foundAddress;
    hook->region = result.region;

    return true;
}

bool EnableHook(Hook* hook, HANDLE processHandle)
{
    if (!hook->located)
        return false;

    if (hook->enabled)
        return true;

    void* detourAddress = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    hook->allocations.push_back((uint64_t)detourAddress);
    hook->originalCode = hook->sequences.pattern.bytes;

    uint64_t returnAddress = hook->targetAddress + hook->sequences.replacement.bytes.size();

    if (hook->sequences.replacement.returnAddress.present)
        returnAddress = hook->targetAddress + hook->sequences.replacement.returnAddress.label.offset;

    Bind(&hook->sequences.replacement, "detourAddress", (uint8_t*)&detourAddress);
    Bind(&hook->sequences.detour, "returnAddress", (uint8_t*)&returnAddress);

    for (auto it = hook->sequences.pattern.bindings.begin(); it < hook->sequences.pattern.bindings.end(); ++it)
    {
        std::vector<uint8_t> buffer;
        buffer.resize(it->size);
        Read(&hook->sequences.pattern, it->name, buffer.data());

        Bind(&hook->sequences.replacement, it->name, buffer.data());
        Bind(&hook->sequences.detour, it->name, buffer.data());
    }

    DWORD oldProtect;
    DWORD newProtect = PAGE_EXECUTE_READWRITE;

    if (!VirtualProtect((LPVOID)hook->region.baseAddress, hook->region.size, newProtect, &oldProtect))
        return false;

    memcpy(detourAddress, hook->sequences.detour.bytes.data(), hook->sequences.detour.bytes.size());
    memcpy((uint8_t*)hook->targetAddress, hook->sequences.replacement.bytes.data(), hook->sequences.replacement.bytes.size());

    if (!VirtualProtect((LPVOID)hook->region.baseAddress, hook->region.size, oldProtect, &newProtect))
        return false;

    FlushInstructionCache(processHandle, (LPVOID)hook->region.baseAddress, (SIZE_T)hook->region.size);

    hook->enabled = true;
}

bool DisableHook(Hook* hook, HANDLE processHandle, std::string& error)
{
    if (!hook->located)
    {
        error = "hook not located";
        return false;
    }

    if (!hook->enabled)
        return true;

    DWORD oldProtect;
    DWORD newProtect = PAGE_EXECUTE_READWRITE;

    if (!VirtualProtect((LPVOID)hook->region.baseAddress, hook->region.size, newProtect, &oldProtect))
    {
        error = "Could not modify hook region memory protection";
        return false;
    }

    memcpy((uint8_t*)hook->targetAddress, hook->originalCode.data(), hook->originalCode.size());

    if (!VirtualProtect((LPVOID)hook->region.baseAddress, hook->region.size, oldProtect, &newProtect))
    {
        error = "Could not reset hook region memory protection";
        return false;
    }

    FlushInstructionCache(processHandle, (LPVOID)hook->region.baseAddress, (SIZE_T)hook->region.size);

    hook->enabled = false;

    return true;
}

