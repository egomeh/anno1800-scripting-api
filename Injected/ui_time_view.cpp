#include "ui_time_view.h"

#include <sstream>
#include <iostream>
#include <chrono>

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

	std::chrono::milliseconds ms(Time_ms);
	auto secs = std::chrono::duration_cast<std::chrono::seconds>(ms);
	ms -= std::chrono::duration_cast<std::chrono::milliseconds>(secs);
	auto mins = std::chrono::duration_cast<std::chrono::minutes>(secs);
	secs -= std::chrono::duration_cast<std::chrono::seconds>(mins);
	auto hour = std::chrono::duration_cast<std::chrono::hours>(mins);
	mins -= std::chrono::duration_cast<std::chrono::minutes>(hour);

	ImGui::Text("%llu hours %llu minutes %llu seconds", hour.count(), mins.count(), secs.count());
}

const char* TimeVieweDebugWindow::GetName()
{
    return "Time";
}

