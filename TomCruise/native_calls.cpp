#include "pch.h"
#include "native_calls.h"
#include <windows.h>
#include "native_callbacks.h"
#include "native_traversal.h"
#include "globals.h"
#include "Q/Tools.h"

namespace
{
    CRITICAL_SECTION NativeThreadCallLock;
    HANDLE NativeThreadCallReturnEvent;

    bool ExchangeExternalCall(CallFromOtherThread callType, std::string& error)
    {
        bool success = false;

        ResetEvent(NativeThreadCallReturnEvent);
        externalCallRequest.store(callType);

        // If the callback takes more than a second, there is litle chance of success
        DWORD waitResult = WaitForSingleObject(NativeThreadCallReturnEvent, 1'000);
        switch (waitResult)
        {
        case WAIT_OBJECT_0:
            success = true;
            break;
        case WAIT_TIMEOUT:
            error = "Timeout";
            break;
        case WAIT_ABANDONED:
            error = "Abandonned";
            break;
        default:
            break; // well....
        }

        return success;
    }
}

std::atomic<CallFromOtherThread> externalCallRequest = CallFromOtherThread::None;

void SignalCallDone()
{
    externalCallRequest.store(CallFromOtherThread::None);
    SetEvent(NativeThreadCallReturnEvent);
}

bool InitNativeThreadCall()
{
    if (!InitializeCriticalSectionAndSpinCount(&NativeThreadCallLock, 0x00000400))
        return false;

    NativeThreadCallReturnEvent = CreateEvent(
        NULL,               // default security attributes
        TRUE,               // manual-reset event
        FALSE,              // initial state is nonsignaled
        TEXT("Native call return event")  // object name
    );

    if (NativeThreadCallReturnEvent == NULL)
        return false;

    ResetEvent(NativeThreadCallReturnEvent);

    return true;
}

bool TearDownNativeThreadCall()
{
    CloseHandle(NativeThreadCallReturnEvent);
    DeleteCriticalSection(&NativeThreadCallLock);

    return true;
}

VirtualShipGetComponentType VirtualShipGetComponent = (VirtualShipGetComponentType)0;

ShipDispatchPreCodeType ShipDispatchPreCode = (ShipDispatchPreCodeType)0;
ShipClearCommandQueuePreCodeType ShipClearCommandQueuePreCode = (ShipClearCommandQueuePreCodeType)0;
ShipTransferPreCodeType ShipTransferPreCode = (ShipTransferPreCodeType)0;
TransferIslandToShipType TransferIslandToShipPreCode = (TransferIslandToShipType)0;
TransferShipToIslandType TransferShipToIslandPreCode = (TransferShipToIslandType)0;
ShipDumpCargoType ShipDumpCargoPreCode = (ShipDumpCargoType)0;

SetShipWaypointCallInput setWaypointForShipInput;
TransferShipRegionCallInput transferShipRegionInput;
ShipDumpCargoCallInput shipDumpCargoInput;
ShipLoadCargoCallInput shipLoadCargoInput;

bool SetWaypointForShips(std::vector<uint64_t> shipIdList, float x, float y, std::string& error)
{
    if (shipIdList.empty())
        return true;

    uint64_t firstId = shipIdList[0];

    firstId = (firstId & 0xffffffff00000000) >> 0x20;

    for (uint64_t id : shipIdList)
    {
        uint64_t otherID = (id & 0xffffffff00000000) >> 0x20;
        if (otherID != firstId)
        {
            error = "Not all world ID's match";
            return false;
        }
    }

    setWaypointForShipInput.worldId = firstId;

    bool success = false;

    SCOPE_LOCK(&NativeThreadCallLock);

    setWaypointForShipInput.idList = shipIdList;
    setWaypointForShipInput.x = x;
    setWaypointForShipInput.y = y;

    success = ExchangeExternalCall(CallFromOtherThread::SetShipWaypoint, error);

    return success;
}

bool TransferShipsToWorlds(std::vector<uint64_t> shipIds, uint64_t worldId)
{
    std::string error;

    // First validate ship id list: they should have the same  wolrd ids
    if (shipIds.empty())
        return true;

    uint64_t firstShipId = (shipIds[0] >> 0x20) & 0xFF;

    for (uint64_t id : shipIds)
    {
        if (((id >> 0x20) & 0xFF) != firstShipId)
        {
            SEND_FORMATTED("Not all ship ids are in the same world");
        }
    }

    // Then grab the full world id
    uint64_t fullWorldId = 0;

    for (uint64_t regionAddress : regionBases)
    {
        if (!regionAddress)
            continue;

        uint64_t regionIdStructPtr = ReadU64(regionAddress + 0x30);

        if (!regionIdStructPtr)
            continue;

        uint64_t regionIdValue = ReadU64(regionIdStructPtr + 0x8);

        uint64_t shortId = (regionIdValue >> 0x20) & 0xFF;

        if (shortId != worldId)
            continue;

        fullWorldId = regionIdValue & 0xFFFFFFFF;
    }

    if (!fullWorldId)
    {
        SEND_FORMATTED("Could not find the full world id");
        return false;
    }

    SEND_FORMATTED("Full target worldid %llx", fullWorldId);

    SCOPE_LOCK(&NativeThreadCallLock);

    transferShipRegionInput.fullWorldId = fullWorldId;
    transferShipRegionInput.idList = shipIds;

    bool success = ExchangeExternalCall(CallFromOtherThread::ShipTransferRegion, error);

    return success;
}

bool DumpShipCargo(uint64_t shipId, uint64_t index, float x, float y)
{
    uint64_t shipBase = 0;

    if (!GetShipById(shipId, &shipBase))
        return false;

    uint64_t cargoBase = VirtualShipGetComponent(shipBase, ComponentIdCargo);

    if (!cargoBase)
        return false;

    std::string error;
    bool success = false;

    SCOPE_LOCK(&NativeThreadCallLock);

    shipDumpCargoInput.shipId = shipId;
    shipDumpCargoInput.cargoBaseAddress = cargoBase;
    shipDumpCargoInput.index = index;
    shipDumpCargoInput.x = x;
    shipDumpCargoInput.y = y;

    // SEND_FORMATTED("Dumping cargo with ship id %llx - base address %llx - index %llx", shipId, cargoBase, index);

    success = ExchangeExternalCall(CallFromOtherThread::ShipDumpCargo, error);

    return success;
}

bool LoadShipCargo(uint64_t islandId, uint64_t shipId, uint64_t resourceId, uint64_t amount, uint64_t slotIndex, uint64_t offloading)
{
    uint64_t shipBase = 0;

    std::string error;
    bool success = false;

    SCOPE_LOCK(&NativeThreadCallLock);

    shipLoadCargoInput.islandId = islandId;
    shipLoadCargoInput.shipId = shipId;
    shipLoadCargoInput.resourceId = resourceId;
    shipLoadCargoInput.amount = amount;
    shipLoadCargoInput.slotIndex = slotIndex;
    shipLoadCargoInput.offloading = offloading;

    success = ExchangeExternalCall(CallFromOtherThread::ShipLoadCargo, error);

    return success;
}

bool GetShipName(uint64_t shipId, std::string* name)
{
    name->clear();

    EnterCriticalSection(&shipServiceLock);
    for (uint64_t address : shipLists)
    {
        uint64_t listPtr = ReadU64(address + 0x20);
        uint64_t size = ReadU64(address + 0x28);

        for (uint64_t i = 0; i < size; ++i)
        {
            uint64_t id = ReadU64(listPtr + i * 0x18);
            uint64_t shipPtr = ReadU64(listPtr + i * 0x18 + 0x8);

            if (id != shipId)
                continue;

            //for (uint64_t componentId = 0; componentId < 2048; ++componentId)
            //{
            //    uint64_t componentAddress = VirtualShipGetComponent(shipPtr, componentId);

            //    if (componentAddress)
            //        SEND_FORMATTED("%llx : %lld", componentAddress, componentId);
            //}

            // Name
            uint64_t nameComponentAddress = VirtualShipGetComponent(shipPtr, ComponentIdName);
            *name = GetStringFromNameComponent(nameComponentAddress);
        }
    }
    LeaveCriticalSection(&shipServiceLock);

    return name->size() > 0;
}

bool GetShipsByName(const std::string& name, std::vector<uint64_t>* shipList)
{
    shipList->clear();

    EnterCriticalSection(&shipServiceLock);
    for (uint64_t address : shipLists)
    {
        uint64_t listPtr = ReadU64(address + 0x20);
        uint64_t size = ReadU64(address + 0x28);

        for (uint64_t i = 0; i < size; ++i)
        {
            uint64_t id = ReadU64(listPtr + i * 0x18);
            uint64_t shipPtr = ReadU64(listPtr + i * 0x18 + 0x8);

            // Name
            uint64_t nameComponentAddress = VirtualShipGetComponent(shipPtr, 266);
            std::string currentName = GetStringFromNameComponent(nameComponentAddress);

            if (name.compare(currentName) == 0)
            {
                shipList->push_back(id);
                SEND_FORMATTED("Found '%s' with id %llx ptr %llx", currentName.c_str(), id, shipPtr);
            }
        }
    }
    LeaveCriticalSection(&shipServiceLock);

    return true;
}

bool GetAllShipsInWorld(const uint64_t& worldId, std::vector<uint64_t>* shipList)
{
    shipList->clear();

    EnterCriticalSection(&shipServiceLock);
    for (uint64_t address : shipLists)
    {
        uint64_t listPtr = ReadU64(address + 0x20);
        uint64_t size = ReadU64(address + 0x28);

        for (uint64_t i = 0; i < size; ++i)
        {
            uint64_t id = ReadU64(listPtr + i * 0x18);
            uint64_t shipPtr = ReadU64(listPtr + i * 0x18 + 0x8);

            if (((id & 0xFFFFFFFF000000) >> 0x20) == worldId)
            {
                shipList->push_back(id);
            }
        }
    }
    LeaveCriticalSection(&shipServiceLock);

    return true;
}

bool GetShipAddress(uint64_t shipId, uint64_t* result)
{
    bool found = false;

    EnterCriticalSection(&shipServiceLock);
    for (uint64_t address : shipLists)
    {
        uint64_t listPtr = ReadU64(address + 0x20);
        uint64_t size = ReadU64(address + 0x28);

        for (uint64_t i = 0; i < size && !found; ++i)
        {
            uint64_t id = ReadU64(listPtr + i * 0x18);
            uint64_t shipPtr = ReadU64(listPtr + i * 0x18 + 0x8);

            if (shipId == id)   
            {
                *result = shipPtr;
                found = true;
            }
        }
    }
    LeaveCriticalSection(&shipServiceLock);

    if (!found)
    {
        SEND_FORMATTED("Could not find address for ship with address %llx", shipId);
        SEND_FORMATTED("Ther are %lld ship lists", shipLists.size());
        
        for (uint64_t address : shipLists)
            SEND_FORMATTED("%llx", address);
    }

    return found;
}


