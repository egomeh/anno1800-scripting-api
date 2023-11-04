#include "ui_area_view.h"

#include <sstream>
#include <iostream>

#include "ui.h"
#include "log.h"
#include "anno_tools.h"
#include "anno_native.h"


static AreaVieweDebugWindow g_AreaVieweDebugWindow;

AreaVieweDebugWindow::AreaVieweDebugWindow() : DebugWindow()
{
}

void AreaVieweDebugWindow::Render()
{
    std::vector<uint64_t> ids;
    std::vector<uint64_t> addresses;
    GetAllAreas(g_ModuleBase, g_BinaryCRC, ids, addresses);

    for (uint64_t& address : addresses)
    {
        std::string result = GetNameFromGUID(g_ModuleBase, g_BinaryCRC, *(uint32_t*)(address + 8));

        ImGui::PushID(address);
        if (ImGui::Button("Copy address"))
        {
            ImGuiIO& io = ImGui::GetIO();

            ANNO_FORMAT(address_string, "%llx", address);
            ANNO_LOG("%s", address_string.c_str());

            io.SetClipboardTextFn(nullptr, address_string.c_str());
        }
        ImGui::PopID();
        ImGui::SameLine();

        ImGui::Text("%llx %s", address, result.c_str());
    }
}

const char* AreaVieweDebugWindow::GetName()
{
    return "Areas";
}

