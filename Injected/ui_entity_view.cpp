#include "ui_entity_view.h"

#include <sstream>
#include <iostream>

#include "ui.h"
#include "log.h"
#include "anno_tools.h"
#include "anno_native.h"
#include "anno_entity.h"

void DisplayAnnoEntity(AnnoEntity& Entity)
{
	ImGui::Text("Type: %lx %s", Entity.GetTypeId(), Entity.GetTypeName().c_str());

	ImGui::PushID((void*)Entity.GetAddress());
	ImGui::Text("Entity Address: %llx", Entity.GetAddress());
	ImGui::SameLine();
	if (ImGui::Button("Copy to clipboard"))
	{
		ImGuiIO& io = ImGui::GetIO();

		uint64_t EntityAddress = Entity.GetAddress();
		ANNO_FORMAT(address_string, "%llx", EntityAddress);
		// ANNO_LOG("%s", EntityAddress.c_str());

		io.SetClipboardTextFn(nullptr, address_string.c_str());
	}
	ImGui::PopID();

	ImGui::Text("Position: %f, %f", Entity.GetX(), Entity.GetY());
	ImGui::Text("Rotation: %f", Entity.GetRotation());

	auto Components = Entity.GetComponents();

	ANNO_FORMAT(components_tree_id, "Components %llx", Entity.GetAddress())

	if (ImGui::TreeNode(components_tree_id.c_str(), "Components"))
	{
		for (auto& Component : Components)
		{
			ANNO_FORMAT(component_tree_id, "Components %llx", Component->GetAddress());
			// ANNO_LOG(component_tree_id.c_str());

			if (ImGui::TreeNode(component_tree_id.c_str(), "%lx %s", Component->GetId(), Component->GetName().c_str()))
			{
				ImGui::Text("Address: %llx", Component->GetAddress());
				ImGui::SameLine();
				if (ImGui::Button("Copy to clipboard"))
				{
					ImGuiIO& io = ImGui::GetIO();

					uint64_t ComponnetAddress = Component->GetAddress();
					ANNO_FORMAT(component_address_string, "%llx", ComponnetAddress);
					// ANNO_LOG("%s", EntityAddress.c_str());

					io.SetClipboardTextFn(nullptr, component_address_string.c_str());
				}

				Component->Render();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
}
