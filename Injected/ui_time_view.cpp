#include "ui_time_view.h"

#include <sstream>
#include <iostream>

#include "ui.h"
#include "log.h"
#include "anno_tools.h"
#include "anno_native.h"


static TimeVieweDebugWindow g_AreaVieweDebugWindow;

TimeVieweDebugWindow::TimeVieweDebugWindow() : DebugWindow()
{
}

void TimeVieweDebugWindow::Render()
{
    uint64_t TimestructOffset = AnnoDataOffset(g_BinaryCRC, DataOffset::TimeStructOffset);
    uint64_t ActualOffset = TimestructOffset + g_ModuleBase;

    uint64_t TimestructBaseAddress = *(uint64_t*)(ActualOffset);

    ImGui::Text("Struct Location: %llx", TimestructBaseAddress);
	ImGui::SameLine();
	if (ImGui::Button("Copy to clipboard"))
	{
		ImGuiIO& io = ImGui::GetIO();
		ANNO_FORMAT(string_address, "%llx", TimestructBaseAddress);
		io.SetClipboardTextFn(nullptr, string_address.c_str());
	}

	uint64_t Time_ms = *(uint64_t*)(TimestructBaseAddress + 0x60);
	uint64_t FrameNumber = *(uint64_t*)(TimestructBaseAddress + 0x70);
	ImGui::Text("Frame Nr. %llu", FrameNumber);
}

const char* TimeVieweDebugWindow::GetName()
{
    return "Time";
}

