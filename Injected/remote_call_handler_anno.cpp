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
	uint64_t virtual_get_component(uint64_t object_address, uint64_t component_id);
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
	return ExtractResourceNodeInfo(address, resource, known);
}

bool RemoteCallHandlerAnno::DebugGetResourceChainInfoFromAddress(const uint64_t& address, std::vector<IslandResource>* resource)
{
	return ExtractResourceNodeChainInfo(address, resource);
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
	uint64_t resource_struct_address = *(uint64_t*)(address + 0x170);
	uint64_t resource_chain_start = **(uint64_t**)(resource_struct_address + 0x40);

	if (!ExtractResourceNodeChainInfo(resource_chain_start, resources))
		return false;

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

			uint16_t current_area_code = 0;
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

			uint16_t current_area_code = 0;
			GetAreaCode(area_address, &current_area_code);

			if (current_area_code == area)
			{
				uint64_t island_list_pointer;
				GetIslandListFromAreaAddress(area_address, &island_list_pointer);

				// Follow the pointer twice to get past the 'dud' pointers???
				island_list_pointer = **(uint64_t**)island_list_pointer;

				ExtractIslandChainFromAddress(island_list_pointer, mustBelongToThePlayer, islands);
				return true;
			}

			return false;
		});

	return true;
}

bool RemoteCallHandlerAnno::GetAllAreas(std::vector<uint32_t>* areas)
{
	constexpr int areas_to_sample = 32;
	int counter = 0;

	std::set<uint16_t> areas_sampled;

	HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
		[&](HookData data) -> bool
		{
			uint64_t area_address = get_area_from_tls();
			uint16_t current_area_code = 0;
			GetAreaCode(area_address, &current_area_code);

			areas_sampled.insert(current_area_code);

			if (++counter > areas_to_sample)
				return true;

			return false;
		});

	for (auto it = areas_sampled.begin(); it != areas_sampled.end(); ++it)
	{
		areas->push_back(*it);
	}

	return true;
}


bool RemoteCallHandlerAnno::DebugGetNameFromGuid(const uint32_t& guid, std::string* name)
{
	bool known;
	*name = GetNameFromGUID(guid, known);
	return true;
}

bool RemoteCallHandlerAnno::DebugGetGuidFromName(const std::string& name, uint32_t* guid)
{
	*guid = (uint32_t)GetGuidFromName(name);
	return true;
}

bool RemoteCallHandlerAnno::GetIslandResources(const uint32_t& areaCode, const uint32_t& islandId, std::vector<IslandResource>* resources)
{
	int counter = 0;
	uint64_t areaAddress = 0;

	HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
		[&](HookData data) -> bool
		{
			uint64_t area_address = get_area_from_tls();
			uint16_t current_area_code = 0;
			GetAreaCode(area_address, &current_area_code);

			if (areaCode == (uint32_t)current_area_code)
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

					uint64_t resource_struct_address = *(uint64_t*)(island.debug_address + 0x170);
					uint64_t resource_chain_start = **(uint64_t**)(resource_struct_address + 0x40);

					if (!ExtractResourceNodeChainInfo(resource_chain_start, resources))
						return false;
				}

				return true;
			}

			if (++counter > 512)
				return true;

			return false;
		});

	return true;
}

bool RemoteCallHandlerAnno::DebugVirtualGetComponentFromAddress(const uint64_t& address, const uint64_t& componentId, uint64_t* componentAddress)
{
	*componentAddress = virtual_get_component(address, componentId);
	return true;
}

bool RemoteCallHandlerAnno::GetIslandResidentialConsumption(const uint32_t& areaCode, const uint32_t& islandId, std::vector<ResourceConsumption>* resources)
{
	int counter = 0;
	uint64_t areaAddress = 0;

	uint64_t target_island_address = 0;

	HookManager::Get().ExecuteInHookSync(HookedFunction::SessionTickHook,
		[&](HookData data) -> bool
		{
			if (++counter > 512)
				return true;

			uint64_t area_address = get_area_from_tls();
			uint16_t current_area_code = 0;
			GetAreaCode(area_address, &current_area_code);

			if (areaCode == (uint32_t)current_area_code)
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

					target_island_address = island.debug_address;
				}

				return true;
			}

			return false;
		});

	if (!target_island_address)
		return false;

	counter = 0;

	HookManager::Get().ExecuteInHookSync(HookedFunction::ConsumptionHook,
		[&](HookData data) -> bool
		{
			if (++counter > 512)
				return true;

			uint64_t island_ptr = *(uint64_t*)(data.rcx + 0x30);

			if (island_ptr == target_island_address)
			{
				uint64_t island_consumption_ptr = *(uint64_t*)(data.rcx + 0x38);

				if (!island_consumption_ptr)
					return true;
				
				uint64_t i = island_consumption_ptr;

				while (true)
				{
					uint32_t resource_type = *(uint32_t*)i;

					// This breaks is the list has an unkown in between some konwns...
					bool known = false;
					std::string resource_name = GetNameFromGUID(resource_type, known);

					if (resource_type == 0)
						break;

					if (!known)
						break;

					ResourceConsumption consumption;
					consumption.name = resource_name;
					consumption.type_id = resource_type;

					float rate_modifier = *(float*)(i + 0xC);

					consumption.rate = 60.0f * rate_modifier;

					resources->push_back(consumption);

					i += 0x38;
				}

				return true;
			}

			return false;
		});

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
			uint16_t current_area_code = 0;
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
			uint16_t current_area_code = 0;
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

				//if (building_type_id == 1010540 || building_type_id == 1010517)
				//	;//ANNO_LOG("trading post %llx", building_address);

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

		bool known_type;
		consumption.name = GetNameFromGUID(conversion.first, known_type);

		if (known_type)
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
			uint16_t current_area_code = 0;
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
			uint16_t current_area_code = 0;
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

					if (!ExtractResourceNodeChainInfo(resource_chain_start, &resources))
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

			uint16_t current_area_code = 0;
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
			uint16_t current_area_id;
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

				ships->push_back(ship_info);

				list_ptr += 0x18;
			}

			return true;
		});

	return true;
}

