
#include "pch.h"
#include "native_callbacks.h"
#include "native_calls.h"
#include "globals.h"
#include "Q/structs.h"

#include <unordered_map>

CRITICAL_SECTION shipServiceLock;

uint64_t g_GameTime = 0;
uint64_t g_FrameNumber = 0;

void __declspec(dllexport) NativeCallbackUpdateTime(uint64_t time, uint64_t frame)
{
    g_GameTime = time;
    g_FrameNumber = frame;
}

uint64_t lastAmount = 0;
uint64_t lastAddress = 0;

void __declspec(dllexport) NativeCallbackAddResource(uint64_t amount, uint64_t address)
{
    lastAmount = amount;
    lastAddress = address;
}

void __declspec(dllexport) NativeCallbackRemoveResource(uint64_t amount, uint64_t address)
{
    lastAmount = amount;
    lastAddress = address;
}

std::set<uint64_t> shipLists;
void ShipListIteration(uint64_t address)
{
    EnterCriticalSection(&shipServiceLock);
    shipLists.insert(address);
    LeaveCriticalSection(&shipServiceLock);
}

std::set<uint64_t> regionBases;
void RegionIteration(uint64_t address)
{
    EnterCriticalSection(&shipServiceLock);
    regionBases.insert(address);
    LeaveCriticalSection(&shipServiceLock);
}

std::set<uint64_t> shipContextSets;

bool makingCallInSelf = false;

// Ship context setting callback, this needs a serious tidy-up...
void ShipContextSet(uint64_t pointerValue)
{
    if (makingCallInSelf)
        return;

    SCOPE_LOCK(&shipServiceLock);

    // If we're making a call to move ships, this is where we do so
    CallFromOtherThread callType = externalCallRequest.load();
    if (callType == CallFromOtherThread::None)
        return;

    if (!IsReadable((void*)pointerValue, 8))
        return;

    uint64_t pointerValue1 = ReadU64(pointerValue);

    uint64_t areaOffset = 0x9DE4 + 0x18;

    if (!IsReadable((void*)(pointerValue1 + areaOffset), 8))
        return;

    uint64_t areaID = (ReadU64(pointerValue1 + areaOffset) >> 0x30) & 0xFFFF;

    uint64_t idValue = 0;

    if (areaID == 0xBF37)
        idValue = 2;
    else if (areaID == 0xBF39)
        idValue = 3;
 
    switch (callType)
    {
    case CallFromOtherThread::SetShipWaypoint:
    {
        uint64_t desiredId = (setWaypointForShipInput.idList[0] & 0xffffffff00000000) >> 0x20;

        //  This should likely test for world id....
        if (idValue == desiredId && ShipDispatchPreCode != 0)
        {
            std::vector<uint64_t> inputBuffer1;
            inputBuffer1.push_back(setWaypointForShipInput.idList.size() << 0x20);

            for (uint64_t shipId : setWaypointForShipInput.idList)
                inputBuffer1.push_back(shipId);

            uint64_t packedWaypoint = 0;
            memcpy((uint8_t*)&packedWaypoint, (void*)&setWaypointForShipInput.x, 4);
            memcpy(((uint8_t*)(&packedWaypoint)) + 4, (void*)&setWaypointForShipInput.y, 4);

            inputBuffer1.push_back(packedWaypoint);

            uint64_t rdx = (uint64_t)inputBuffer1.data();
            rdx -= (2 + 8); // 2 for the offset, 8 for total offset, why??? though this is really weird

            std::vector<uint64_t> inputBuffer2;
            inputBuffer2.push_back(rdx);
            inputBuffer2.push_back(0xC);
            inputBuffer2.push_back(0x0);
            inputBuffer2.push_back(0x10000);

            uint64_t middleJump = (uint64_t)inputBuffer2.data();
            uint64_t rcx = (uint64_t)&middleJump;

            makingCallInSelf = true;
            ShipDispatchPreCode(rcx, rdx);
            makingCallInSelf = false;

            SignalCallDone();
        }
        break;
    }
    case CallFromOtherThread::ShipTransferRegion:
    {
        uint64_t desiredId = (transferShipRegionInput.idList[0] & 0xffffffff00000000) >> 0x20;

        //  This should likely test for world id....
        if (idValue == desiredId && ShipDispatchPreCode != 0)
        {
            std::vector<uint64_t> inputBuffer1;
            inputBuffer1.push_back(transferShipRegionInput.idList.size() << 0x20);

            for (uint64_t shipId : transferShipRegionInput.idList)
                inputBuffer1.push_back(shipId);

            inputBuffer1.push_back(transferShipRegionInput.fullWorldId);

            uint64_t rdx = (uint64_t)inputBuffer1.data();
            rdx -= (2 + 8); // 2 for the offset, 8 for total offset, why??? though this is really weird

            std::vector<uint64_t> inputBuffer2;
            inputBuffer2.push_back(rdx);
            inputBuffer2.push_back(0xC);
            inputBuffer2.push_back(0x0);
            inputBuffer2.push_back(0x10000);

            uint64_t middleJump = (uint64_t)inputBuffer2.data();
            uint64_t rcx = (uint64_t)&middleJump;

            makingCallInSelf = true;
            ShipTransferPreCode(rcx, rdx);
            makingCallInSelf = false;

            SignalCallDone();
        }
        break;
    }
    case CallFromOtherThread::ShipDumpCargo:
    {
        uint64_t desiredId = (shipDumpCargoInput.shipId & 0xffffffff00000000) >> 0x20;

        //  This should likely test for world id....
        if (idValue == desiredId && ShipDumpCargoPreCode != 0)
        {
            // Pack the coordinates into a 64-bit value
            uint64_t dumpCoordinate = 0;

            *(reinterpret_cast<float*>(&dumpCoordinate) + 0) = shipDumpCargoInput.x;
            *(reinterpret_cast<float*>(&dumpCoordinate) + 1) = shipDumpCargoInput.y;

            uint64_t rcx = shipDumpCargoInput.cargoBaseAddress;
            uint64_t rdx = shipDumpCargoInput.index;
            uint64_t r8 = (uint64_t)&dumpCoordinate;

            makingCallInSelf = true;
            ShipDumpCargoPreCode(rcx, rdx, r8);
            makingCallInSelf = false;

            SignalCallDone();
        }

        break;
    }
    case CallFromOtherThread::ShipLoadCargo:
    {
        uint64_t desiredId = (shipLoadCargoInput.shipId & 0xffffffff00000000) >> 0x20;

        //  This should likely test for world id....
        if (idValue == desiredId && ShipDumpCargoPreCode != 0)
        {
            std::vector<uint64_t> inputBuffer1;
            inputBuffer1.push_back(shipLoadCargoInput.shipId);

            std::string yesno = shipLoadCargoInput.offloading == 0 ? "yes" : "no";
            SEND_FORMATTED("offloading %s", yesno.c_str());

            if (shipLoadCargoInput.offloading == 0)
            {
                // second line is | islandID (1 byte) | resource ID (2 byts) | zero (1 byte)
                uint64_t island = shipLoadCargoInput.islandId & 0xFFFF;
                uint64_t resource = (shipLoadCargoInput.resourceId & 0xFFFFFFFF) << 16;
                uint64_t slotIndex = (shipLoadCargoInput.slotIndex & 0xFFFFFFFF) << 48;
                uint64_t inputLine2 = 0;
                inputLine2 = island | resource | slotIndex;
                inputBuffer1.push_back(inputLine2);

                uint64_t inputLine3 = (shipLoadCargoInput.amount & 0xFF) << 16;
                inputBuffer1.push_back(inputLine3);
            }
            else
            {
                uint64_t amount = (shipLoadCargoInput.amount & 0xFFFF) << 48;
                uint64_t island = (shipLoadCargoInput.islandId & 0xFFFF) << 32;
                uint64_t slotIndex = (shipLoadCargoInput.slotIndex & 0xFFFF);
                uint64_t inputLine2 = amount | island | slotIndex;
                inputBuffer1.push_back(inputLine2);
                inputBuffer1.push_back(0);
            }

            uint64_t rdx = (uint64_t)inputBuffer1.data();
            rdx -= 0xe; // I still have zero clue what this offset is all about...

            std::vector<uint64_t> inputBuffer2;
            inputBuffer2.push_back(rdx);
            inputBuffer2.push_back(0xC);
            inputBuffer2.push_back(0x0);
            inputBuffer2.push_back(0x10000);

            uint64_t middleJump = (uint64_t)inputBuffer2.data();
            uint64_t rcx = (uint64_t)&middleJump;

            makingCallInSelf = true;
            if (shipLoadCargoInput.offloading == 0)
                TransferIslandToShipPreCode(rcx, rdx);
            else
                TransferShipToIslandPreCode(rcx, rdx);
            makingCallInSelf = false;

            SignalCallDone();
        }

        break;
    }
    }
}

std::unordered_map<uint64_t, uint64_t> islandConsumptionMap;

void IslandConsumptionIteration(uint64_t address)
{
    SCOPE_LOCK(&shipServiceLock);

    uint64_t identifierStructAddress = ReadU64(address + 0x20);

    if (!identifierStructAddress)
        return;

    uint64_t islandID = ReadU64(identifierStructAddress + 0x8) & 0xFFFF;

    islandConsumptionMap[islandID] = address;
}
