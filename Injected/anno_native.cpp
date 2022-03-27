#include "anno_native.h"
#include "anno_tools.h"
#include "log.h"

bool ExtractResourceNodeInfo(uint64_t address, IslandResource* resourceInfo)
{
	if (!address)
		return false;

	// And out with 32 bits, I'm not sure if type ids can actually be 64 bits!
	resourceInfo->type_id = *(uint32_t*)(address + 0x10);
	resourceInfo->capacity = *(uint32_t*)(address + 0x1C);
	resourceInfo->amount = *(uint32_t*)(address + 0x20);

	// By the look of it, if this is null, I think it's an invalid node
	if (!resourceInfo->type_id)
		return false;

	return true;
}

bool ExtractResourceNodeChianInfo(uint64_t address, std::vector<IslandResource>* resourceChain)
{
	IslandResource resource;

	if (ExtractResourceNodeInfo(address, &resource))
	{
		// If we could extract the resource node, add it to the chain.
		resourceChain->push_back(resource);

		// Then find the next node
		ExtractResourceNodeChianInfo(*(uint64_t*)address, resourceChain);
	}

	// After the first time we reach an invalid node, we exit

	return true;
}

bool DoesIslandBelongToPlayer(uint64_t address)
{
	// I have almost no clue if this is right...?
	uint16_t belong_flag = *(uint16_t*)(address + 0x4e);
	return belong_flag == 0;
}

bool GetIslandName(uint64_t island_address, std::string& name)
{
	uint64_t name_address = island_address + 0x118;
	return ReadAnnoString(name_address, name);
}

bool ExtractIslandChainFromAddress(uint64_t address, std::vector<IslandInfo>* islands)
{
	uint64_t current_address = address;
	int attempts = 0;

	while (true)
	{
		// Don't go on forever if an assumption is wrong
		if (++attempts > 512)
			break;

		uint32_t island_id = *(uint32_t*)(current_address + 0x10) & 0xffff;
		uint64_t current_island_address = current_address + 0x18;

		if (DoesIslandBelongToPlayer(current_island_address))
		{
			IslandInfo info;
			info.debug_address = current_island_address;
			info.island_id = island_id;

			if (GetIslandName(current_island_address, info.name))
				islands->push_back(info);
		}

		current_address = *(uint64_t*)current_address;

		if (current_address == address)
			break;
	}

	return true;
}

bool GetAreaCode(uint64_t area_address, uint16_t* area_code)
{
	if (!area_address)
		return false;

	*area_code = *(uint16_t*)(area_address + 0x8);

	return true;
}

bool GetIslandListFromAreaAddress(uint64_t address, uint64_t* list_pointer)
{
	uint64_t intermediate_struct = *(uint64_t*)(address + 0x200);
	*list_pointer = *(uint64_t*)(intermediate_struct + 0x80);

	return true;
}
