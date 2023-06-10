#include "remote_call_handler_anno.h"
#include "hook.h"
#include "anno_native.h"
#include "anno_tools.h"
#include "log.h"
#include "tools.h"
#include <windows.h>
#include <set>
#include <unordered_map>

extern "C"
{
	uint64_t get_area_from_tls();
	uint64_t get_anno_component(uint64_t entity_address, uint64_t component_id);
}

bool RemoteCallHandlerAnno::WriteMessageBox(const std::string& message)
{
	MessageBoxA(NULL, message.c_str(), "Write message to log", MB_OK);
	return true;
}

bool RemoteCallHandlerAnno::GetGameTime(uint64_t* time, uint64_t* frame)
{
	HookManager::Get().ExecuteInHookSync(HookedFunction::GameTimeHook,
		[&](HookData data) -> bool
		{
			*time = *(uint64_t*)(data.rbx + 0x60);
			// It seems to actually be a 32 bit integer holding the frame number.
			// So we mask out the garbage.
			*frame = *(uint64_t*)(data.rbx + 0x70) & (uint32_t)(-1);

			return true;
		});

	return true;
}

bool RemoteCallHandlerAnno::DebugGetResourceInfoFromAddress(const uint64_t& address, IslandResource* resource)
{
	bool known;
	return ExtractResourceNodeInfo(module_base, binary_crc, address, resource, known);
}

bool RemoteCallHandlerAnno::DebugGetResourceChainInfoFromAddress(const uint64_t& address, std::vector<IslandResource>* resource)
{
	return ExtractResourceNodeChainInfo(module_base, binary_crc, address, resource);
}

bool RemoteCallHandlerAnno::DebugGetIslandNameFromAddress(const uint64_t& address, std::string* name)
{
	uint64_t name_address = address + 0x118;
	ReadAnnoString(name_address, *name);
	return true;
}

bool RemoteCallHandlerAnno::DebugReadStringFromAddress(const uint64_t& address, std::string* name)
{
	ReadAnnoString(address, *name);
	return true;
}

bool RemoteCallHandlerAnno::DebugGetIslandResources(const uint64_t& address, std::vector<IslandResource>* resources)
{
	uint64_t resource_struct_address = *(uint64_t*)(address + 0x178);
	uint64_t resource_chain_start = **(uint64_t**)(resource_struct_address + 0x38);

	ANNO_LOG("ptr %llx", resource_chain_start);
	//if (!ExtractResourceNodeChainInfo(resource_chain_start, resources))
		//return false;

	return true;
}

bool RemoteCallHandlerAnno::DebugGetIslandChainFromAddress(const uint64_t& address, const bool& mustBelongToThePlayer, std::vector<IslandInfo>* islands)
{
	return ExtractIslandChainFromAddress(address, mustBelongToThePlayer, islands);
}

bool RemoteCallHandlerAnno::DebugGetFirstAreaStructAddress(uint64_t* address)
{
	HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
		[&](HookData data) -> bool
		{
			*address = get_area_from_tls();
			return true;
		});

	return true;
}

bool RemoteCallHandlerAnno::DebugGetAreaWithCode(const uint32_t& areaCode, uint64_t* address)
{
	uint64_t temporary_address = 0;
	int attempts = 0;
	HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
		[&](HookData data) -> bool
		{
			// Don't keep going forever, if you pass a bad code,
			// we want to return eventually
			if (++attempts > 100)
				return true;

			temporary_address = get_area_from_tls();

			uint64_t current_area_code = 0;
			GetAreaCode(temporary_address, &current_area_code);

			if (current_area_code == areaCode)
			{
				*address = temporary_address;
				return true;
			}

			return false;
		});

	return true;
}

bool RemoteCallHandlerAnno::GetWorldIslands(const uint32_t& area, const bool& mustBelongToThePlayer, std::vector<IslandInfo>* islands)
{
	std::vector<uint64_t> areas;
	std::vector<uint64_t> addresses;
	::GetAllAreas(module_base, binary_crc, areas, addresses);

	for (std::size_t i = 0; i < areas.size(); ++i)
	{
		if (areas[i] == (uint64_t)area)
		{
			uint64_t island_list_pointer;
			GetIslandListFromAreaAddress(addresses[i], &island_list_pointer);

			ANNO_LOG("island_list_pointer %llx", island_list_pointer);

			// Follow the pointer twice to get past the 'dud' pointers???
			island_list_pointer = **(uint64_t**)island_list_pointer;

			ExtractIslandChainFromAddress(island_list_pointer, mustBelongToThePlayer, islands);
		}
	}

	return true;
}

bool RemoteCallHandlerAnno::GetAllAreas(std::vector<uint32_t>* areas)
{

	std::vector<uint64_t> collected_areas;
	std::vector<uint64_t> addresses;
	::GetAllAreas(module_base, binary_crc, collected_areas, addresses);

	for (uint64_t area_code : collected_areas)
		areas->push_back((uint32_t)area_code);
	
	return true;
	//constexpr int areas_to_sample = 32;
	//int counter = 0;

	//std::set<uint16_t> areas_sampled;

	//HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
	//	[&](HookData data) -> bool
	//	{
	//		uint64_t area_address = get_area_from_tls();
	//		uint16_t current_area_code = 0;
	//		GetAreaCode(area_address, &current_area_code);

	//		areas_sampled.insert(current_area_code);

	//		if (++counter > areas_to_sample)
	//			return true;

	//		return false;
	//	});

	//for (auto it = areas_sampled.begin(); it != areas_sampled.end(); ++it)
	//{
	//	areas->push_back(*it);
	//}

	//const uint64_t binary_crc_raw = (uint64_t)binary_crc;
	//const uint64_t offset = AnnoDataOffset(binary_crc, DataOffset::GameStateOffset);
	//const uint64_t game_state_address = module_base + offset;

	//uint64_t game_state_base = *(uint64_t*)(game_state_address);

	//// Iterate over all worlds assuming the list is null-terminated (I have no clue)
	//uint64_t world_iterator = game_state_base + 0x48;
	//while (true)
	//{
	//	uint64_t world_address = *(uint64_t*)world_iterator;
	//	
	//	if (!world_address)
	//		break;

	//	uint64_t area_code;
	//	GetAreaCode(world_address, &area_code);
	//	areas->push_back((uint32_t)area_code);

	//	std::string area_name = GetNameFromGUID(module_base, binary_crc, area_code);

	//	ANNO_LOG("Found area with name %s at %llx", area_name.c_str(), world_address);

	//	world_iterator += 8;
	//}

	//return true;
}


bool RemoteCallHandlerAnno::DebugGetNameFromGuid(const uint32_t& guid, std::string* name)
{
	//bool known;
	*name = GetNameFromGUID(module_base, binary_crc, guid);//, known);
	return true;
}

bool RemoteCallHandlerAnno::DebugGetGuidFromName(const std::string& name, uint32_t* guid)
{
	*guid = (uint32_t)GetGuidFromName(name);
	return true;
}

bool RemoteCallHandlerAnno::GetIslandResources(const uint32_t& areaCode, const uint32_t& islandId, std::vector<IslandResource>* resources)
{
	std::vector<IslandInfo> Islands;
	this->GetWorldIslands(areaCode, true, &Islands);

	for (const IslandInfo& island : Islands)
	{
		if (island.island_id != islandId)
			continue;

		uint64_t resource_struct_address = *(uint64_t*)(island.debug_address + 0x178);
		uint64_t resource_chain_start = **(uint64_t**)(resource_struct_address + 0x38);

		if (!ExtractResourceNodeChainInfo(module_base, binary_crc, resource_chain_start, resources))
			return false;
	}

	return true;
}

bool RemoteCallHandlerAnno::DebugVirtualGetComponentFromAddress(const uint64_t& address, const uint64_t& componentId, uint64_t* componentAddress)
{
	*componentAddress = get_anno_component(address, componentId);
	return true;
}

bool RemoteCallHandlerAnno::GetIslandResidentialConsumption(const uint32_t& areaCode, const uint32_t& islandId, std::vector<ResourceConsumption>* resources)
{
	std::vector<uint64_t> areas;
	std::vector<uint64_t> addresses;
	::GetAllAreas(module_base, binary_crc, areas, addresses);

	for (std::size_t i = 0; i < areas.size(); ++i)
	{
		if (areas[i] != (uint64_t)areaCode)
			continue;

		uint64_t intermediate_struct = *(uint64_t*)(addresses[i] + 0x208);

		uint64_t list_ptr = *(uint64_t*)(intermediate_struct + 0x58);
		uint64_t list_end = *(uint64_t*)(intermediate_struct + 0x60);

		while (list_ptr != list_end)
		{
			uint16_t current_island_id = *(uint16_t*)(list_ptr);
			
			if (current_island_id != islandId)
			{
				list_ptr += 0x10;
				continue;
			}

			uint64_t second_intermediate_struct = *(uint64_t*)(list_ptr + 0x08);
			uint64_t third_intermediate_struct = *(uint64_t*)(second_intermediate_struct + 0x118);

			uint64_t consume_list_ptr = *(uint64_t*)(third_intermediate_struct + 0x40);
			uint64_t consume_list_end = *(uint64_t*)(third_intermediate_struct + 0x48);

			while (consume_list_ptr < consume_list_end)
			{
				float rate_modifier = *(float*)(consume_list_ptr + 0xC);

				if (rate_modifier > 0.0f)
				{
					uint32_t resource_type = *(uint32_t*)consume_list_ptr;
					std::string resource_name = ::GetNameFromGUID(module_base, binary_crc, resource_type);

					ResourceConsumption consumption;
					consumption.name = resource_name;
					consumption.type_id = resource_type;
					consumption.rate = 60.0f * rate_modifier;

					resources->push_back(consumption);
				}

				consume_list_ptr += 0x38;
			}

			break;
		}
		
		break;
	}

	return true;

	//int counter = 0;
	//uint64_t areaAddress = 0;

	//uint64_t target_island_address = 0;

	//HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
	//	[&](HookData data) -> bool
	//	{
	//		if (++counter > 512)
	//			return true;

	//		uint64_t area_address = get_area_from_tls();
	//		uint64_t current_area_code = 0;
	//		GetAreaCode(area_address, &current_area_code);

	//		if (areaCode == (uint32_t)current_area_code)
	//		{
	//			// Got the right code
	//			std::vector<IslandInfo> islands;

	//			uint64_t island_list_pointer;
	//			GetIslandListFromAreaAddress(area_address, &island_list_pointer);

	//			// Follow the pointer twice to get past the 'dud' pointers???
	//			island_list_pointer = **(uint64_t**)island_list_pointer;

	//			ExtractIslandChainFromAddress(island_list_pointer, false, &islands);

	//			for (int i = 0; i < islands.size(); ++i)
	//			{
	//				IslandInfo& island = islands[i];

	//				if (island.island_id != islandId)
	//					continue;

	//				target_island_address = island.debug_address;
	//			}

	//			return true;
	//		}

	//		return false;
	//	});

	//if (!target_island_address)
	//	return false;

	//counter = 0;

	//HookManager::Get().ExecuteInHookSync(HookedFunction::ConsumptionHook,
	//	[&](HookData data) -> bool
	//	{
	//		if (++counter > 512)
	//			return true;

	//		uint64_t island_ptr = *(uint64_t*)(data.rcx + 0x30);

	//		if (island_ptr == target_island_address)
	//		{
	//			uint64_t island_consumption_ptr = *(uint64_t*)(data.rcx + 0x38);

	//			if (!island_consumption_ptr)
	//				return true;
	//			
	//			uint64_t i = island_consumption_ptr;

	//			while (true)
	//			{
	//				uint32_t resource_type = *(uint32_t*)i;

	//				// This breaks is the list has an unkown in between some konwns...
	//				// bool known = false;
	//				std::string resource_name = GetNameFromGUID(module_base, binary_crc, resource_type);//, known);

	//				if (resource_type == 0)
	//					break;

	//				if (resource_name.size() == 0)
	//					break;

	//				ResourceConsumption consumption;
	//				consumption.name = resource_name;
	//				consumption.type_id = resource_type;

	//				float rate_modifier = *(float*)(i + 0xC);

	//				consumption.rate = 60.0f * rate_modifier;

	//				resources->push_back(consumption);

	//				i += 0x38;
	//			}

	//			return true;
	//		}

	//		return false;
	//	});

	return true;
}

bool RemoteCallHandlerAnno::DebugGetIslandBuildingAddresses(const uint32_t& areaId, const uint32_t& islandId, std::vector<uint64_t>* addresses)
{
	int counter = 0;

	HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
		[&](HookData data) -> bool
		{
			if (++counter > 512)
				return true;

			uint64_t area_address = get_area_from_tls();
			uint64_t current_area_code = 0;
			GetAreaCode(area_address, &current_area_code);

			if (current_area_code != areaId)
				return false;

			uint64_t step1 = *(uint64_t*)(area_address + 0x200);
			
			uint64_t island_ptr = *(uint64_t*)(step1 + 0xE8);
			uint64_t island_ptr_end = *(uint64_t*)(step1 + 0xF0);

			uint64_t step2 = 0;
			for (uint64_t i = island_ptr; i < island_ptr_end; i += 0x10)
			{
				uint16_t current_island_id = *(uint16_t*)i;
				
				if (islandId == current_island_id)
				{
					step2 = *(uint64_t*)(i + 0x8);
				}
			}

			if (!step2)
				return false;

			uint64_t step3 = *(uint64_t*)(step2 + 0x0E0);
			uint64_t step4 = *(uint64_t*)(step3 + 0x538);

			uint64_t building_list_iterator = *(uint64_t*)(step4 + 0x28);
			uint64_t building_list_end = *(uint64_t*)(step4 + 0x30);

			for (; building_list_iterator != building_list_end; building_list_iterator += 8)
			{
				uint64_t building_base = *(uint64_t*)(building_list_iterator);
				uint32_t building_type = *(uint32_t*)(building_base + 8);

				addresses->push_back(*(uint64_t*)building_list_iterator);
			}

			return true;
		});

	return true;
}

bool RemoteCallHandlerAnno::GetIslandIndustrialConversion(const uint32_t& areaId, const uint32_t& islandId, std::vector<ResourceConsumption>* conversions)
{
	std::unordered_map<uint32_t, double> conversion_map;

	int counter = 0;

	HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
		[&](HookData data) -> bool
		{
			if (++counter > 512)
				return true;

			uint64_t area_address = get_area_from_tls();
			uint64_t current_area_code = 0;
			GetAreaCode(area_address, &current_area_code);

			if (current_area_code != areaId)
				return false;

			uint64_t step1 = *(uint64_t*)(area_address + 0x200);

			uint64_t island_ptr = *(uint64_t*)(step1 + 0xE8);
			uint64_t island_ptr_end = *(uint64_t*)(step1 + 0xF0);

			uint64_t step2 = 0;
			for (uint64_t i = island_ptr; i < island_ptr_end; i += 0x10)
			{
				uint16_t current_island_id = *(uint16_t*)i;

				if (islandId == current_island_id)
				{
					step2 = *(uint64_t*)(i + 0x8);
				}
			}

			if (!step2)
				return false;

			uint64_t step3 = *(uint64_t*)(step2 + 0x0E0);
			uint64_t step4 = *(uint64_t*)(step3 + 0x538);

			uint64_t building_list_iterator = *(uint64_t*)(step4 + 0x28);
			uint64_t building_list_end = *(uint64_t*)(step4 + 0x30);

			for (; building_list_iterator != building_list_end; building_list_iterator += 8)
			{
				uint64_t building_address = *(uint64_t*)building_list_iterator;

				uint32_t building_type_id = *(uint32_t*)(building_address + 8);

				GetBuildingIndustrialConversion(building_address, conversion_map);
				GetBuildingBuffConversion(building_address, conversion_map);
			}

			return true;
		});

	for (const std::pair<uint32_t, double> conversion : conversion_map)
	{
		ResourceConsumption consumption;
		consumption.type_id = conversion.first;
		consumption.rate = (float)conversion.second;

		//bool known_type;
		consumption.name = GetNameFromGUID(module_base, binary_crc, conversion.first);// , known_type);

		if (consumption.name.size() > 0)
			conversions->push_back(consumption);
	}

	return true;
}

bool RemoteCallHandlerAnno::DebugTryEnqueueShipForTrade(const uint32_t& areaId, const uint32_t& islandId, const uint64_t& tradeComponent)
{
	int counter = 0;

	HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
		[&](HookData data) -> bool
		{
			if (++counter > 512)
				return true;

			uint64_t area_address = get_area_from_tls();
			uint64_t current_area_code = 0;
			GetAreaCode(area_address, &current_area_code);

			if (current_area_code != areaId)
				return false;

			uint64_t function_address = module_base + 0x117C800;
			uint64_t islandIdValueThing = ((uint64_t)islandId << 0x20) & 0x0000FFFF00000000;
			reinterpret_cast<void(*)(uint64_t, uint64_t)>(function_address)(tradeComponent, islandIdValueThing);

			return true;
		});

	return true;
}

bool RemoteCallHandlerAnno::MinMaxResourcesOnIsland(const uint32_t& areaId, const uint32_t& islandId,  const uint32_t& lowerBound, const uint32_t& upperBound)
{
	int counter = 0;
	uint64_t areaAddress = 0;

	HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
		[&](HookData data) -> bool
		{
			uint64_t area_address = get_area_from_tls();
			uint64_t current_area_code = 0;
			GetAreaCode(area_address, &current_area_code);

			if (areaId == (uint32_t)current_area_code)
			{
				// Got the right code
				std::vector<IslandInfo> islands;

				uint64_t island_list_pointer;
				GetIslandListFromAreaAddress(area_address, &island_list_pointer);

				// Follow the pointer twice to get past the 'dud' pointers???
				island_list_pointer = **(uint64_t**)island_list_pointer;

				ExtractIslandChainFromAddress(island_list_pointer, false, &islands);

				for (int i = 0; i < islands.size(); ++i)
				{
					IslandInfo& island = islands[i];

					if (island.island_id != islandId)
						continue;

					std::vector<IslandResource> resources;

					uint64_t resource_struct_address = *(uint64_t*)(island.debug_address + 0x170);
					uint64_t resource_chain_start = **(uint64_t**)(resource_struct_address + 0x40);

					if (!ExtractResourceNodeChainInfo(module_base, binary_crc, resource_chain_start, &resources))
						return false;

					for (uint64_t i = 0; i < resources.size(); ++i)
					{
						if ((uint32_t)resources[i].amount > ((uint32_t)resources[i].capacity - upperBound))
						{
							*(uint32_t*)(resources[i].amount_ptr) = resources[i].capacity - upperBound;
						}

						if ((uint32_t)resources[i].amount < lowerBound)
						{
							*(uint32_t*)(resources[i].amount_ptr) = lowerBound;
						}
					}
				}

				return true;
			}

			if (++counter > 512)
				return true;

			return false;
		});

	return true;
}

bool RemoteCallHandlerAnno::DebugGetAreaAddress(const uint32_t& areaID, uint64_t* areaAddress)
{
	uint64_t area_address = 0;
	int attempts = 0;

	HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
		[&](HookData data) -> bool
		{
			// Don't keep going forever, if you pass a bad code,
			// we want to return eventually
			if (++attempts > 512)
				return true;

			area_address = get_area_from_tls();

			uint64_t current_area_code = 0;
			GetAreaCode(area_address, &current_area_code);

			if (current_area_code == areaID)
			{
				*areaAddress = area_address;
				return true;
			}

			return false;
		});

	return true;
}

bool RemoteCallHandlerAnno::DebugGetVehicleLists(std::vector<uint64_t>* vehicleLists)
{
	int attempts = 0;
	HookManager::Get().ExecuteInHookSync(HookedFunction::VehicleSortingHook,
		[&](HookData data) -> bool
		{
			// Do 32 runs, expect all lists are visited
			if (++attempts > 8)
				return true;

			const uint64_t current_list = data.rcx;

			bool hasList = false;
			for (int i = 0; i < vehicleLists->size(); ++i)
			{
				if ((*vehicleLists)[i] == current_list)
					hasList = true;
			}

			if (!hasList)
				vehicleLists->push_back(current_list);

			return false;
		});

	return true;
}

bool RemoteCallHandlerAnno::GetShipsInRegion(const uint32_t& areaId, std::vector<ShipInfo>* ships)
{
	int attempts = 0;
	HookManager::Get().ExecuteInHookSync(HookedFunction::VehicleSortingHook,
		[&](HookData data) -> bool
		{
			// Don't run forever, in case we've passed an invalid areaId
			// 512 is a bit excessive, but it should resovle in at most
			// number of regions iterations.
			if (++attempts > 512)
				return true;

			uint64_t current_area = get_area_from_tls();
			uint64_t current_area_id;
			GetAreaCode(current_area, &current_area_id);

			if (current_area_id != (uint16_t)areaId)
				return false;

			uint64_t ship_list_struct = data.rcx;
			uint64_t list_ptr = *(uint64_t*)(ship_list_struct + 0x28);
			uint64_t list_end = *(uint64_t*)(ship_list_struct + 0x30);

			// If either pointer is null, we assume there are no ships
			if (list_ptr == 0)
				return true;

			if (list_end == 0)
				return true;

			while (list_ptr < list_end)
			{
				uint64_t id_field = *(uint64_t*)list_ptr;
				uint64_t ptr_field = *(uint64_t*)(list_ptr + 0x8);

				ShipInfo ship_info;
				ship_info.id = (uint32_t)id_field;
				ship_info.debug_address = ptr_field;

				// Get the ships name
				uint64_t name_component = get_anno_component(ptr_field, 0x14b);

				std::string ship_name;
				if (!ReadAnnoString(name_component + 0x28, ship_name))
					ship_name = "[failed to read ship name]";

				ship_info.name = ship_name;

				// Get the ships location data
				uint64_t location_component = get_anno_component(ptr_field, 0x134);

				uint64_t task_queue_size = *(uint64_t*)(location_component + 0x38);
				ship_info.idle = task_queue_size == 0;

				Vector position;
				position.x = *(float*)(location_component + 0x44);
				position.y = *(float*)(location_component + 0x48);

				ship_info.position = position;
				ship_info.rotation = *(float*)(location_component + 0x4c);

				// Get the cargo information
				uint64_t cargo_component = get_anno_component(ptr_field, 0x2c5);

				uint64_t cargo_struct = *(uint64_t*)(cargo_component + 0x30);

				uint64_t cargo_slot_ptr = *(uint64_t*)(cargo_struct + 0x10);
				uint64_t cargo_slot_end = *(uint64_t*)(cargo_struct + 0x18);

				while (cargo_slot_ptr < cargo_slot_end)
				{
					CargoSlot current_slot;

					uint32_t amount = *(uint32_t*)(cargo_slot_ptr + 0x20);

					current_slot.amount = amount;
					current_slot.occupied = amount > 0;
					current_slot.type_id = 0;
					current_slot.type_name = "[unkonwn]";

					if (amount > 0)
					{
						uint64_t type_struct = *(uint64_t*)(cargo_slot_ptr + 0x8);
						current_slot.type_id = *(uint32_t*)(type_struct + 0x8);
						//bool known_type = false;
						current_slot.type_name = GetNameFromGUID(module_base, binary_crc, (uint64_t)current_slot.type_id);// , known_type);
					}

					ship_info.cargo.push_back(current_slot);

					cargo_slot_ptr += 0x28;
				}

				// Add the ship to the list
				ships->push_back(ship_info);

				list_ptr += 0x18;
			}

			return true;
		});

	return true;
}

bool RemoteCallHandlerAnno::DebugGetEntityComponets(const uint64_t& entityAddress, std::vector<DebugComponent>* components)
{
	HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
		[&](HookData data) -> bool
		{
			for (uint64_t id = 0; id < 2048; ++id)
			{
				uint64_t component_address = get_anno_component(entityAddress, id);
				
				if (component_address == 0)
					continue;

				DebugComponent debug_component;
				debug_component.id = id;
				debug_component.address = component_address;
				components->push_back(debug_component);
			}

			return true;
		});

	return true;
}

bool RemoteCallHandlerAnno::SetIslandResource(const uint32_t& world_id, const uint32_t& island_id, const uint32_t& resource_type, const uint32_t& target_value, uint32_t* previous_value)
{
	int counter = 0;
	HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
		[&](HookData data) -> bool
		{
			uint64_t area_address = get_area_from_tls();
			uint64_t current_area_code = 0;
			GetAreaCode(area_address, &current_area_code);

			if (world_id == (uint32_t)current_area_code)
			{
				// Got the right code
				std::vector<IslandInfo> islands;

				uint64_t island_list_pointer;
				GetIslandListFromAreaAddress(area_address, &island_list_pointer);

				// Follow the pointer twice to get past the 'dud' pointers???
				island_list_pointer = **(uint64_t**)island_list_pointer;

				ExtractIslandChainFromAddress(island_list_pointer, false, &islands);

				for (int i = 0; i < islands.size(); ++i)
				{
					IslandInfo& island = islands[i];

					if (island.island_id != island_id)
						continue;

					uint64_t resource_struct_address = *(uint64_t*)(island.debug_address + 0x170);
					uint64_t resource_chain_start = **(uint64_t**)(resource_struct_address + 0x40);

					std::vector< IslandResource> resources;

					if (!ExtractResourceNodeChainInfo(module_base, binary_crc, resource_chain_start, &resources))
						return false;

					for (IslandResource resource : resources)
					{
						if (resource.type_id != resource_type)
							continue;

						*previous_value = resource.amount;
						*(uint32_t*)resource.amount_ptr = target_value;
					}
				}

				return true;
			}

			if (++counter > 512)
				return true;

			return false;
		});

	return true;
}

