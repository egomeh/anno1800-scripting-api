#pragma once

#include "structs.gen.h"

bool ExtractResourceNodeInfo(uint64_t address, IslandResource* resourceInfo);

bool ExtractResourceNodeChianInfo(uint64_t address, std::vector<IslandResource>* resourceInfo);

