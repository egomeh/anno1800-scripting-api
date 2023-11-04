#include "ui_island_view.h"

#include <sstream>
#include <iostream>

#include "ui.h"
#include "log.h"
#include "anno_tools.h"
#include "anno_native.h"


static IslandViewDebugWindow g_IslandViewDebugWindow;

IslandViewDebugWindow::IslandViewDebugWindow() : DebugWindow()
{
}

void IslandViewDebugWindow::Render()
{
    static char address_test_buffer[64] = "";
    ImGui::InputText("Island Address", address_test_buffer, 64, ImGuiInputTextFlags_CharsHexadecimal);

    static int clicked = 0;
    if (ImGui::Button("Fetch Data"))
    {
        ANNO_LOG("%s", address_test_buffer);

        uint64_t address;
        std::stringstream ss;
        ss << std::hex << address_test_buffer;
        ss >> address;

        FetchIslandData(address);
    }

    if (island_id != 0)
    {
        ImGui::Text("island id: %x", island_id);
    }

    if (island_name != "")
    {
        ImGui::Text("island name : %s", island_name.c_str());
    }
}

const char* IslandViewDebugWindow::GetName()
{
    return "Island View";
}

void IslandViewDebugWindow::FetchIslandData(uint64_t island_address)
{
    island_id = *(uint16_t*)(island_address + 0x4a);
    ReadAnnoString(island_address + 0x120, island_name);
    ANNO_LOG("string addr: %llx", island_address + 0x120);
}
