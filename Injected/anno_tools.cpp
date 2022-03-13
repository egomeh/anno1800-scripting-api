#include "anno_tools.h"
#include "tools.h"

bool ReadAnnoString(uint64_t address, std::string& result)
{
    if (!address)
    {
        result = "[Could not retreive]";
        return false; 
    }

    if (!IsReadable((void*)(address + 0x18), 8))
    {
        result = "[Could not read longString]";
        return false;
    }

    if (!IsReadable((void*)(address + 0x10), 8))
    {
        result = "[Could not read length]";
        return false;
    }

    if (!IsReadable((void*)(address), 16))
    {
        result = "[Could not read string address]";
        return false;
    }

    uint64_t longString = *(uint64_t*)(address + 0x18);
    uint64_t length = *(uint64_t*)(address + 0x10);

    if (length > 64)
    {
        result = "[The name cannot be that long...]";
        return false;
    }

    bool isLong = longString > 0x7;

    wchar_t buffer[256];

    wchar_t* stringLocation = (wchar_t*)(address);

    if (isLong)
        stringLocation = (wchar_t*)*(uint64_t*)(address);

    if (!IsReadable((void*)(stringLocation), length))
    {
        result = "[Could not read pointer to string]";
        return false;
    }

    wmemset(buffer, 0, _countof(buffer));
    wmemcpy(buffer, stringLocation, length);

    std::wstring wideString = buffer;
    std::string name = utf8_encode(wideString);

    result = name;
    return true;
}
