#pragma once

#include <inttypes.h>
#include <unordered_map>
#include <set>
#include <windows.h>
#include "Q/structs.h"

extern CRITICAL_SECTION shipServiceLock;

// Time callbacks

extern uint64_t g_GameTime;
extern uint64_t g_FrameNumber;

void __declspec(dllexport) NativeCallbackUpdateTime(uint64_t time, uint64_t frame);

// Resource callbacks

extern uint64_t lastAmount;
extern uint64_t lastAddress;

void __declspec(dllexport) NativeCallbackAddResource(uint64_t amount, uint64_t address);
void __declspec(dllexport) NativeCallbackRemoveResource(uint64_t amount, uint64_t address);

extern std::set<uint64_t> shipLists;

void ShipListIteration(uint64_t address);

extern std::set<uint64_t> regionBases;

void RegionIteration(uint64_t address);

extern std::set<uint64_t> shipContextSets;

// Ship context setting callback
void ShipContextSet(uint64_t pointerValue);

extern std::unordered_map<uint64_t, uint64_t> islandConsumptionMap;

void IslandConsumptionIteration(uint64_t address);
