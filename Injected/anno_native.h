#pragma once

#include "structs.gen.h"
#include <unordered_map>

bool ExtractResourceNodeInfo(uint64_t address, IslandResource* resourceInfo, bool& known);

bool ExtractResourceNodeChainInfo(uint64_t address, std::vector<IslandResource>* resourceInfo);

bool DoesIslandBelongToPlayer(uint64_t address);

bool GetIslandName(uint64_t island_address, std::string& name);

bool GetAreaCode(uint64_t area_address, uint64_t* area_code);

bool ExtractIslandChainFromAddress(uint64_t address, bool mustBelongToThePlayer, std::vector<IslandInfo>* islands);

bool GetIslandListFromAreaAddress(uint64_t address, uint64_t* list_pointer);

bool GetBuildingConversion(uint64_t building_address, std::unordered_map<uint32_t, double>& conversion_map, uint64_t component_id);

bool GetBuildingIndustrialConversion(uint64_t building_address, std::unordered_map<uint32_t, double>& conversion_map);

bool GetBuildingBuffConversion(uint64_t building_address, std::unordered_map<uint32_t, double>& conversion_map);

