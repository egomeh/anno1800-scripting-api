#include "anno_native.h"
#include "anno_tools.h"

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
