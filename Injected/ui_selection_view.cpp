#include "ui_selection_view.h"

#include <sstream>
#include <iostream>

#include "ui.h"
#include "log.h"
#include "anno_tools.h"
#include "anno_native.h"

#include "anno_entity.h"

#include "ui_entity_view.h"

// static SelectionVieweDebugWindow g_AreaVieweDebugWindow;

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
        }
    }

    for (AnnoEntity Item : Selection)
    {
        DisplayAnnoEntity(Item);
    }
}

const char* SelectionVieweDebugWindow::GetName()
{
    return "Selection";
}

