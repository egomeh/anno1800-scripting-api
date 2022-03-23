#include "memory.h"

MemoryReplacement::MemoryReplacement() : placement_address(nullptr), placed(false)
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

    DWORD thisThread = GetCurrentThreadId();

    ForEachThreadInProcess([&](DWORD id, HANDLE thread)
    {
        if (id == thisThread)
            return;

        SuspendThread(thread);

        CONTEXT context = {};
        context.ContextFlags |= CONTEXT_ALL;

        if (!GetThreadContext(thread, &context))
            return;

        uint64_t rip = context.Rip;
        while (rip >= (uint64_t)address && rip <= (((uint64_t)address) + memory.size()))
        {
            ResumeThread(thread);
            Sleep(1);
            SuspendThread(thread);

            context = {};
            context.ContextFlags |= CONTEXT_ALL;
            if (!GetThreadContext(thread, &context))
                return;

            rip = context.Rip;
        }
    });

    // Write the data to the location we want
    memcpy(address, memory.data(), memory.size());

    ForEachThreadInProcess([&](DWORD id, HANDLE thread)
    {
        if (id == thisThread)
            return;

        ResumeThread(thread);
    });

    placement_address = address;

    VirtualProtect(address, 0x1000, old_protection, &dummy_protection);

    placed = true;
}

void MemoryReplacement::Undo()
{
    if (!placed)
        return;

    DWORD old_protection;
    DWORD dummy_protection;
    VirtualProtect(placement_address, 0x1000, PAGE_EXECUTE_READWRITE, &old_protection);

    DWORD thisThread = GetCurrentThreadId();

    ForEachThreadInProcess([&](DWORD id, HANDLE thread)
    {
        if (id == thisThread)
            return;

        SuspendThread(thread);

        CONTEXT context = {};
        context.ContextFlags |= CONTEXT_ALL;

        if (!GetThreadContext(thread, &context))
            return;

        uint64_t rip = context.Rip;
        while (rip >= (uint64_t)placement_address && rip <= (((uint64_t)placement_address) + old_memory.size()))
        {
            ResumeThread(thread);
            Sleep(1);
            SuspendThread(thread);

            if (!GetThreadContext(thread, &context))
                return;

            rip = context.Rip;
        }
    });

    // Write the data to the location we want
    memcpy(placement_address, old_memory.data(), memory.size());

    ForEachThreadInProcess([&](DWORD id, HANDLE thread)
    {
        if (id == thisThread)
            return;

        ResumeThread(thread);
    });

    VirtualProtect(placement_address, 0x1000, old_protection, &dummy_protection);

    placed = false;
}
