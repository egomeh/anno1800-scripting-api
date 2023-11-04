#include "ui_id_to_name.h"

#include <sstream>
#include <iostream>

#include "ui.h"
#include "log.h"
#include "anno_tools.h"
#include "anno_native.h"


static IdToNameDebugWindow g_IdToNameDebugWindow;

IdToNameDebugWindow::IdToNameDebugWindow() : DebugWindow()
{
}

void IdToNameDebugWindow::Render()
{
    bool do_update = false;
    
    if (ImGui::Checkbox("hex", &IsHex))
        do_update = true;

    ImGui::Text("In-Game ID: ");
    ImGui::SameLine();

    static char address_test_buffer[64] = "";
    if (ImGui::InputText(" ", address_test_buffer, 64))
        do_update = true;

    if (do_update)
    {
        uint64_t id;
        std::stringstream ss;

        if (IsHex)
            ss << std::hex;

        ss << address_test_buffer;
        ss >> id;

        result = GetNameFromGUID(g_ModuleBase, g_BinaryCRC, id);
    }

    ImGui::Text(result.c_str());
}

const char* IdToNameDebugWindow::GetName()
{
    return "Id to Name";
}

