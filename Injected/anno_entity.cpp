#include "anno_entity.h"
#include "anno_native.h"
#include "anno_tools.h"

#include "memory.h"
#include "hook.h"
#include "log.h"
#include <set>
#include <algorithm>
#include <cmath>
#include <numeric>

AnnoEntity::AnnoEntity(uint64_t _address)
{
	address = _address;
	TypeId = *(uint32_t*)(address + 8);
	TypeName = GetNameFromGUID(g_ModuleBase, g_BinaryCRC, TypeId);

	int position_offset = 0x40;
	x = *(float*)(address + position_offset);
	y = *(float*)(address + position_offset + 2 * sizeof(float));
	rotation = *(float*)(address + position_offset + 3 * sizeof(float));

	for (uint32_t id = 0; id < 2048; ++id)
	{
		auto Component = GetComponentFromEntity(*this, id);

		if (Component)
			Components.push_back(Component);
	}

}

AnnoEntity::~AnnoEntity()
{
}

uint32_t AnnoEntity::GetTypeId()
{
	return TypeId;
}

float AnnoEntity::GetX()
{
	return x;
}

float AnnoEntity::GetY()
{
	return y;
}

float AnnoEntity::GetRotation()
{
	return rotation;
}

std::vector<std::shared_ptr<AnnoComponent>> AnnoEntity::GetComponents()
{
	return Components;
}

uint64_t AnnoEntity::GetAddress()
{
	return address;
}

std::string AnnoEntity::GetTypeName()
{
	return TypeName;
}

AnnoComponent::AnnoComponent(uint32_t _id, uint64_t _address)
{
	id = _id;
	address = _address;
}

AnnoComponent::~AnnoComponent()
{
}

uint64_t AnnoComponent::GetAddress()
{
	return address;
}

void AnnoComponent::Render()
{
	ImGui::Text("No data");
}

uint32_t AnnoComponent::GetId()
{
	return id;
}

std::string AnnoComponent::GetName()
{
	return "Unknown Component";
}

std::shared_ptr<AnnoComponent> GetComponentFromEntity(AnnoEntity& Entity, uint32_t ComponentId)
{
	uint64_t component_address = get_anno_component(Entity.GetAddress(), (uint64_t)ComponentId);

	if (!component_address)
		return nullptr;

	if (ComponentId == 0x158)
		return std::make_shared<AnnoComponent_Name>(ComponentId, component_address);
	else if (ComponentId == 0x30a)
		return std::make_shared<AnnoComponent_InternalStorage>(ComponentId, component_address);
	else if (ComponentId == 0x301)
		return std::make_shared<AnnoComponent_Production>(ComponentId, component_address);
	else if (ComponentId == 0x141)
		return std::make_shared<AnnoComponent_Movement>(ComponentId, component_address);

	return std::make_shared<AnnoComponent>(ComponentId, component_address);
}

AnnoComponent_Name::AnnoComponent_Name(uint32_t id, uint64_t _address) : AnnoComponent(id, _address)
{
	ReadAnnoString(_address + 0x10, NameOfComponent);
}

AnnoComponent_Name::~AnnoComponent_Name()
{
}

void AnnoComponent_Name::Render()
{
	ImGui::Text("Name %s", NameOfComponent.c_str());
}

std::string AnnoComponent_Name::GetName()
{
	return "Editable Name";
}

AnnoComponent_InternalStorage::AnnoComponent_InternalStorage(uint32_t id, uint64_t _address) : AnnoComponent(id, _address)
{
	uint32_t n_storage_slots = *(uint32_t*)(address + 0x30);

	if (!n_storage_slots)
		return;

	uint64_t ptr_to_delimiters = *(uint64_t*)(address + 0x28);
	
	if (!ptr_to_delimiters)
		return;

	uint64_t item_ptr = *(uint64_t*)(ptr_to_delimiters + 0);
	uint64_t last_item = *(uint64_t*)(ptr_to_delimiters + 8);

	while (true)
	{
		StorageItem Item;

		uint32_t TypeId = *(uint32_t*)(item_ptr + 6 * sizeof(uint32_t));
		uint32_t Capacity = *(uint32_t*)(item_ptr + 7 * sizeof(uint32_t));
		uint32_t Amount = *(uint32_t*)(item_ptr + 8 * sizeof(uint32_t));
			
		Item.TypeName = GetNameFromGUID(g_ModuleBase, g_BinaryCRC, TypeId);

		Item.Capacity = Capacity;
		Item.Amount = Amount;

		StorageItems.push_back(Item);

		if (item_ptr == last_item)
			break;

		item_ptr = *(uint64_t*)(item_ptr);
	}
}

AnnoComponent_InternalStorage::~AnnoComponent_InternalStorage()
{
}

void AnnoComponent_InternalStorage::Render()
{
	for (StorageItem& Item : StorageItems)
	{
		ImGui::ProgressBar((float)Item.Amount / (float)Item.Capacity, ImVec2(0.0f, 0.0f));
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("%d/%d %s", Item.Amount, Item.Capacity, Item.TypeName.c_str());
	}
}

std::string AnnoComponent_InternalStorage::GetName()
{
	return "Internal Storage";
}

AnnoComponent_Production::AnnoComponent_Production(uint32_t id, uint64_t _address) : AnnoComponent(id, _address)
{
	ProductionOutputId = *(uint32_t*)(address + 0x168);
	ProductionOutputName = GetNameFromGUID(g_ModuleBase, g_BinaryCRC, ProductionOutputId);

	Efficiency = *(float*)(address + 0x13C);
	cycle_base_time_ms = *(uint32_t*)(address + 0x1F8);
	time_left_in_cycle = *(float*)(address + 0x134);
	cycle_time_multiplier = *(float*)(address + 0x1DC);
	cycle_count = *(uint32_t*)(address + 0x140);

	uint64_t SecondaryProduction_ptr = *(uint64_t*)(address + 0x188);
	uint64_t SecondaryProduction_end = *(uint64_t*)(address + 0x190);

	if (!SecondaryProduction_ptr)
		return;

	while (true)
	{
		uint32_t Type = *(uint32_t*)(SecondaryProduction_ptr);
		float Rate = *(float*)(SecondaryProduction_ptr + 4);

		SecondaryProduction Production;
		Production.type = Type;
		Production.name = GetNameFromGUID(g_ModuleBase, g_BinaryCRC, Type);

		for (int i = 1; i < 100; i++)
		{
			float previous_error= abs(1.0f / (float)(max(1, i - 1)));
			float error = abs(1.0f / (float)(max(1, i)) - Rate);
			float next_error = abs(1.0f / (float)(max(1, i + 1)) - Rate);

			Production.nthCycle = i;

			if (previous_error >= error && error <= next_error)
				break;
		}

		SecondaryProductions.push_back(Production);

		SecondaryProduction_ptr += 8;

		if (SecondaryProduction_ptr == SecondaryProduction_end)
			break;
	}
}

AnnoComponent_Production::~AnnoComponent_Production()
{
}

void AnnoComponent_Production::Render()
{
	float full_cylce_time = (float)(cycle_base_time_ms);

	if (full_cylce_time == 0)
		return;

	float progress = ((full_cylce_time - time_left_in_cycle) / full_cylce_time);
	ImGui::ProgressBar(progress, ImVec2(250.0f, 0.0f));
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("%lx %s at %d%% efficiency", ProductionOutputId, ProductionOutputName.c_str(), (int)(100 * Efficiency));
	ImGui::Text("Base cycle time: %d seconds", cycle_base_time_ms / 1'000);
	ImGui::Text("Time left in cycle: %.2f seconds", (time_left_in_cycle / 1'000.0f) * cycle_time_multiplier);

	for (SecondaryProduction P : SecondaryProductions)
	{
		float extended_cycle_time = (float)(cycle_base_time_ms) * P.nthCycle;

		if (extended_cycle_time == 0)
			continue;

		int full_cycles_left = P.nthCycle - ((cycle_count + 1) % P.nthCycle);
		ImGui::Text("cycles left %d", full_cycles_left);
		float progress = ((extended_cycle_time - (time_left_in_cycle + (full_cycles_left - 1) * full_cylce_time)) / extended_cycle_time);
		ImGui::ProgressBar(progress, ImVec2(250.0f, 0.0f));
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("%lx %s every %d cycle", P.type, P.name.c_str(), P.nthCycle);
	}
}

std::string AnnoComponent_Production::GetName()
{
	return "Production";
}

AnnoComponent_Movement::AnnoComponent_Movement(uint32_t id, uint64_t _address) : AnnoComponent(id, _address)
{
	x = *(float*)(address + 0x2C);
	y = *(float*)(address + 0x30);
	rotation = *(float*)(address + 0x34);

	uint32_t nWaypoints = *(uint32_t*)(address + 0x20);
	uint64_t waypoint_ptr = *(uint64_t*)(address + 0x10);

	uint64_t waypoint_sruct_ptr = *(uint64_t*)(waypoint_ptr);
	uint64_t waypoint_sruct_end = *(uint64_t*)(waypoint_ptr + 8);

	while (waypoint_sruct_ptr != waypoint_sruct_end)
	{
		WayPoint wp;
			
		//ANNO_LOG("waypoint_ptr %llx", waypoint_sruct_ptr);
		wp.x = *(float*)(waypoint_sruct_ptr + sizeof(float));
		wp.y = *(float*)(waypoint_sruct_ptr + 2 * sizeof(float));
		Waypoints.push_back(wp);

		waypoint_sruct_ptr += 0x10;
	}
}

AnnoComponent_Movement::~AnnoComponent_Movement()
{
}

void AnnoComponent_Movement::Render()
{
	ImGui::Text("Position = (%.0f, %.0f)", x, y);
	ImGui::Text("rotation = %.2f", rotation);

	for (const WayPoint wp : Waypoints)
	{
		ImGui::Text("Waypoint %.1f, %.1f", wp.x, wp.y);
	}
}

std::string AnnoComponent_Movement::GetName()
{
	return "Movement";
}
