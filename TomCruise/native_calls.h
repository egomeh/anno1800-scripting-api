#pragma once

#include <inttypes.h>
#include <atomic>


enum class CallFromOtherThread
{
    None,
    SetShipWaypoint,
    ShipTransferRegion,
    ShipDumpCargo,
    ShipLoadCargo,
};

extern std::atomic<CallFromOtherThread> externalCallRequest;

typedef uint64_t (*VirtualShipGetComponentType)(uint64_t, uint64_t);
extern VirtualShipGetComponentType VirtualShipGetComponent;

typedef void (*ShipDispatchPreCodeType)(uint64_t, uint64_t);
extern ShipDispatchPreCodeType ShipDispatchPreCode;

typedef void (*ShipTransferPreCodeType)(uint64_t, uint64_t);
extern ShipTransferPreCodeType ShipTransferPreCode;

typedef void (*TransferIslandToShipType)(uint64_t, uint64_t);
extern TransferIslandToShipType TransferIslandToShipPreCode;

typedef void (*TransferShipToIslandType)(uint64_t, uint64_t);
extern TransferShipToIslandType TransferShipToIslandPreCode;

typedef void (*ShipDumpCargoType)(uint64_t cargoComponent, uint64_t slotId, uint64_t coordPtr);
extern ShipDumpCargoType ShipDumpCargoPreCode;

void SignalCallDone();

bool InitNativeThreadCall();
bool TearDownNativeThreadCall();

struct SetShipWaypointCallInput
{
    std::vector<uint64_t> idList;
    float x;
    float y;
    uint64_t worldId;
};

struct TransferShipRegionCallInput
{
    std::vector<uint64_t> idList;
    uint64_t fullWorldId; // We can extract this from the resgion data
};

struct ShipDumpCargoCallInput
{
    uint64_t shipId;
    uint64_t cargoBaseAddress;
    uint64_t index;
    float x;
    float y;
};

struct ShipLoadCargoCallInput
{
    uint64_t islandId;
    uint64_t shipId;
    uint64_t resourceId;
    uint64_t amount;
    uint64_t slotIndex;
    uint64_t offloading;
};

extern SetShipWaypointCallInput setWaypointForShipInput;
extern TransferShipRegionCallInput transferShipRegionInput;
extern ShipDumpCargoCallInput shipDumpCargoInput;
extern ShipLoadCargoCallInput shipLoadCargoInput;

bool SetWaypointForShips(std::vector<uint64_t> shipIdList, float x, float y, std::string& error);
bool TransferShipsToWorlds(std::vector<uint64_t> shipIds, uint64_t worldId);
bool DumpShipCargo(uint64_t shipId, uint64_t index, float x, float y);
bool LoadShipCargo(uint64_t islandId, uint64_t shipId, uint64_t resourceId, uint64_t amount, uint64_t slotIndex, uint64_t offloading);

bool GetShipName(uint64_t shipId, std::string* name);
bool GetShipsByName(const std::string& name, std::vector<uint64_t>* shipList);
bool GetAllShipsInWorld(const uint64_t& worldId, std::vector<uint64_t>* shipList);

bool GetShipAddress(uint64_t shipId, uint64_t* result);

