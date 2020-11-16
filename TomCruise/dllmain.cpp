// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <windows.h>
#include <shlobj.h>

#include "Q/Tools.h"
#include "Q/structs.h"
#include "Q/BytePattern.h"
#include "Q/Hooking.h"

#include "hook_scripts.h"
#include "native_call_setup.h"
#include "native_callbacks.h"
#include "native_calls.h"
#include "native_traversal.h"
#include "autocomms_native.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include <inttypes.h>

#include <string>
#include <psapi.h>
#include <vector>
#include <mutex>
#include <sstream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <set>

#include "globals.h"

#define DLL_ERROR(message) {MessageBoxA(NULL, message, "Error in DLL", MB_OK); goto cleanup;}

HMODULE self;

SOCKET outputSocket;

std::vector<std::string> printBuffer;

bool WinsockInitialized()
{
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET && WSAGetLastError() == WSANOTINITIALISED) {
        return false;
    }

    closesocket(s);
    return true;
}

bool InitWSA()
{
    WSADATA wsaData;
    DWORD iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (!iResult)
        return false;

    return true;
}

uint64_t GetGameTimeMs()
{
    return g_GameTime;
}

uint64_t TimeDiffInMinutes(uint64_t begin, uint64_t end)
{
    uint64_t diff = end - begin;
    return diff / (60u * 1'000u);
}

void FlushPrintBuffer()
{
    std::vector<std::string> copy = printBuffer;
    printBuffer.clear();

    for (const std::string& line : copy)
    {
        SendMessageToHost(line.c_str());
    }
}

HANDLE gEndEvent;

bool PrepareAndLocateHook(Hook* hook, const char* name, uint64_t targetFunction, uint64_t moduleBase, uint64_t presumedAddress, const char* hookScript, MemoryRegion* regions, size_t nRegions)
{
    hook->name = name;

    std::string error;

    if (!PrepareHook(hook, targetFunction, hookScript, error))
    {
        SEND_FORMATTED("Could not parse hook script '%s': %s", name, error.c_str());
        return false;
    }

    // Remove anything from above the 'skip+'
    if (hook->sequences.pattern.skipAbove.present)
    {
        const auto eraseBegin = hook->sequences.pattern.bytes.begin();
        const auto eraseEnd = eraseBegin + hook->sequences.pattern.skipAbove.label.offset;
        hook->sequences.pattern.bytes.erase(eraseBegin, eraseEnd);

        for (auto it = hook->sequences.pattern.bindings.begin(); it < hook->sequences.pattern.bindings.end(); ++it)
        {
            if (!it->bound)
                continue;

            it->offset -= hook->sequences.pattern.skipAbove.label.offset;
        }
    }

    if (!LocateHook(hook, regions, nRegions, presumedAddress))
    {
        SEND_FORMATTED("Could not locate hook '%s'", name);
        return false;
    }

    hook->originalCode = hook->sequences.pattern.bytes;

    uint64_t offsetFromModule = hook->targetAddress - moduleBase;
    SEND_FORMATTED("%llx (Module + %llx) - %s", hook->targetAddress, offsetFromModule, name);

    return true;
}

bool PrepareAndSetupNativeCall(NativeCallSetup* nativeCallSetup, const char* name, const char* codePattern, const char* scirpt, uint64_t* hostCall, uint64_t moduleBase, uint64_t expectedAddress, MemoryRegion* regions, size_t nRegions)
{
    nativeCallSetup->name = name;

    if (codePattern)
    {
        Cursor codePatternCursor{ codePattern };

        if (!ReadByteSequence(&codePatternCursor, &nativeCallSetup->nativeCodePattern))
        {
            SEND_FORMATTED("Could not parse native code pattern '%s'", name);
            return false;
        }
    }

    Cursor preCallCode{ scirpt };

    if (!ReadByteSequence(&preCallCode, &nativeCallSetup->setupCode))
    {
        SEND_FORMATTED("Could not read setup code sequence '%s'", name);
        return false;
    }

    LocationResult result;
    if (codePattern)
    {
        if (!LocatePatternInRegions(&nativeCallSetup->nativeCodePattern, regions, nRegions, expectedAddress, &result))
        {
            SEND_FORMATTED("Could not locate native code '%s'", name);
            return false;
        }
    }

    nativeCallSetup->allocation = (uint64_t)VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    if (codePattern)
        Bind(&nativeCallSetup->setupCode, "nativeCode", (uint8_t*)&result.foundAddress);

    memcpy((void*)nativeCallSetup->allocation, nativeCallSetup->setupCode.bytes.data(), nativeCallSetup->setupCode.bytes.size());

    *hostCall = nativeCallSetup->allocation;

    nativeCallSetup->prepared = true;

    if (codePattern)
    {
        uint64_t offsetFromModule = result.foundAddress - moduleBase;
        SEND_FORMATTED("%llx -> %llx (module + %llx) - %s", nativeCallSetup->allocation, result.foundAddress, offsetFromModule, name);
    }
    else
    {
        SEND_FORMATTED("%llx %s", nativeCallSetup->allocation, name);
    }

    return true;
}

bool DeleteNativeSetup(NativeCallSetup* nativeSetup)
{
    if (!nativeSetup->prepared)
        return false;

    if (!VirtualFree((void*)nativeSetup->allocation, 0, MEM_RELEASE))
        SEND_FORMATTED("Failed to free memory at location 0x%" PRIx64, nativeSetup->allocation);

    return true;
}

static bool ComputeFunctionOffests(HANDLE anno, std::vector<Hook>& hooks, std::vector<NativeCallSetup>& nativeCalls)
{
    static const char* processName = "Anno1800.exe";

    // Get the modules that are loaded in the exe.
    HMODULE* modules = nullptr;
    uint64_t numberOfModules = 0;
    if (!GetProcessModules(anno, &modules, numberOfModules))
        FATAL_ERROR("Could not get module list");

    HMODULE annoModule;
    if (!FindModule(anno, modules, numberOfModules, processName, annoModule))
        FATAL_ERROR("Could not find module by name");

    MODULEINFO moduleInfo;
    if (!GetModuleInformation(anno, annoModule, &moduleInfo, sizeof(moduleInfo)))
        FATAL_ERROR("Could not get module information");

    uint64_t moduleBase = (uint64_t)(moduleInfo.lpBaseOfDll);
    uint64_t entryPoint = (uint64_t)(moduleInfo.EntryPoint);
    uint64_t moduleSize = (uint64_t)(moduleInfo.SizeOfImage);
    uint64_t exeSectionSize = moduleSize - (entryPoint - moduleBase);

    SendMessageToHost("Going to find all memory regions");

    MemoryRegion* regions;
    size_t nRegions;
    if (!GetExecutableRegions(anno, moduleBase, moduleBase + moduleSize, &regions, nRegions))
        SendMessageToHost("Could not read all the regions");

    SEND_FORMATTED("Found %llu executable regions", nRegions);

    SendMessageToHost("Done finding all memory regions");

    bool isUplayBinary = false;

    Hook timeHook;
    PrepareAndLocateHook(&timeHook, "time hook", (uint64_t)NativeCallbackUpdateTime, moduleBase, moduleBase + 0x122d0f1, HookScripts::TimeAndFrame, regions, nRegions);
    hooks.push_back(timeHook);

    //Hook shipContextSetup;
    //PrepareAndLocateHook(&shipContextSetup, "ship context hook", (uint64_t)ShipContextSet, moduleBase, moduleBase + 0x1223691, HookScripts::ShipContextSetHook, regions, nRegions);
    //hooks.push_back(shipContextSetup);

    //Hook shipListIteration;
    //PrepareAndLocateHook(&shipListIteration, "Ship list iteration", (uint64_t)ShipListIteration, moduleBase, moduleBase + 0x8084E0, HookScripts::ShipListIteration, regions, nRegions);
    //hooks.push_back(shipListIteration);

    //Hook regionIteration;
    //PrepareAndLocateHook(&regionIteration, "Region iteration", (uint64_t)RegionIteration, moduleBase, moduleBase + 0x69A64C, HookScripts::RegionIteration, regions, nRegions);
    //hooks.push_back(regionIteration);

    //uint64_t islandConsumptionOffset = isUplayBinary ? 0x6685BD : 0x46060D;

    //Hook islandConsumptionIteration;
    //PrepareAndLocateHook(&islandConsumptionIteration, "Island consumption", (uint64_t)IslandConsumptionIteration, moduleBase, moduleBase + islandConsumptionOffset, HookScripts::IslandConsumptionIteration, regions, nRegions);
    //hooks.push_back(islandConsumptionIteration);

    //NativeCallSetup virtualShipGetComponent;
    //PrepareAndSetupNativeCall(&virtualShipGetComponent, "virtual ship: GetComponent", nullptr, NativeCallSetups::VirtualShipGetComponent, (uint64_t*)&VirtualShipGetComponent, moduleBase, moduleBase, regions, nRegions);
    //nativeCalls.push_back(virtualShipGetComponent);

    //NativeCallSetup shipMoveSetup;
    //PrepareAndSetupNativeCall(&shipMoveSetup, "Ship move call", NativeCallSetups::ShipDispatch, NativeCallSetups::ShipDispatchPreCall, (uint64_t*)&ShipDispatchPreCode, moduleBase, moduleBase + 0x10E66A0, regions, nRegions);
    //nativeCalls.push_back(shipMoveSetup);

    //NativeCallSetup shipRegionTransferSetup;
    //PrepareAndSetupNativeCall(&shipRegionTransferSetup, "Ship region transfer", NativeCallSetups::ShipTransfer, NativeCallSetups::ShipTransferPreCall, (uint64_t*)&ShipTransferPreCode, moduleBase, moduleBase + 0x74AEF0, regions, nRegions);
    //nativeCalls.push_back(shipRegionTransferSetup);

    NativeCallSetup transferIslandToShipSetup;
    PrepareAndSetupNativeCall(&transferIslandToShipSetup, "Resource Island -> Ship", NativeCallSetups::LoadCargoFromIslandToShip, NativeCallSetups::LoadCargoFromIslandToShipPreCall, (uint64_t*)&TransferIslandToShipPreCode, moduleBase, moduleBase + 0x10FCCD0, regions, nRegions);
    nativeCalls.push_back(transferIslandToShipSetup);

    NativeCallSetup transferShipToIslandSetup;
    PrepareAndSetupNativeCall(&transferShipToIslandSetup, "Resource Island <- Ship", NativeCallSetups::LoadCargoFromShipToIsland, NativeCallSetups::LoadCargoFromShipToIslandPreCall, (uint64_t*)&TransferShipToIslandPreCode, moduleBase, moduleBase + 0x10FB980, regions, nRegions);
    nativeCalls.push_back(transferShipToIslandSetup);

    NativeCallSetup shipDumpSetup;
    PrepareAndSetupNativeCall(&shipDumpSetup, "Ship dump cargo call", NativeCallSetups::ShipCargoDump, NativeCallSetups::ShipCargoDumpPreCall, (uint64_t*)&ShipDumpCargoPreCode, moduleBase, moduleBase + 0x74AEF0, regions, nRegions);
    nativeCalls.push_back(shipDumpSetup);

    for (Hook& hook : hooks)
        EnableHook(&hook, anno);

    SendMessageToHost("Deleting region info");
    delete[] regions;

    SendMessageToHost("Done looking for functions");

    return true;
}

DWORD TargetMain(HANDLE thread)
{
    char logFilePath[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, logFilePath);
    std::string strLogPath = logFilePath;
    strLogPath.append("\\anno_script_log.txt");

    g_logFile = CreateFileA(strLogPath.c_str(), // name of the write
        GENERIC_WRITE,          // open for writing
        FILE_SHARE_READ,        // Allow others to read while open
        NULL,                   // default security
        CREATE_ALWAYS,          // create new file only
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL);                  // no attr. template

    constexpr int bufferSize = 5000;
    constexpr const char* defaultPort = "30010";

    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL, * ptr = NULL, hints;
    const char* sendbuf = "This is a message from inside Anno 1800";
    int iResult;
    int recvbuflen = bufferSize;

    if (!WinsockInitialized() && InitWSA())
        FATAL_ERROR("could not initialize WSA");

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("localhost", defaultPort, &hints, &result);
    if (iResult != 0)
        FATAL_ERROR("Could not resolve address");

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (ConnectSocket == INVALID_SOCKET)
            FATAL_ERROR("Failed to make socket");

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);
    result = NULL;

    if (ConnectSocket == INVALID_SOCKET)
        FATAL_ERROR("Did not create soket");

    outputSocket = ConnectSocket;

    SendMessageToHost("DLL initialized");

    if (!InitializeCriticalSectionAndSpinCount(&shipServiceLock, 0x00000400))
        SendMessageToHost("Failed to create critical section");

    InitNativeThreadCall();

    std::vector<Hook> hooks;
    std::vector<NativeCallSetup> nativeCalls;

    ComputeFunctionOffests(GetCurrentProcess(), hooks, nativeCalls);

    gEndEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("Terminate Event"));

    char ok[1] = { (char)1 };
    send(ConnectSocket, ok, 1, 0);

    while (KeepGoing)
    {
        if (!AutoComms::HandleScriptCall(ConnectSocket))
            KeepGoing = false;
    }

    SetEvent(gEndEvent);

    CloseHandle(gEndEvent);

    for (Hook& hook : hooks)
    {
        std::string error;
        if (!DisableHook(&hook, GetCurrentProcess(), error))
        {
            SEND_FORMATTED("Error disabling hook '%s'", error.c_str());
        }
    }

    for (NativeCallSetup& nativeCall : nativeCalls)
        DeleteNativeSetup(&nativeCall);

    Message exitMessage;
    exitMessage.type = MessageType::Exit;
    HackSendMessage(outputSocket, &exitMessage);

    TearDownNativeThreadCall();

    Sleep(250);

    if (outputSocket != INVALID_SOCKET)
        closesocket(ConnectSocket);

    if (ConnectSocket != INVALID_SOCKET)
        closesocket(ConnectSocket);

    if (result)
        freeaddrinfo(result);

    SendMessageToHost("Injected DLL is going to exit");

    CloseHandle(g_logFile);

    FreeLibraryAndExitThread(self, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        self = hModule;
        CreateThread(NULL, NULL, &TargetMain, NULL, NULL, NULL);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
