#include "pch.h"
#include "Tools.h"
#include <string>
#include <vector>
#include "structs.h"
#include "autocomms.h"

std::string utf8_encode(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring utf8_decode(const std::string& str)
{
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

bool GetProcessByName(const char* processName, HANDLE* processHandle)
{
    *processHandle = nullptr;

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            std::wstring processNameW = utf8_decode(processName);

            if (wcscmp(entry.szExeFile, processNameW.c_str()) == 0)
            {
                HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                if (handle == NULL)
                    return false;

                *processHandle = handle;

                return true;
            }
        }
    }

    return false;
}

bool GetProcessModules(const HANDLE& processHandle, HMODULE** moduleList, uint64_t& numberOfModules)
{
    if (processHandle == NULL)
        return false;

    numberOfModules = 0;

    auto modulesAllocator = ScopedAllocator<HMODULE>(1024);
    HMODULE* modules = modulesAllocator.data();

    DWORD neededBytes = 0;

    if (!EnumProcessModules(processHandle, modules, sizeof(modules), &neededBytes))
        return false;

    numberOfModules = neededBytes / sizeof(HMODULE);

    *moduleList = new HMODULE[numberOfModules];

    for (uint64_t i = 0; i < numberOfModules; ++i)
    {
        (*moduleList)[i] = modules[i];
    }

    return true;
}

bool EndsWith(const char* string, const char* check)
{
    size_t stringLength = strlen(string);
    size_t checkLength = strlen(check);

    if (stringLength < checkLength)
        return false;

    for (size_t i = 0; i < checkLength; ++i)
    {
        if (string[stringLength - i - 1] != check[checkLength - i - 1])
            return false;
    }

    return true;
}

bool FindModule(HANDLE process, HMODULE* moduleList, uint64_t numberOfModules, const char* nameEnd, HMODULE& result)
{
    for (int i = 0; i < numberOfModules; ++i)
    {
        char nameBuffer[4096];
        if (!GetModuleFileNameExA(process, moduleList[i], nameBuffer, sizeof(nameBuffer)))
            return false;

        if (EndsWith(nameBuffer, nameEnd))
        {
            result = moduleList[i];
            return true;
        }
    }

    return false;
}

bool GetProcessName(HANDLE process, wchar_t** name)
{
    if (!process)
        return false;
 
    TCHAR buffer[MAX_PATH];
    if (!GetModuleFileNameEx(process, 0, buffer, MAX_PATH))
        return false;

    size_t nameLength = wcslen(buffer);

    *name = new wchar_t[MAX_PATH];

    wcscpy_s(*name, MAX_PATH, buffer);

    return true;
}

bool IsExecutable(DWORD protection)
{
    if (protection == PAGE_EXECUTE)
        return true;

    if (protection == PAGE_EXECUTE_READ)
        return true;

    if (protection == PAGE_EXECUTE_READWRITE)
        return true;

    if (protection == PAGE_EXECUTE_WRITECOPY)
        return true;

    return false;
}

bool GetExecutableRegions(HANDLE process, uint64_t regionBegin, uint64_t regionEnd, MemoryRegion** regions, uint64_t& nRegions)
{
    if (regionEnd < regionBegin)
        return false;

    // Count number of regions
    nRegions = 0;

    uint64_t address = regionBegin;
    MEMORY_BASIC_INFORMATION memoryInfo;

    while (address < regionEnd)
    {
        size_t bytesInBuffer = VirtualQueryEx(process, (void*)address, &memoryInfo, sizeof(memoryInfo));

        if (bytesInBuffer == 0)
            return false;

        if (IsExecutable(memoryInfo.Protect))
            ++nRegions;

        address += memoryInfo.RegionSize;
    }

    address = regionBegin;
    *regions = new MemoryRegion[nRegions];
    size_t regionIndex = 0;

    while (address < regionEnd)
    {
        size_t bytesInBuffer = VirtualQueryEx(process, (void*)address, &memoryInfo, sizeof(memoryInfo));

        if (bytesInBuffer == 0)
            return false;

        if (IsExecutable(memoryInfo.Protect))
        {
            MemoryRegion& region = (*regions)[regionIndex];
            region.baseAddress = (uint64_t)memoryInfo.BaseAddress;
            region.size = (uint64_t)memoryInfo.RegionSize;
            region.protection = memoryInfo.Protect;
            ++regionIndex;
        }

        address += memoryInfo.RegionSize;
    }

    return true;
}

bool GetThreadList(DWORD processID)
{
    HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshotHandle == INVALID_HANDLE_VALUE)
        return false;

    ScopedAllocator<HANDLE> handleAllocator = ScopedAllocator<HANDLE>(1024);
    HANDLE* handles = handleAllocator.data();

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (!Thread32First(snapshotHandle, &te32))
    {
        CloseHandle(snapshotHandle);
        return false;
    }

    do
    {
        if (te32.th32OwnerProcessID == processID)
        {
            printf("%d\n", te32.th32ThreadID);
            te32.tpBasePri;
            te32.tpDeltaPri;
        }
    } while (Thread32Next(snapshotHandle, &te32));

    return true;
}

bool IsReadable(void* address, size_t byteCount)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
        return false;

    if (mbi.State != MEM_COMMIT)
        return false;

    if (mbi.Protect == PAGE_NOACCESS || mbi.Protect == PAGE_EXECUTE)
        return false;

    // This checks that the start of memory block is in the same "region" as the
    // end. If it isn't you "simplify" the problem into checking that the rest of 
    // the memory is readable.
    size_t blockOffset = (size_t)((char*)address - (char*)mbi.AllocationBase);
    size_t blockBytesPostPtr = mbi.RegionSize - blockOffset;

    if (blockBytesPostPtr < byteCount)
        return IsReadable((char*)address + blockBytesPostPtr,
            byteCount - blockBytesPostPtr);

    return true;
}

bool IsWriteable(void* address, size_t byteCount)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
        return false;

    if (mbi.State != MEM_COMMIT)
        return false;

    if (mbi.Protect == PAGE_NOACCESS || mbi.Protect == PAGE_READONLY ||
        mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READ ||
        mbi.Protect == PAGE_EXECUTE)
        return false;

    // This checks that the start of memory block is in the same "region" as the
    // end. If it isn't you "simplify" the problem into checking that the rest of 
    // the memory is readable.
    size_t blockOffset = (size_t)((char*)address - (char*)mbi.AllocationBase);
    size_t blockBytesPostPtr = mbi.RegionSize - blockOffset;

    if (blockBytesPostPtr < byteCount)
        return IsReadable((char*)address + blockBytesPostPtr,
            byteCount - blockBytesPostPtr);

    return true;
}


bool IsReadablePointer(uint64_t address)
{
    return IsReadable((void*)address, sizeof(void*));
}

uint64_t ReadU64(uint64_t address)
{
    return *(uint64_t*)address;
}

uint32_t ReadU32(uint64_t address)
{
    return *(uint32_t*)address;
}

float ReadF32(uint64_t address)
{
    return *(float*)address;
}

std::string resourceIDToString(uint32_t id)
{
    switch (id)
    {
    case 0xf6a10: return "grain";
    case 0xf6a11: return "beef";
    case 0xf6a12: return "hops";
    case 0xf6a13: return "potatoes";
    case 0x1d4c8: return "wood";
    case 0xf6a15: return "wool";
    case 0xf6a17: return "pigs";
    case 0xf6a21: return "fur";
    case 0x1d4ce: return "grapes";
    case 0xf6a16: return "paprika";
    case 0xf6a18: return "fish";
    case 0xf6a38: return "salpeter";
    case 0xf6a34: return "quartz sand";
    case 0xf6a1a: return "concrete";
    case 0xf6a1b: return "soap";
    case 0xf6a14: return "timber";
    case 0xf6a1d: return "bricks";
    case 0xf6a47: return "fur coats";
    case 0xf6a1f: return "window";
    case 0xf6a20: return "light bulbs";
    case 0xf6a22: return "sailcloth";
    case 0xf6a23: return "chassis";
    case 0xf6a19: return "clay";
    case 0xf6a1e: return "sewing machines";
    case 0xf6a25: return "bread";
    case 0xf6a26: return "beer";
    case 0xf6a27: return "goulash";
    case 0xf6a29: return "canned food";
    case 0xf6a28: return "schnapps";
    case 0xf6a3e: return "sausage";
    case 0x1d4d0: return "champagne";
    case 0xf6a2a: return "beams";
    case 0xf6a2b: return "steel";
    case 0xf6a2d: return "weapons";
    case 0xf6a2e: return "dynamite";
    case 0xf6a2f: return "advanced weapons";
    case 0xf6a30: return "steam motors";
    case 0xf6a31: return "steam carriages";
    case 0xf6a1c: return "brass";
    case 0xf6a32: return "coal";
    case 0xf6a33: return "iron";
    case 0xf6a35: return "zinc";
    case 0xf6a36: return "copper";
    case 0xf6a37: return "cement";
    case 0xf6a39: return "gold ore";
    case 0xf6a3a: return "tallow";
    case 0xf6a3b: return "flour";
    case 0xf6a3c: return "malt";
    case 0xf6a3d: return "basic clothes";
    case 0xf6a41: return "glass";
    case 0xf6a42: return "inlay";
    case 0xf6a43: return "carbon filament";
    case 0xf6a45: return "high-wheeler";
    case 0xf6a46: return "pocket watch";
    case 0x1d4de: return "glasses";
    case 0xf6a48: return "phonographs";
    case 0xf6a49: return "gold";
    case 0xf6a4a: return "jewelry";
    case 0xf6a4b: return "sugar cane";
    case 0xf6a4c: return "tobacco";
    case 0xf6a4d: return "cotton";
    case 0xf6a4e: return "cocoa";
    case 0xf6a4f: return "caoutchouc";
    case 0x1d4df: return "coffee beans";
    case 0x1d4e2: return "corn";
    case 0x1d4e4: return "alpaca wool";
    case 0x1d4e9: return "bananas";
    case 0xf6a50: return "pearls";
    case 0x1d4ea: return "fish oil";
    case 0x1d4eb: return "ponchos";
    case 0x1d4ec: return "felt";
    case 0x1d4e5: return "bombin";
    case 0xf6a51: return "rum";
    case 0xf6a52: return "chocolate";
    case 0x1d4e0: return "coffee";
    case 0x1d4e1: return "fried bananas";
    case 0x1d4e3: return "burrito";
    case 0xf6a3f: return "sugar";
    case 0xf6a40: return "cotton cloth";
    case 0xf6a53: return "cigars";
    case 0x1d4d5: return "stone coal";
    case 0xf6b86: return "oil";
    case 0x1d4d6: return "electricity";
    case 0x1b836: return "caribou meat";
    case 0x1b839: return "goose feathers";
    case 0x1b837: return "bear fur";
    case 0x1b83a: return "husky";
    case 0x1b83b: return "whale oil";
    case 0x1b838: return "seal skin";
    case 0x1b841: return "pemmican";
    case 0x1b83e: return "oil lamps";
    case 0x1b83d: return "sleeping bags";
    case 0x1b83c: return "parka";
    case 0x1b840: return "sled frames";
    case 0x1b83f: return "sleds";
    case 0x1b842: return "gas";
    case 0x1c50c: return "arctic scrap I";
    default: return "";
    }
    return "";
}

uint32_t stringToResourceID(const char* name)
{
    if (strcmp(name, "Money") == 0) { return 0xf6961; }
    else if (strcmp(name, "Influence") == 0) { return 0xf6a0e; }
    else if (strcmp(name, "grain") == 0) { return 0xf6a10; }
    else if (strcmp(name, "beef") == 0) { return 0xf6a11; }
    else if (strcmp(name, "hops") == 0) { return 0xf6a12; }
    else if (strcmp(name, "potatoes") == 0) { return 0xf6a13; }
    else if (strcmp(name, "wood") == 0) { return 0x1d4c8; }
    else if (strcmp(name, "wool") == 0) { return 0xf6a15; }
    else if (strcmp(name, "pigs") == 0) { return 0xf6a17; }
    else if (strcmp(name, "fur") == 0) { return 0xf6a21; }
    else if (strcmp(name, "grapes") == 0) { return 0x1d4ce; }
    else if (strcmp(name, "paprika") == 0) { return 0xf6a16; }
    else if (strcmp(name, "fish") == 0) { return 0xf6a18; }
    else if (strcmp(name, "salpeter") == 0) { return 0xf6a38; }
    else if (strcmp(name, "quartz sand") == 0) { return 0xf6a34; }
    else if (strcmp(name, "concrete") == 0) { return 0xf6a1a; }
    else if (strcmp(name, "soap") == 0) { return 0xf6a1b; }
    else if (strcmp(name, "timber") == 0) { return 0xf6a14; }
    else if (strcmp(name, "bricks") == 0) { return 0xf6a1d; }
    else if (strcmp(name, "fur coats") == 0) { return 0xf6a47; }
    else if (strcmp(name, "window") == 0) { return 0xf6a1f; }
    else if (strcmp(name, "light bulbs") == 0) { return 0xf6a20; }
    else if (strcmp(name, "sailcloth") == 0) { return 0xf6a22; }
    else if (strcmp(name, "chassis") == 0) { return 0xf6a23; }
    else if (strcmp(name, "clay") == 0) { return 0xf6a19; }
    else if (strcmp(name, "sewing machines") == 0) { return 0xf6a1e; }
    else if (strcmp(name, "bread") == 0) { return 0xf6a25; }
    else if (strcmp(name, "beer") == 0) { return 0xf6a26; }
    else if (strcmp(name, "goulash") == 0) { return 0xf6a27; }
    else if (strcmp(name, "canned food") == 0) { return 0xf6a29; }
    else if (strcmp(name, "schnapps") == 0) { return 0xf6a28; }
    else if (strcmp(name, "sausage") == 0) { return 0xf6a3e; }
    else if (strcmp(name, "champagne") == 0) { return 0x1d4d0; }
    else if (strcmp(name, "beams") == 0) { return 0xf6a2a; }
    else if (strcmp(name, "steel") == 0) { return 0xf6a2b; }
    else if (strcmp(name, "weapons") == 0) { return 0xf6a2d; }
    else if (strcmp(name, "dynamite") == 0) { return 0xf6a2e; }
    else if (strcmp(name, "advanced weapons") == 0) { return 0xf6a2f; }
    else if (strcmp(name, "steam motors") == 0) { return 0xf6a30; }
    else if (strcmp(name, "steam carriages") == 0) { return 0xf6a31; }
    else if (strcmp(name, "brass") == 0) { return 0xf6a1c; }
    else if (strcmp(name, "coal") == 0) { return 0xf6a32; }
    else if (strcmp(name, "iron") == 0) { return 0xf6a33; }
    else if (strcmp(name, "zinc") == 0) { return 0xf6a35; }
    else if (strcmp(name, "copper") == 0) { return 0xf6a36; }
    else if (strcmp(name, "cement") == 0) { return 0xf6a37; }
    else if (strcmp(name, "gold ore") == 0) { return 0xf6a39; }
    else if (strcmp(name, "tallow") == 0) { return 0xf6a3a; }
    else if (strcmp(name, "flour") == 0) { return 0xf6a3b; }
    else if (strcmp(name, "malt") == 0) { return 0xf6a3c; }
    else if (strcmp(name, "basic clothes") == 0) { return 0xf6a3d; }
    else if (strcmp(name, "glass") == 0) { return 0xf6a41; }
    else if (strcmp(name, "inlay") == 0) { return 0xf6a42; }
    else if (strcmp(name, "carbon filament") == 0) { return 0xf6a43; }
    else if (strcmp(name, "high-wheeler") == 0) { return 0xf6a45; }
    else if (strcmp(name, "pocket watch") == 0) { return 0xf6a46; }
    else if (strcmp(name, "glasses") == 0) { return 0x1d4de; }
    else if (strcmp(name, "phonographs") == 0) { return 0xf6a48; }
    else if (strcmp(name, "gold") == 0) { return 0xf6a49; }
    else if (strcmp(name, "jewelry") == 0) { return 0xf6a4a; }
    else if (strcmp(name, "sugar cane") == 0) { return 0xf6a4b; }
    else if (strcmp(name, "tobacco") == 0) { return 0xf6a4c; }
    else if (strcmp(name, "cotton") == 0) { return 0xf6a4d; }
    else if (strcmp(name, "cocoa") == 0) { return 0xf6a4e; }
    else if (strcmp(name, "caoutchouc") == 0) { return 0xf6a4f; }
    else if (strcmp(name, "coffee beans") == 0) { return 0x1d4df; }
    else if (strcmp(name, "corn") == 0) { return 0x1d4e2; }
    else if (strcmp(name, "alpaca wool") == 0) { return 0x1d4e4; }
    else if (strcmp(name, "bananas") == 0) { return 0x1d4e9; }
    else if (strcmp(name, "pearls") == 0) { return 0xf6a50; }
    else if (strcmp(name, "fish oil") == 0) { return 0x1d4ea; }
    else if (strcmp(name, "ponchos") == 0) { return 0x1d4eb; }
    else if (strcmp(name, "felt") == 0) { return 0x1d4ec; }
    else if (strcmp(name, "bombin") == 0) { return 0x1d4e5; }
    else if (strcmp(name, "rum") == 0) { return 0xf6a51; }
    else if (strcmp(name, "chocolate") == 0) { return 0xf6a52; }
    else if (strcmp(name, "coffee") == 0) { return 0x1d4e0; }
    else if (strcmp(name, "fried bananas") == 0) { return 0x1d4e1; }
    else if (strcmp(name, "burrito") == 0) { return 0x1d4e3; }
    else if (strcmp(name, "sugar") == 0) { return 0xf6a3f; }
    else if (strcmp(name, "cotton cloth") == 0) { return 0xf6a40; }
    else if (strcmp(name, "cigars") == 0) { return 0xf6a53; }
    else if (strcmp(name, "stone coal") == 0) { return 0x1d4d5; }
    else if (strcmp(name, "oil") == 0) { return 0xf6b86; }
    else if (strcmp(name, "electricity") == 0) { return 0x1d4d6; }
    else if (strcmp(name, "caribou meat") == 0) { return 0x1b836; }
    else if (strcmp(name, "goose feathers") == 0) { return 0x1b839; }
    else if (strcmp(name, "bear fur") == 0) { return 0x1b837; }
    else if (strcmp(name, "husky") == 0) { return 0x1b83a; }
    else if (strcmp(name, "whale oil") == 0) { return 0x1b83b; }
    else if (strcmp(name, "seal skin") == 0) { return 0x1b838; }
    else if (strcmp(name, "pemmican") == 0) { return 0x1b841; }
    else if (strcmp(name, "oil lamps") == 0) { return 0x1b83e; }
    else if (strcmp(name, "sleeping bags") == 0) { return 0x1b83d; }
    else if (strcmp(name, "parka") == 0) { return 0x1b83c; }
    else if (strcmp(name, "sled frames") == 0) { return 0x1b840; }
    else if (strcmp(name, "sleds") == 0) { return 0x1b83f; }
    else if (strcmp(name, "gas") == 0) { return 0x1b842; }

    return -1;
}

bool HackSendMessage(SOCKET socket, Message* message)
{
    int result = send(socket, (char*)message, sizeof(Message), 0);

    if (result == SOCKET_ERROR)
        return false;

    return true;
}

bool HackReceiveMessage(SOCKET socket, Message *message)
{
    int iResult = recv(socket, (char*)message, sizeof(Message), 0);

    if (iResult == SOCKET_ERROR)
        return false;

    return true;
}

void whitespace(const char*& string)
{
    while (isspace(*string))
        ++string;
}

bool match(const char*& string, const char* symbol)
{
    size_t symbol_length = strlen(symbol);

    for (int i = 0; i < symbol_length; ++i)
        if (string[i] != symbol[i])
            return false;

    string += symbol_length;
    return true;
}

bool shipname(const char*& string, std::string& name)
{
    const char* cursor = string;

    whitespace(cursor);

    if (!match(cursor, "\""))
        return false;

    const char* stringBegin = cursor;

    while (*cursor != '\"')
    {
        ++cursor;
        if (*cursor == '\0')
            return false;
    }

    const char* stringEnd = cursor - 1;

    int size = (int)(stringEnd - stringBegin + 1);
    char* buffer = new char[size + 1];
    memset(buffer, 0, size + 1);
    memcpy(buffer, stringBegin, size);

    name = buffer;

    delete[] buffer;

    ++cursor;
    string = cursor;

    return true;
}

bool number(const char*& string, float& result)
{
    const char* cursor = string;

    whitespace(cursor);

    // If we start with a '.' that is not followed by a number, this is not a number
    if (*cursor == '.' && !isdigit(*(cursor + 1)))
        return false;

    // If we don't start with either a '.' or a digit, we fail.
    if (*cursor != '.' && !isdigit(*cursor))
        return false;

    // Assume at first that we're dealing with an integer number.
    bool is_integer = *cursor != '.';
    int number = 0;

    while (isdigit(*cursor))
    {
        number *= 10;
        number += *cursor - '0';
        ++cursor;
    }

    // If there is a '.' the the result is a real number.
    float decimal_part = 0.;

    if (*cursor == '.')
    {
        ++cursor;
        is_integer = false;
    }

    float decimal_place = 0.1f;
    while (isdigit(*cursor))
    {
        decimal_part += static_cast<float>(*cursor - '0') * decimal_place;
        decimal_place *= 0.1f;
        ++cursor;
    }

    result = static_cast<float>(number);
    result += decimal_part;

    string = cursor;
    return true;
}

bool integer(const char*& string, int& result)
{
    const char* cursor = string;

    whitespace(cursor);

    // If we start with a '.' that is not followed by a number, this is not a number
    if (!isdigit(*(cursor)))
        return false;

    int number = 0;

    while (isdigit(*cursor))
    {
        number *= 10;
        number += *cursor - '0';
        ++cursor;
    }
    result = number;

    string = cursor;
    return true;
}

bool ParseMoveOrder(const char* string, std::vector<std::string>& names, std::string& islandName, float& x, float& y)
{
    names.clear();
    islandName.clear();
    x = 0.f;
    y = 0.f;
    std::string name;

    if (!shipname(string, name))
        return false;

    names.push_back(name);

    while (true)
    {
        whitespace(string);

        if (!match(string, ","))
            break;

        if (!shipname(string, name))
            return false;

        names.push_back(name);
    }

    std::string targetName;

    // Either parse a coordinate pair or a name
    if (!shipname(string, targetName))
    {
        if (!number(string, x))
            return false;

        if (!number(string, y))
            return false;
    }
    else
    {
        islandName = targetName;
    }

    return true;
}

bool IsTradingPost(uint64_t id) 
{
    switch (id)
    {
    case 101290: case 1010540: case 112659: case 112865:
    case 1010517: case 101293: case 101291: case 100514:
    case 112660: case 112866: case 100510: case 100515:
    case 112661: case 112867: case 100511: case 101292:
        return true;
    default:
        return false;
    }
}

SOCKET WaitForScriptingSocket()
{
    SOCKET listener = NULL;
    SOCKET client = NULL;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int recvbuflen = 0x1000;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    int iResult = getaddrinfo(NULL, "1800", &hints, &result);
    if (iResult != 0)
        return NULL;

    listener = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listener == INVALID_SOCKET)
        return NULL;

    iResult = bind(listener, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
        return NULL;

    freeaddrinfo(result);

    iResult = listen(listener, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
        NULL;

    client = accept(listener, NULL, NULL);

    closesocket(listener);

    return client;
}

bool ReadMessageFromSocket(SOCKET socket, std::vector<uint8_t>& buffer)
{
    buffer.clear();
    buffer.resize(0x100);
    int bytesRead = recv(socket, (char*)buffer.data(), static_cast<int>(buffer.size()), 0);

    if (bytesRead == SOCKET_ERROR)
        return false;

    buffer.resize(bytesRead);

    uint64_t inSize = 0;
    size_t offset = 0;

    if (!AutoComms::Deserialize(&inSize, buffer, &offset))
        return false;

    while (buffer.size() < inSize)
    {
        std::vector<uint8_t> tempBuffer(0x100);

        int extraBytes = recv(socket, (char*)tempBuffer.data(), static_cast<int>(tempBuffer.size()), 0);

        if (extraBytes == SOCKET_ERROR)
            return false;

        tempBuffer.resize(extraBytes);

        buffer.insert(buffer.end(), tempBuffer.begin(), tempBuffer.end());
    }

    return true;
}

uint64_t AreaToAreaID(AutoComms::Area area)
{
    switch (area)
    {
    case AutoComms::Area::OldWorld:
        return 2;
    case AutoComms::Area::NewWorld:
        return 3;
    case AutoComms::Area::Arctic:
        return 4;
    default:
        return -1;
    }
}

AutoComms::ShipType IDToShipType(uint64_t shipTypeId)
{
    switch (shipTypeId)
    {
    case 0x18855:
        return AutoComms::ShipType::Gunboat;
    case 0x18856:
        return AutoComms::ShipType::Schooner;
    case 0x18857:
        return AutoComms::ShipType::Frigate;
    case 0x18858:
        return AutoComms::ShipType::ShipOfTheLine;
    case 0x18859:
    case 0x192c4:
        return AutoComms::ShipType::Clipper;
    case 0x1885a:
        return AutoComms::ShipType::Battlecruiser;
    case 0x1885b:
        return AutoComms::ShipType::Monitor;
    case 0x18b01:
        return AutoComms::ShipType::CommandShip;
    default:
        return AutoComms::ShipType::Invalid;
    }
}
