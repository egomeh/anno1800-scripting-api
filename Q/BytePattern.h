#pragma once

#include <vector>
#include <string>
#include <optional>

#include "structs.h"

struct Cursor
{
    const char* position;
};

enum class ValueType
{
    UI64BIT,
    UI32BIT,
    UI8BIT,
};

struct ByteSequenceVariable
{
    std::string name;
    size_t offset;
    uint64_t size;
    bool bound;
};

struct Label
{
    std::string name;
    size_t offset;
};

struct SpecialLabel
{
    bool present;
    Label label;
};

struct ByteSequence
{
    std::vector<uint8_t> bytes;
    std::vector<ByteSequenceVariable> bindings;
    SpecialLabel skipAbove;
    SpecialLabel returnAddress;
};

bool ParseByte(Cursor* cursor, uint8_t& value);
bool ReadByteSequence(Cursor* cursor, ByteSequence* sequence);

bool Bind(ByteSequence* sequence, std::string name, uint8_t* valueBuffer);
bool Read(ByteSequence* sequence, std::string name, uint8_t* valueBuffer);

bool FindAndFill(ByteSequence* sequence, uint8_t* memory, size_t bytes, size_t* offset);

struct HookSequences
{
    ByteSequence pattern;
    ByteSequence replacement;
    ByteSequence detour;
    uint32_t nopPadding;
};

std::string ByteSequencesToString(const ByteSequence& sequence);

bool ParseHookAndDetour(HookSequences* sequences, const char* source, std::string& error);
bool DoHook(HookSequences* sequences, uint8_t* memory, size_t nBytes, uint64_t targetAddress, CodePutback* putback, MemoryAllocation* detourMemory);

