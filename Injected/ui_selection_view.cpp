#include "ui_selection_view.h"

#include <sstream>
#include <iostream>

#include "ui.h"
#include "log.h"
#include "anno_tools.h"
#include "anno_native.h"

#include "anno_entity.h"

#include "ui_entity_view.h"

extern "C"
{
    uint64_t get_area_from_tls();
    uint64_t align_stack_32();
    void add_rsp(uint64_t);
}

static SelectionVieweDebugWindow g_AreaVieweDebugWindow;

SelectionVieweDebugWindow::SelectionVieweDebugWindow() : DebugWindow()
{
}

void SelectionVieweDebugWindow::Render()
{
    std::vector<uint64_t> ids;
    std::vector<uint64_t> addresses;
    GetAllAreas(g_ModuleBase, g_BinaryCRC, ids, addresses);

    struct SelectionItem
    {
        std::string name;
    };

    std::vector<AnnoEntity> Selection;
    uint64_t area_with_selection = 0;

    for (uint64_t& address : addresses)
    {
        uint64_t selection_address = *(uint64_t*)(address + 0x3B0);

        if (!selection_address)
            continue;

        uint64_t selection_list_begin = *(uint64_t*)(selection_address + 0xB8);
        uint64_t selection_list_end = *(uint64_t*)(selection_address + 0xC0);

        for (uint64_t i = selection_list_begin; i < selection_list_end; i += 0x10)
        {
            uint64_t item_ptr = *(uint64_t*)(i + 8);
            uint32_t item_id = *(uint32_t*)(item_ptr + 8);

            Selection.emplace_back(item_ptr);
            area_with_selection = address;
        }
    }

    for (AnnoEntity Item : Selection)
    {
        DisplayAnnoEntity(Item);
    }

    ImGui::PushItemWidth(100);
    ImGui::PushID(0x43e7b87d);
    if (ImGui::InputText("", XCoordinateTextBuffer, sizeof(XCoordinateTextBuffer), ImGuiInputTextFlags_CharsDecimal))
    {
        std::stringstream ss;
        ss << XCoordinateTextBuffer;
        ss >> XCoordinate;
    }
    ImGui::PopID();
    ImGui::PopItemWidth();

    ImGui::SameLine();

    ImGui::PushItemWidth(100);
    ImGui::PushID(0xa0b3e902);
    if (ImGui::InputText("", YCoordinateTextBuffer, sizeof(YCoordinateTextBuffer), ImGuiInputTextFlags_CharsDecimal))
    {
        std::stringstream ss;
        ss << YCoordinateTextBuffer;
        ss >> YCoordinate;
    }
    ImGui::PopID();
    ImGui::PopItemWidth();

    ImGui::SameLine();

    if (ImGui::Button("Move Selected Ships"))
    {
        ANNO_LOG("Moving ships to location %f, %f", XCoordinate, YCoordinate);

        std::vector<uint64_t> ShipIds;

        for (AnnoEntity& Entity : Selection)
        {
            uint64_t address = Entity.GetAddress();
            uint64_t id = *(uint64_t*)(address + 0x28);
            ShipIds.push_back(id);
        }

        HookManager::Get().ExecuteInHookAsync(HookedFunction::SessionTickHook, [this, area_with_selection, ShipIds](HookData data)
            {
                uint64_t area = get_area_from_tls();
                _tmp = *(uint64_t*)area;

                if (_tmp != area_with_selection)
                    return false;

                uint64_t StartOfShipIdData = (uint64_t)&ShipIds[0];

                struct _ShipIdDelimiterPtr
                {
                    uint64_t IdsBegin;
                    uint64_t IdsEnd;
                } ShipIdDelimiterPtr { StartOfShipIdData, StartOfShipIdData + ShipIds.size() * sizeof(uint64_t)};

                struct _Coordinate
                {
                    float x, y;
                } Coordinate = { XCoordinate, YCoordinate };

                uint64_t MoveShipsFunctionAddress = g_ModuleBase + AnnoDataOffset(g_BinaryCRC, DataOffset::FunctionMoveObjects);
                auto MoveShipsFunction = reinterpret_cast<void(*)(uint64_t, _ShipIdDelimiterPtr, _Coordinate*)>(MoveShipsFunctionAddress);

                MoveShipsFunction(0, ShipIdDelimiterPtr, &Coordinate);

                return true;
            });
    }

    ImGui::Text("_tmp = %llx", _tmp);
    ImGui::SameLine();
    if (ImGui::Button("Copy to clipboard"))
    {
        ImGuiIO& io = ImGui::GetIO();

        uint64_t ComponnetAddress = _tmp;
        ANNO_FORMAT(tmp_string, "%llx", ComponnetAddress);

        io.SetClipboardTextFn(nullptr, tmp_string.c_str());
    }
}

const char* SelectionVieweDebugWindow::GetName()
{
    return "Selection";
}

