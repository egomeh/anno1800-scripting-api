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
    int stringLength = strlen(string);
    int checkLength = strlen(check);

    if (stringLength < checkLength)
        return false;

    for (int i = 0; i < checkLength; ++i)
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

bool GetAssumedFunctions(const char* filename, const char* section, AssumedFunction** functions, uint64_t &entries)
{
    LPVOID data = NULL;
    
    HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    DWORD fileSize = GetFileSize(hFile, &fileSize);

    data = (LPVOID)(new char[fileSize]);

    DWORD read = -1;
    if (!ReadFile(hFile, data, fileSize, &read, NULL))
        return false;

    char* text = (char*)data;

    size_t numberOfLines = 0;
    for (int i = 0; i < fileSize; ++i)
    {
        if (text[i] == '\n')
            ++numberOfLines;
    }

    CloseHandle(hFile);

    return true;
}

bool WriteToPipeBlocking(HANDLE pipe, const char* message)
{
    DWORD bytesWritten = 0;
    BOOL wrote = WriteFile(pipe, message, strlen(message) + 1, &bytesWritten, NULL);

    if (!wrote)
        return false;

    return true;
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

uint64_t FindAddressByPointerList(const uint64_t* offsets, uint32_t n, const uint64_t address, bool followFirst, bool followLast)
{
    uint64_t finalAddress = address;

    if (!IsReadable((void*)finalAddress, sizeof(void*)))
        return 0;

    if (followFirst)
    {
        finalAddress = *(uint64_t*)(finalAddress);

        if (!IsReadable((void*)finalAddress, sizeof(void*)))
            return 0;
    }

    for (int i = 0; i < n - 1; ++i)
    {
        finalAddress += offsets[i];

        if (!IsReadable((void*)finalAddress, sizeof(void*)))
            return 0;

        finalAddress = *(uint64_t*)(finalAddress);

        if (!IsReadable((void*)finalAddress, sizeof(void*)))
            return 0;
    }

    finalAddress += offsets[n - 1];

    if (!IsReadable((void*)finalAddress, sizeof(void*)))
        return false;

    if (followLast)
    {
        finalAddress = *(uint64_t*)(finalAddress);

        if (!IsReadable((void*)finalAddress, sizeof(void*)))
            return 0;
    }

    return finalAddress;
}

bool PlotAsterisk(size_t width, size_t height, float* data, std::vector<std::string>& output)
{
    output.clear();

    output = std::vector<std::string>();

    for (int i = 0; i < height; ++i)
    {
        output.push_back(std::string(width, ' '));
    }

    for (size_t x = 0; x < width; ++x)
    {
        float sample = data[x];
        int sampleOnScale = max(0, (sample * (float)height) - 1);
        output[height - sampleOnScale - 1][x] = '*';
    }

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

    int size = stringEnd - stringBegin + 1;
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
    double decimal_part = 0.;

    if (*cursor == '.')
    {
        ++cursor;
        is_integer = false;
    }

    double decimal_place = .1;
    while (isdigit(*cursor))
    {
        decimal_part += static_cast<double>(*cursor - '0') * decimal_place;
        decimal_place *= .1;
        ++cursor;
    }

    result = static_cast<double>(number);
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
    int bytesRead = recv(socket, (char*)buffer.data(), buffer.size(), 0);

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

        int extraBytes = recv(socket, (char*)tempBuffer.data(), tempBuffer.size(), 0);

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

uint64_t ResourceToID(AutoComms::Resource resource)
{
    switch (resource)
    {
    case AutoComms::Resource::Grain: return  0xf6a10;
    case AutoComms::Resource::Beef: return  0xf6a11;
    case AutoComms::Resource::Hops: return 0xf6a12;
    case AutoComms::Resource::Potatoes: return 0xf6a13;
    case AutoComms::Resource::Wood: return 0x1d4c8;
    case AutoComms::Resource::Wool: return 0xf6a15;
    case AutoComms::Resource::Pigs: return 0xf6a17;
    case AutoComms::Resource::Fur: return 0xf6a21;
    case AutoComms::Resource::Grapes: return 0x1d4ce;
    case AutoComms::Resource::Paprika: return 0xf6a16;
    case AutoComms::Resource::Fish: return 0xf6a18;
    case AutoComms::Resource::Salpeter: return 0xf6a38;
    case AutoComms::Resource::QuartzSand: return 0xf6a34;
    case AutoComms::Resource::Concrete: return 0xf6a1a;
    case AutoComms::Resource::Soap: return 0xf6a1b;
    case AutoComms::Resource::Timber: return 0xf6a14;
    case AutoComms::Resource::Bricks: return 0xf6a1d;
    case AutoComms::Resource::FurCoats: return 0xf6a47;
    case AutoComms::Resource::Window: return 0xf6a1f;
    case AutoComms::Resource::LightBulbs: return 0xf6a20;
    case AutoComms::Resource::Sailcloth: return 0xf6a22;
    case AutoComms::Resource::Chassis: return 0xf6a23;
    case AutoComms::Resource::Clay: return 0xf6a19;
    case AutoComms::Resource::SewingMachines: return 0xf6a1e;
    case AutoComms::Resource::Bread: return 0xf6a25;
    case AutoComms::Resource::Beer: return 0xf6a26;
    case AutoComms::Resource::Goulash: return 0xf6a27;
    case AutoComms::Resource::CannedFood: return 0xf6a29;
    case AutoComms::Resource::Schnapps: return 0xf6a28;
    case AutoComms::Resource::Sausage: return 0xf6a3e;
    case AutoComms::Resource::Champagne: return 0x1d4d0;
    case AutoComms::Resource::Beams: return 0xf6a2a;
    case AutoComms::Resource::Steel: return 0xf6a2b;
    case AutoComms::Resource::Weapons: return 0xf6a2d;
    case AutoComms::Resource::Dynamite: return 0xf6a2e;
    case AutoComms::Resource::AdvancedWeapons: return 0xf6a2f;
    case AutoComms::Resource::SteamMotors: return 0xf6a30;
    case AutoComms::Resource::SteamCarriages: return 0xf6a31;
    case AutoComms::Resource::Brass: return 0xf6a1c;
    case AutoComms::Resource::Coal: return 0xf6a32;
    case AutoComms::Resource::Iron: return 0xf6a33;
    case AutoComms::Resource::Zinc: return 0xf6a35;
    case AutoComms::Resource::Copper: return 0xf6a36;
    case AutoComms::Resource::Cement: return 0xf6a37;
    case AutoComms::Resource::GoldOre: return 0xf6a39;
    case AutoComms::Resource::Tallow: return 0xf6a3a;
    case AutoComms::Resource::Flour: return 0xf6a3b;
    case AutoComms::Resource::Malt: return 0xf6a3c;
    case AutoComms::Resource::BasicClothes: return 0xf6a3d;
    case AutoComms::Resource::Glass: return 0xf6a41;
    case AutoComms::Resource::Inlay: return 0xf6a42;
    case AutoComms::Resource::CarbonFilament: return 0xf6a43;
    case AutoComms::Resource::HighWheeler: return 0xf6a45;
    case AutoComms::Resource::PocketWatch: return 0xf6a46;
    case AutoComms::Resource::Glasses: return 0x1d4de;
    case AutoComms::Resource::Phonographs: return 0xf6a48;
    case AutoComms::Resource::Gold: return 0xf6a49;
    case AutoComms::Resource::Jewelry: return 0xf6a4a;
    case AutoComms::Resource::SugarCane: return 0xf6a4b;
    case AutoComms::Resource::Tobacco: return 0xf6a4c;
    case AutoComms::Resource::Cotton: return 0xf6a4d;
    case AutoComms::Resource::Cocoa: return 0xf6a4e;
    case AutoComms::Resource::Caoutchouc: return 0xf6a4f;
    case AutoComms::Resource::CoffeeBeans: return 0x1d4df;
    case AutoComms::Resource::Corn: return 0x1d4e2;
    case AutoComms::Resource::AlpacaWool: return 0x1d4e4;
    case AutoComms::Resource::Bananas: return 0x1d4e9;
    case AutoComms::Resource::Pearls: return 0xf6a50;
    case AutoComms::Resource::FishOil: return 0x1d4ea;
    case AutoComms::Resource::Ponchos: return 0x1d4eb;
    case AutoComms::Resource::Felt: return 0x1d4ec;
    case AutoComms::Resource::Bombin: return 0x1d4e5;
    case AutoComms::Resource::Rum: return 0xf6a51;
    case AutoComms::Resource::Chocolate: return 0xf6a52;
    case AutoComms::Resource::Coffee: return 0x1d4e0;
    case AutoComms::Resource::FriedBananas: return 0x1d4e1;
    case AutoComms::Resource::Burrito: return 0x1d4e3;
    case AutoComms::Resource::Sugar: return 0xf6a3f;
    case AutoComms::Resource::CottonCloth: return 0xf6a40;
    case AutoComms::Resource::Cigars: return 0xf6a53;
    case AutoComms::Resource::StoneCoal: return 0x1d4d5;
    case AutoComms::Resource::Oil: return 0xf6b86;
    case AutoComms::Resource::Electricity: return 0x1d4d6;
    case AutoComms::Resource::CaribouMeat: return 0x1b836;
    case AutoComms::Resource::GooseFeathers: return 0x1b839;
    case AutoComms::Resource::BearFur: return 0x1b837;
    case AutoComms::Resource::Husky: return 0x1b83a;
    case AutoComms::Resource::WhaleOil: return 0x1b83b;
    case AutoComms::Resource::SealSkin: return 0x1b838;
    case AutoComms::Resource::Pemmican: return 0x1b841;
    case AutoComms::Resource::OilLamps: return 0x1b83e;
    case AutoComms::Resource::SleepingBags: return 0x1b83d;
    case AutoComms::Resource::Parka: return 0x1b83c;
    case AutoComms::Resource::SledFrames: return 0x1b840;
    case AutoComms::Resource::Sleds: return 0x1b83f;
    case AutoComms::Resource::Gas: return 0x1b842;
    default: return -1;
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

AutoComms::Resource IDToResourceType(uint64_t resourceId)
{
    switch (resourceId)
    {
    case 0xf6a10: return AutoComms::Resource::Grain;
    case 0xf6a11: return AutoComms::Resource::Beef;
    case 0xf6a12: return AutoComms::Resource::Hops;
    case 0xf6a13: return AutoComms::Resource::Potatoes;
    case 0x1d4c8: return AutoComms::Resource::Wood;
    case 0xf6a15: return AutoComms::Resource::Wool;
    case 0xf6a17: return AutoComms::Resource::Pigs;
    case 0xf6a21: return AutoComms::Resource::Fur;
    case 0x1d4ce: return AutoComms::Resource::Grapes;
    case 0xf6a16: return AutoComms::Resource::Paprika;
    case 0xf6a18: return AutoComms::Resource::Fish;
    case 0xf6a38: return AutoComms::Resource::Salpeter;
    case 0xf6a34: return AutoComms::Resource::QuartzSand;
    case 0xf6a1a: return AutoComms::Resource::Concrete;
    case 0xf6a1b: return AutoComms::Resource::Soap;
    case 0xf6a14: return AutoComms::Resource::Timber;
    case 0xf6a1d: return AutoComms::Resource::Bricks;
    case 0xf6a47: return AutoComms::Resource::FurCoats;
    case 0xf6a1f: return AutoComms::Resource::Window;
    case 0xf6a20: return AutoComms::Resource::LightBulbs;
    case 0xf6a22: return AutoComms::Resource::Sailcloth;
    case 0xf6a23: return AutoComms::Resource::Chassis;
    case 0xf6a19: return AutoComms::Resource::Clay;
    case 0xf6a1e: return AutoComms::Resource::SewingMachines;
    case 0xf6a25: return AutoComms::Resource::Bread;
    case 0xf6a26: return AutoComms::Resource::Beer;
    case 0xf6a27: return AutoComms::Resource::Goulash;
    case 0xf6a29: return AutoComms::Resource::CannedFood;
    case 0xf6a28: return AutoComms::Resource::Schnapps;
    case 0xf6a3e: return AutoComms::Resource::Sausage;
    case 0x1d4d0: return AutoComms::Resource::Champagne;
    case 0xf6a2a: return AutoComms::Resource::Beams;
    case 0xf6a2b: return AutoComms::Resource::Steel;
    case 0xf6a2d: return AutoComms::Resource::Weapons;
    case 0xf6a2e: return AutoComms::Resource::Dynamite;
    case 0xf6a2f: return AutoComms::Resource::AdvancedWeapons;
    case 0xf6a30: return AutoComms::Resource::SteamMotors;
    case 0xf6a31: return AutoComms::Resource::SteamCarriages;
    case 0xf6a1c: return AutoComms::Resource::Brass;
    case 0xf6a32: return AutoComms::Resource::Coal;
    case 0xf6a33: return AutoComms::Resource::Iron;
    case 0xf6a35: return AutoComms::Resource::Zinc;
    case 0xf6a36: return AutoComms::Resource::Copper;
    case 0xf6a37: return AutoComms::Resource::Cement;
    case 0xf6a39: return AutoComms::Resource::GoldOre;
    case 0xf6a3a: return AutoComms::Resource::Tallow;
    case 0xf6a3b: return AutoComms::Resource::Flour;
    case 0xf6a3c: return AutoComms::Resource::Malt;
    case 0xf6a3d: return AutoComms::Resource::BasicClothes;
    case 0xf6a41: return AutoComms::Resource::Glass;
    case 0xf6a42: return AutoComms::Resource::Inlay;
    case 0xf6a43: return AutoComms::Resource::CarbonFilament;
    case 0xf6a45: return AutoComms::Resource::HighWheeler;
    case 0xf6a46: return AutoComms::Resource::PocketWatch;
    case 0x1d4de: return AutoComms::Resource::Glasses;
    case 0xf6a48: return AutoComms::Resource::Phonographs;
    case 0xf6a49: return AutoComms::Resource::Gold;
    case 0xf6a4a: return AutoComms::Resource::Jewelry;
    case 0xf6a4b: return AutoComms::Resource::SugarCane;
    case 0xf6a4c: return AutoComms::Resource::Tobacco;
    case 0xf6a4d: return AutoComms::Resource::Cotton;
    case 0xf6a4e: return AutoComms::Resource::Cocoa;
    case 0xf6a4f: return AutoComms::Resource::Caoutchouc;
    case 0x1d4df: return AutoComms::Resource::CoffeeBeans;
    case 0x1d4e2: return AutoComms::Resource::Corn;
    case 0x1d4e4: return AutoComms::Resource::AlpacaWool;
    case 0x1d4e9: return AutoComms::Resource::Bananas;
    case 0xf6a50: return AutoComms::Resource::Pearls;
    case 0x1d4ea: return AutoComms::Resource::FishOil;
    case 0x1d4eb: return AutoComms::Resource::Ponchos;
    case 0x1d4ec: return AutoComms::Resource::Felt;
    case 0x1d4e5: return AutoComms::Resource::Bombin;
    case 0xf6a51: return AutoComms::Resource::Rum;
    case 0xf6a52: return AutoComms::Resource::Chocolate;
    case 0x1d4e0: return AutoComms::Resource::Coffee;
    case 0x1d4e1: return AutoComms::Resource::FriedBananas;
    case 0x1d4e3: return AutoComms::Resource::Burrito;
    case 0xf6a3f: return AutoComms::Resource::Sugar;
    case 0xf6a40: return AutoComms::Resource::CottonCloth;
    case 0xf6a53: return AutoComms::Resource::Cigars;
    case 0x1d4d5: return AutoComms::Resource::StoneCoal;
    case 0xf6b86: return AutoComms::Resource::Oil;
    case 0x1d4d6: return AutoComms::Resource::Electricity;
    case 0x1b836: return AutoComms::Resource::CaribouMeat;
    case 0x1b839: return AutoComms::Resource::GooseFeathers;
    case 0x1b837: return AutoComms::Resource::BearFur;
    case 0x1b83a: return AutoComms::Resource::Husky;
    case 0x1b83b: return AutoComms::Resource::WhaleOil;
    case 0x1b838: return AutoComms::Resource::SealSkin;
    case 0x1b841: return AutoComms::Resource::Pemmican;
    case 0x1b83e: return AutoComms::Resource::OilLamps;
    case 0x1b83d: return AutoComms::Resource::SleepingBags;
    case 0x1b83c: return AutoComms::Resource::Parka;
    case 0x1b840: return AutoComms::Resource::SledFrames;
    case 0x1b83f: return AutoComms::Resource::Sleds;
    case 0x1b842: return AutoComms::Resource::Gas;
    default: return AutoComms::Resource::Invalid;
    }
}

AutoComms::Building IDToBuildingType(uint64_t buildingID)
{
    switch (buildingID)
    {
    case 0xF6B55:
    case 0x1B8E1:
    case 0x1B813:
    case 0xF6B6C:
    case 0x18BAA: return AutoComms::Building::SmallTradingPost;

    case 0x1889E:
    case 0x1B8E2:
    case 0x1B814:
    case 0x188A2:
    case 0x18BAB: return AutoComms::Building::MediumTradingPost;

    case 0x18BAC:
    case 0x1889F:
    case 0x1B8E3:
    case 0x1B815:
    case 0x188A3: return AutoComms::Building::LargeTradingPost;

    case 0xF6AA7: return AutoComms::Building::FarmerResidence;
    case 0xF6AA8: return AutoComms::Building::WorkerResidence;
    case 0xF6AA9: return AutoComms::Building::ArtisanResidence;

    case 0xF6AC4:
    case 0x18B89: return AutoComms::Building::Marketplace;

    case 0xF6A5A:
    case 0x18B8C:
    case 0x1C00F: return AutoComms::Building::LumberjacksHut;

    case 0xF6A74: return AutoComms::Building::Brewery;
    case 0xF6A75: return AutoComms::Building::ArtisanalKitchen;
    case 0xF6A77: return AutoComms::Building::Cannery;

    case 0xF6A58: return AutoComms::Building::HopFarm;

    case 0xF6A56: return AutoComms::Building::GrainFarm;
    case 0xF6A5E: return AutoComms::Building::GrainField;
    case 0xF6A89: return AutoComms::Building::FlourMill;
    case 0xF6A73: return AutoComms::Building::Bakery;

    case 0x18840:
    case 0x18B93: return AutoComms::Building::ClayPit;

    case 0xF6A81: return AutoComms::Building::IronMine;

    default: return AutoComms::Building::Invalid;
    }
}
