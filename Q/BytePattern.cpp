#include "pch.h"
#include "BytePattern.h"

#include <ctype.h>
#include <stdio.h>
#include <string>

#include <stdio.h>

const static char ValidCharacters[] = "0123456789ABCDEF";

bool CharacterToByteValue(const char literal, uint8_t& value)
{
    if (literal >= '0' && literal <= '9')
    {
        value = literal - '0';
        return true;
    }

    if (literal >= 'A' && literal <= 'F')
    {
        value = literal - 'A' + 0xA;
        return true;
    }

    if (literal >= 'a' && literal <= 'f')
    {
        value = literal - 'a' + 0xA;
        return true;
    }

    return false;
}

void IgnoreZeroX(Cursor* cursor)
{
    char first = *cursor->position;
    char second = *(cursor->position + 1);

    if (first != '0')
        return;

    if (!(second == 'x' || second== 'X'))
        return;

    cursor->position += 2;
}

bool Consume(Cursor* cursor, const char* sequence)
{
    size_t sequenceLength = strlen(sequence);
    if (memcmp(cursor->position, sequence, sequenceLength) == 0)
        cursor->position += sequenceLength;
    else
        return false;

    return true;
}

void IgnoreWhiteSpace(Cursor* cursor)
{
    while (isspace(*cursor->position))
        ++cursor->position;

    while (Consume(cursor, "//"))
    {
        while (!(*cursor->position == '\n' || *cursor->position == '\0'))
            ++cursor->position;

        while (isspace(*cursor->position))
            ++cursor->position;
    }
}

bool ParseNumber(Cursor* cursor, uint64_t* number)
{
    Cursor cursorAfter = *cursor;

    if (!isdigit(*cursorAfter.position))
        return false;

    *number = 0;

    while (isdigit(*cursorAfter.position))
    {
        *number *= 10;
        *number += (*cursorAfter.position) - (uint64_t)'0';
        ++cursorAfter.position;
    }

    *cursor = cursorAfter;

    return true;
}

bool ParseByte(Cursor* cursor, uint8_t& value)
{
    uint8_t left = 0;
    uint8_t right = 0;

    if (!CharacterToByteValue(*cursor->position, left))
        return false;

    if (!CharacterToByteValue(*(cursor->position + 1), right))
        return false;

    cursor->position += 2;

    value = (left << 4) | right;

    return true;
}

bool ParseBytes(Cursor* cursor, std::vector<uint8_t>& bytes)
{
    Cursor cursorAfter = *cursor;

    IgnoreZeroX(&cursorAfter);

    std::vector<uint8_t> newbytes;

    uint8_t value;
    if (!CharacterToByteValue(*cursorAfter.position, value))
        return false;

    while (CharacterToByteValue(*cursorAfter.position, value))
    {
        uint8_t byte;
        if (!ParseByte(&cursorAfter, byte))
            return false;

        newbytes.push_back(byte);
    }

    *cursor = cursorAfter;

    std::reverse(newbytes.begin(), newbytes.end());
    for (uint8_t byte : newbytes)
        bytes.push_back(byte);

    return true;
}

bool ParseName(Cursor* cursor, std::string& name)
{
    Cursor cursorAfter = *cursor;

    if (!isalpha(*cursorAfter.position))
        return false;

    while (isalnum(*cursorAfter.position))
        ++cursorAfter.position;

    name = std::string(cursor->position, cursorAfter.position - cursor->position);

    *cursor = cursorAfter;

    return true;
}

bool ParseVariableSize(Cursor* cursor, uint64_t* size)
{
    Cursor cursorAfter = *cursor;

    bool foundType = false;

    if (!ParseNumber(&cursorAfter, size))
        return false;
    
    *cursor = cursorAfter;

    return true;
}

bool ParseBinding(Cursor* cursor, ByteSequence* bytes)
{
    Cursor cursorAfter = *cursor;

    ByteSequenceVariable variable;
    variable.bound = false;

    if (!Consume(&cursorAfter, "["))
        return false;

    IgnoreWhiteSpace(&cursorAfter);

    if (!ParseName(&cursorAfter, variable.name))
        return false;

    IgnoreWhiteSpace(&cursorAfter);

    if (!Consume(&cursorAfter, ":"))
        return false;

    IgnoreWhiteSpace(&cursorAfter);

    {
        uint64_t size;
        if (!ParseVariableSize(&cursorAfter, &size))
            return false;

        variable.size = size;

        IgnoreWhiteSpace(&cursorAfter);
    }

    if (!Consume(&cursorAfter, "]"))
        return false;

    variable.offset = bytes->bytes.size();

    for (int i = 0; i < variable.size; ++i)
        bytes->bytes.push_back(0);

    bytes->bindings.push_back(variable);

    *cursor = cursorAfter;

    return true;
}

bool ParseVolatilePushPop(Cursor* cursor, ByteSequence* bytes)
{
    Cursor cursorAfter = *cursor;

    if (!Consume(&cursorAfter, "["))
        return false;

    IgnoreWhiteSpace(&cursorAfter);

    bool push = Consume(&cursorAfter, "push");

    if (!push && !Consume(&cursorAfter, "pop"))
        return false;

    IgnoreWhiteSpace(&cursorAfter);

    if (!Consume(&cursorAfter, "volatile"))
        return false;

    IgnoreWhiteSpace(&cursorAfter);

    if (!Consume(&cursorAfter, "]"))
        return false;

    std::vector<uint8_t> addedSequence;

    if (push)
        addedSequence = { 0x9c, 0x50, 0x53, 0x51, 0x52, 0x41, 0x50, 0x41, 0x51, 0x41, 0x52, 0x41, 0x53 };
    else
        addedSequence = { 0x41, 0x5B, 0x41, 0x5A, 0x41, 0x59, 0x41, 0x58, 0x5A, 0x59, 0x5B, 0x58, 0x9d };

    bytes->bytes.insert(bytes->bytes.end(), addedSequence.begin(), addedSequence.end());

    *cursor = cursorAfter;

    return true;
}

bool ParseSpecialLabel(Cursor* cursor, ByteSequence* bytes)
{
    Cursor cursorAfter = *cursor;

    bool found = false;

    if (Consume(&cursorAfter, "skip+"))
    {
        bytes->skipAbove.label.name = "skip+";
        bytes->skipAbove.label.offset = bytes->bytes.size();
        bytes->skipAbove.present = true;
        found = true;
    }
    else if (Consume(&cursorAfter, "returnLocation"))
    {
        bytes->returnAddress.label.name = "returnLocation";
        bytes->returnAddress.label.offset = bytes->bytes.size();
        bytes->returnAddress.present = true;
        found = true;
    }

    if (!found)
        return false;

    *cursor = cursorAfter;
    return true;
}

bool ParseNextSubsequence(Cursor* cursor, ByteSequence* sequence)
{
    if (ParseSpecialLabel(cursor, sequence))
        return true;

    if (ParseBytes(cursor, sequence->bytes))
        return true;

    if (ParseBinding(cursor, sequence))
        return true;

    if (ParseVolatilePushPop(cursor, sequence))
        return true;

    return false;
}

bool ReadByteSequence(Cursor* cursor, ByteSequence* sequence)
{
    sequence->skipAbove.present = false;
    sequence->returnAddress.present = false;

    Cursor cursorAfter = *cursor;

    IgnoreWhiteSpace(&cursorAfter);

    if (!ParseNextSubsequence(&cursorAfter, sequence))
        return false;

    uint8_t byte;
    while (true)
    {
        *cursor = cursorAfter;

        IgnoreWhiteSpace(&cursorAfter);
        Consume(&cursorAfter, ",");
        IgnoreWhiteSpace(&cursorAfter);

        if (!ParseNextSubsequence(&cursorAfter, sequence))
            break;

        *cursor = cursorAfter;
    }

    return true;
}

bool Bind(ByteSequence* sequence, std::string name, uint8_t* valueBuffer)
{
    std::vector<ByteSequenceVariable>::iterator it = sequence->bindings.begin();

    bool foundMatch = false;

    while (it != sequence->bindings.end())
    {
        if (name.compare(it->name) == 0)
        {
            memcpy((uint8_t*)(sequence->bytes.data() + it->offset), valueBuffer, it->size);
            it->bound = true;
            foundMatch = true;
        }

        ++it;
    }

    return foundMatch;
}

bool Read(ByteSequence* sequence, std::string name, uint8_t* valueBuffer)
{
    std::vector<ByteSequenceVariable>::iterator it = sequence->bindings.begin();

    while (it != sequence->bindings.end())
    {
        if (name.compare(it->name) == 0)
        {
            memcpy(valueBuffer, (uint8_t*)(sequence->bytes.data() + it->offset), it->size);
            return true;
        }

        ++it;
    }

    return false;
}



bool FindAndFill(ByteSequence* sequence, uint8_t* memory, size_t bytes, size_t* offset)
{
    *offset = 0;

    std::vector<uint8_t> mask(sequence->bytes.size(), 0xFF);

    std::vector<ByteSequenceVariable>::iterator it = sequence->bindings.begin();

    while (it != sequence->bindings.end())
    {
        if (it->bound)
            continue;

        for (int i = 0; i < it->size; ++i)
        {
            mask[it->offset + i] = 0x00;
        }
        
        ++it;
    }

    while (*offset <= bytes - sequence->bytes.size())
    {
        bool success = true;

        for (int i = 0; i < sequence->bytes.size(); ++i)
        {
            uint8_t lhs = (sequence->bytes[i] & mask[i]);
            uint8_t rhs = (memory[*offset + i] & mask[i]);

            if (lhs != rhs)
            {
                success = false;
                break;
            }
        }

        if (success)
        {
            it = sequence->bindings.begin();
            while (it != sequence->bindings.end())
            {
                it->bound = true;
                ++it;
            }

            for (int i = 0; i < sequence->bytes.size(); ++i)
            {
                sequence->bytes[i] = memory[i + *offset];
            }

            return true;
        }

        *offset += 1;
    }

    return false;
}

std::string ByteSequencesToString(const ByteSequence& sequence)
{
    std::vector<std::string> bytesToString;

    for (int i = 0; i < sequence.bytes.size(); ++i)
    {
        char buffer[32];

        sprintf_s(buffer, "%02X", sequence.bytes[i]);

        bytesToString.push_back(buffer);
    }

    for (auto it = sequence.bindings.begin(); it != sequence.bindings.end(); ++it)
    {
        if (it->bound)
            continue;

        for (int i = 0; i < it->size; ++i)
        {
            bytesToString[i + it->offset] = "??";
        }
    }

    std::string result;

    for (int i = 0; i < bytesToString.size(); ++i)
        result += bytesToString[i];

    return result;
}

bool ParseHookAndDetour(HookSequences* sequences, const char* source, std::string& error)
{
    Cursor cursor {source};

    IgnoreWhiteSpace(&cursor);

    if (!Consume(&cursor, "-pattern:"))
        FAIL("No pattern");

    if (!ReadByteSequence(&cursor, &sequences->pattern))
        FAIL("Could not read bytes in pattern sequence");

    IgnoreWhiteSpace(&cursor);

    if (!Consume(&cursor, "-replacement:"))
        FAIL("No replacement");

    if (!ReadByteSequence(&cursor, &sequences->replacement))
        FAIL("Could not read bytes in replacement sequence");

    IgnoreWhiteSpace(&cursor);

    if (!Consume(&cursor, "-detour:"))
        FAIL("No detour");

    if (!ReadByteSequence(&cursor, &sequences->detour))
        FAIL("Could not read detour sequence");

    size_t lengthOfPattern = sequences->pattern.bytes.size();
    size_t lengthOfReplacement = sequences->replacement.bytes.size();

    // Must replace with less bytes that
    if (lengthOfPattern < lengthOfReplacement)
        FAIL("Replacement is longer than pattern");

    size_t nopPadding = lengthOfPattern - lengthOfReplacement;

    if (sequences->pattern.skipAbove.present)
        nopPadding -= sequences->pattern.skipAbove.label.offset;

    sequences->replacement.bytes.insert(sequences->replacement.bytes.end(), nopPadding, 0x90);
    sequences->nopPadding = nopPadding;

    return true;
}

bool DoHook(HookSequences* sequences, uint8_t* memory, size_t nBytes, uint64_t targetAddress, CodePutback* putback, MemoryAllocation* detourMemory)
{
    size_t offset;
    if (!FindAndFill(&sequences->pattern, memory, nBytes, &offset))
        return false;

    // Remove anything from above the 'skip+'
    if (sequences->pattern.skipAbove.present)
    {
        const auto eraseBegin = sequences->pattern.bytes.begin();
        const auto eraseEnd = eraseBegin + sequences->pattern.skipAbove.label.offset;
        sequences->pattern.bytes.erase(eraseBegin, eraseEnd);

        for (auto it = sequences->pattern.bindings.begin(); it < sequences->pattern.bindings.end(); ++it)
        {
            if (!it->bound)
                continue;

            it->offset -= sequences->pattern.skipAbove.label.offset;
        }

        offset += sequences->pattern.skipAbove.label.offset;
    }

    // Assume that all variables in the pattern are bound to something when fitted in memory
    for (auto it = sequences->pattern.bindings.begin(); it < sequences->pattern.bindings.end(); ++it)
    {
        if (!it->bound)
            return false;
    }

    // Assume detour is less than one page
    void* detourAddress = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    detourMemory->address = (uint64_t)detourAddress;

    putback->address = (uint64_t)(memory + offset);
    putback->code = sequences->pattern.bytes;

    uint64_t returnAddress = (uint64_t)memory + offset + sequences->replacement.bytes.size();

    if (sequences->replacement.returnAddress.present)
        returnAddress = (uint64_t)memory + offset + sequences->replacement.returnAddress.label.offset;

    Bind(&sequences->replacement, "detourAddress", (uint8_t*)&detourAddress);
    Bind(&sequences->detour, "targetAddress", (uint8_t*)&targetAddress);
    Bind(&sequences->detour, "returnAddress", (uint8_t*)&returnAddress);

    for (auto it = sequences->pattern.bindings.begin(); it < sequences->pattern.bindings.end(); ++it)
    {
        std::vector<uint8_t> buffer;
        buffer.resize(it->size);
        Read(&sequences->pattern, it->name, buffer.data());

        Bind(&sequences->replacement, it->name, buffer.data());
        Bind(&sequences->detour, it->name, buffer.data());
    }

    memcpy(detourAddress, sequences->detour.bytes.data(), sequences->detour.bytes.size());
    memcpy((uint8_t*)(memory + offset), sequences->replacement.bytes.data(), sequences->replacement.bytes.size());

    return true;
}
