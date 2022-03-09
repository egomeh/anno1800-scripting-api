#include "anno_native.h"

bool ExtractResourceNodeInfo(uint64_t address, IslandResource* resourceInfo)
{
	if (!address)
		return false;

	// And out with 32 bits, I'm not sure if type ids can actually be 64 bits!
	resourceInfo->type_id = *(uint64_t*)(address + 0x10) & 0xFFFFFFFF;
	resourceInfo->capacity = *(uint32_t*)(address + 0x1C);
	resourceInfo->amount = *(uint32_t*)(address + 0x20);

	// By the look of it, if this is null, I think it's an invalid node
	if (!resourceInfo->type_id)
		return false;

	return true;
}

