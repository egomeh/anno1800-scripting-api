#include "anno_native.h"
#include "anno_tools.h"
#include "hook.h"
#include "log.h"
#include <set>
#include <algorithm>

bool ExtractResourceNodeInfo(uint64_t module_base, BinaryCRC32 binary_crc, uint64_t address, IslandResource* resourceInfo, bool& known)
{
	if (!address)
		return false;

	// And out with 32 bits, I'm not sure if type ids can actually be 64 bits!
	resourceInfo->type_id = *(uint32_t*)(address + 0x10);
	resourceInfo->capacity = *(uint32_t*)(address + 0x1C);
	resourceInfo->amount_ptr = address + 0x20;
	resourceInfo->amount = *(uint32_t*)(resourceInfo->amount_ptr);
	resourceInfo->name = ::GetNameFromGUID(module_base, binary_crc, resourceInfo->type_id);

	// By the look of it, if this is null, I think it's an invalid node
	if (!resourceInfo->type_id)
		return false;

	return true;
}

bool ExtractResourceNodeChainInfo(uint64_t module_base, BinaryCRC32 binary_crc, uint64_t address, std::vector<IslandResource>* resourceChain)
{
	std::set<uint64_t> visited_addresses;

	while (true)
	{
		IslandResource resource;

		bool known = false;
		if (ExtractResourceNodeInfo(module_base, binary_crc, address, &resource, known))
		{
			if (resource.amount > 0)
				resourceChain->push_back(resource);
		}

		visited_addresses.insert(address);
		address = *(uint64_t*)address;

		if (visited_addresses.find(address) != visited_addresses.end())
			return true;
	}
}

bool DoesIslandBelongToPlayer(uint64_t address)
{
	// I have almost no clue if this is right...?
	// Maybe check the code that references this flag
	uint16_t belong_flag = *(uint16_t*)(address + 0x4e);
	return belong_flag == 0;
}

bool GetIslandName(uint64_t module_base, BinaryCRC32 binary_crc, uint64_t island_address, std::string& name)
{
	uint32_t name_uid = *(uint32_t*)(island_address + 0x140);
	name = ::GetNameFromGUID(module_base, binary_crc, (uint64_t)name_uid);
	return true;
}

bool ExtractIslandChainFromAddress(uint64_t module_base, BinaryCRC32 binary_crc, uint64_t address, bool mustBelongToThePlayer, std::vector<IslandInfo>* islands)
{
	std::vector<uint64_t> visited_islands;

	uint64_t current_address = address;
	int attempts = 0;

	while (true)
	{
		visited_islands.push_back(current_address);

		// Don't go on forever if an assumption is wrong
		if (++attempts > 512)
			break;

		uint32_t island_id = *(uint32_t*)(current_address + 0x10) & 0xffff;
		uint64_t current_island_address = current_address + 0x18;
		uint64_t pointer_that_may_indicate_if_it_is_a_valid_island = *(uint32_t*)(current_island_address + 0x8);

		if ((DoesIslandBelongToPlayer(current_island_address) || !mustBelongToThePlayer) && island_id
			&& pointer_that_may_indicate_if_it_is_a_valid_island)
		{
			IslandInfo info;
			info.debug_address = current_island_address;
			info.island_id = island_id;

			ANNO_LOG("island address %llx", current_island_address);

			bool CouldGetName = GetIslandName(module_base, binary_crc, current_island_address, info.name);

			if (CouldGetName && info.name.size() > 0)
			{
				islands->push_back(info);
			}
			else
			{
				ANNO_LOG("Failed to get name from island %llx: %s", current_island_address, info.name.c_str());
			}
		}

		current_address = *(uint64_t*)current_address;

		if (std::find(visited_islands.begin(), visited_islands.end(), current_address) != visited_islands.end())
			break;
	}

	return true;
}

bool GetAreaCode(uint64_t area_address, uint64_t* area_code)
{
	if (!area_address)
		return false;

	*area_code = *(uint64_t*)(area_address + 0x8) & 0xFFFFFFFF;

	return true;
}

bool GetIslandListFromAreaAddress(uint64_t address, uint64_t* list_pointer)
{
	uint64_t intermediate_struct = *(uint64_t*)(address + 0x208);
	*list_pointer = *(uint64_t*)(intermediate_struct + 0x138);

	return true;
}

bool GetBuildingConversion(uint64_t building_address, std::unordered_map<uint32_t, double>& conversion_map, uint64_t component_id)
{
	uint64_t production_node = get_anno_component(building_address, component_id);

	if (!production_node)
		return true;

	uint32_t output_type = *(uint32_t*)(production_node + 0x180);
	std::vector<uint32_t> input_types;

	float efficiency_rate = *(float*)(production_node + 0x20C);
	uint32_t base_production_time_ms = *(uint32_t*)(production_node + 0x210);

	// This uses xmm registers, we might want to store/restore them in the trampoline
	// or move this out of the hook call
	const double ms_per_unit = (double)(base_production_time_ms);
	const double base_minutes_per_unit = ms_per_unit / 60'000.0;
	const double base_units_per_minute = 1. / base_minutes_per_unit;
	const double actual_units_per_minute = base_units_per_minute * (double)efficiency_rate;

	uint64_t consume_list_start = *(uint64_t*)(production_node + 0x160);
	uint64_t consume_list_end = *(uint64_t*)(production_node + 0x168);

	if (consume_list_start != 0)
	{
		for (uint64_t consume_list_iterator = consume_list_start; consume_list_iterator != consume_list_end; consume_list_iterator += 8)
		{
			uint32_t consume_type = *(uint32_t*)(consume_list_iterator);
			input_types.push_back(consume_type);
		}
	}

	if (conversion_map.count(output_type) == 0)
	{
		conversion_map[output_type] = -actual_units_per_minute;
	}
	else
	{
		conversion_map[output_type] += -actual_units_per_minute;
	}

	uint64_t extra_goods_list_start = *(uint64_t*)(production_node + 0x1A0);
	uint64_t extra_goods_list_end = *(uint64_t*)(production_node + 0x1A8);

	if (extra_goods_list_start)
	{
		for (uint64_t i = extra_goods_list_start; i < extra_goods_list_end; i += 8)
		{
			uint32_t extra_good_type = *(uint32_t*)(i);
			float extra_good_rate = *(float*)(i + 4);


			if (conversion_map.count(extra_good_type) == 0)
			{
				conversion_map[extra_good_type] = -actual_units_per_minute * extra_good_rate;
			}
			else
			{
				conversion_map[extra_good_type] += -actual_units_per_minute * extra_good_rate;
			}
		}
	}

	for (size_t i = 0; i < input_types.size(); ++i)
	{
		if (conversion_map.count(input_types[i]) == 0)
		{
			conversion_map[input_types[i]] = actual_units_per_minute;
		}
		else
		{
			conversion_map[input_types[i]] += actual_units_per_minute;
		}
	}

	return true;
}

bool GetBuildingIndustrialConversion(uint64_t building_address, std::unordered_map<uint32_t, double>& conversion_map)
{
	return GetBuildingConversion(building_address, conversion_map, 712);
}

bool GetBuildingBuffConversion(uint64_t building_address, std::unordered_map<uint32_t, double>& conversion_map)
{
	return GetBuildingConversion(building_address, conversion_map, 708);
}

bool GetAllAreas(uint64_t module_base, BinaryCRC32 binary_crc, std::vector<uint64_t>& areas, std::vector<uint64_t>& addresses)
{
	const uint64_t offset = AnnoDataOffset(binary_crc, DataOffset::GameStateOffset);
	const uint64_t game_state_address = module_base + offset;

	uint64_t game_state_base = *(uint64_t*)(game_state_address);

	// Iterate over all worlds assuming the list is null-terminated (I have no clue)
	uint64_t world_iterator = game_state_base + 0x48;

	while (true)
	{
		uint64_t world_address = *(uint64_t*)world_iterator;

		if (!world_address)
			break;

		uint64_t area_code;
		GetAreaCode(world_address, &area_code);
		areas.push_back(area_code);
		addresses.push_back(world_address);

		std::string area_name = GetNameFromGUID(module_base, binary_crc, area_code);

		/// ANNO_LOG("Found area with name %s at %llx", area_name.c_str(), world_address);

		world_iterator += 8;
	}

	return true;
}

std::string GetNameFromGUID(uint64_t module_base, BinaryCRC32 binary_crc, uint64_t guid)
{
    uint64_t asset_name_database_ptr = module_base + AnnoDataOffset(binary_crc, DataOffset::AssetNameDatabase);
	uint64_t asset_name_database = *(uint64_t*)(asset_name_database_ptr) + 0x28;

	uint64_t guid_to_name_ptr = module_base + AnnoDataOffset(binary_crc, DataOffset::FunctionGUIDToName);
	uint64_t name_ptr = ((uint64_t(*)(uint64_t, uint64_t, uint64_t))(guid_to_name_ptr))(asset_name_database, guid, 1);

	std::string name;
	ReadAnnoString(name_ptr, name);

	return name;
}

