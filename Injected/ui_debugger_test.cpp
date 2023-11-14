#include "ui_debugger_test.h"

#include <sstream>
#include <iostream>
#include <sstream>

#include "Windows.h"

#include "ui.h"
#include "log.h"
#include "anno_tools.h"
#include "anno_native.h"

#include "Zydis.h"

// static DebuggerTestDebugWindow g_AreaVieweDebugWindow;

static uint64_t frame_count_at_request = 0;
static bool Tracing = false;
static CRITICAL_SECTION TraceCS;

std::vector<std::string> hits;

long WINAPI TestDebugHandler(PEXCEPTION_POINTERS exception)
{
    // Rest trap flag
    exception->ContextRecord->EFlags &= ~(1 << 8);

    // Set Continue flag
    exception->ContextRecord->EFlags |= 0x00010000;

    if (!Tracing)
        return EXCEPTION_CONTINUE_EXECUTION;

    EnterCriticalSection(&TraceCS);

    uint64_t TimestructOffset = AnnoDataOffset(g_BinaryCRC, DataOffset::TimeStructOffset);
    uint64_t ActualOffset = TimestructOffset + g_ModuleBase;

    uint64_t TimestructBaseAddress = *(uint64_t*)(ActualOffset);
    uint64_t FrameNumber = *(uint64_t*)(TimestructBaseAddress + 0x70);

    if (FrameNumber != (frame_count_at_request + 1))
    {
        LeaveCriticalSection(&TraceCS);
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    DWORD CurrentThread = GetCurrentThreadId();

    ZyanU64 runtime_address = exception->ContextRecord->Rip;

    ZydisDisassembledInstruction instruction;
    ZydisDisassembleIntel(ZYDIS_MACHINE_MODE_LONG_64, runtime_address, (void*)runtime_address, 0x20, &instruction);

    ANNO_FORMAT(entry, "Thread %lx broke on %llx instruction was %s frame # %llu", CurrentThread, runtime_address, instruction.text, FrameNumber);
    hits.push_back(entry);

    // Unset trap flag

    LeaveCriticalSection(&TraceCS);

    return EXCEPTION_CONTINUE_EXECUTION;
}

DebuggerTestDebugWindow::DebuggerTestDebugWindow() : DebugWindow()
{
    int i = 0;
    for (auto& BP : Breakpoints)
    {
        ++i;
        ANNO_FORMAT(bp_label, "Breakpoint #%d", i);
        BP.label = bp_label;
    }

    InitializeCriticalSectionAndSpinCount(&TraceCS, 0x1000);
}

DebuggerTestDebugWindow::~DebuggerTestDebugWindow()
{
    DeleteCriticalSection(&TraceCS);
}

void DebuggerTestDebugWindow::Render()
{
    bool TraceButtonPressed = ImGui::Button("Start Trace");

    for (auto& BP : Breakpoints)
    {
        BP.Draw();
    }

    for (std::string& hit : hits)
    {
        ImGui::Text(hit.c_str());
    }

    if (TraceButtonPressed)
    {
        StartTrace();
    }
    
    uint64_t TimestructOffset = AnnoDataOffset(g_BinaryCRC, DataOffset::TimeStructOffset);
    uint64_t ActualOffset = TimestructOffset + g_ModuleBase;

    uint64_t TimestructBaseAddress = *(uint64_t*)(ActualOffset);
    uint64_t FrameNumber = *(uint64_t*)(TimestructBaseAddress + 0x70);

    if (FrameNumber > frame_count_at_request + 1)
        StopTrace();
}

const char* DebuggerTestDebugWindow::GetName()
{
    return "Debugger test";
}

void DebuggerTestDebugWindow::StartTrace()
{
    ANNO_LOG("Running trace");
    hits.clear();
    hits.reserve(0x1000);

    uint64_t TimestructOffset = AnnoDataOffset(g_BinaryCRC, DataOffset::TimeStructOffset);
    uint64_t ActualOffset = TimestructOffset + g_ModuleBase;

    uint64_t TimestructBaseAddress = *(uint64_t*)(ActualOffset);
    uint64_t FrameNumber = *(uint64_t*)(TimestructBaseAddress + 0x70);

    frame_count_at_request = FrameNumber;
    Tracing = true;

    HandlerHandle = AddVectoredExceptionHandler(TRUE, &TestDebugHandler);

    for (auto& BP : Breakpoints)
    {
        if (!BP.enabled)
            continue;

        ANNO_LOG("Enabling breakpoint for address %llx", BP.actual_address);
        BP.HardwareBreakpoint.Set((void*)BP.actual_address, 1, HardwareBreakpoint::Condition::Execute);
    }
}

void DebuggerTestDebugWindow::StopTrace()
{
    if (!Tracing)
        return;

    ANNO_LOG("Stopping trace");
    for (auto& BP : Breakpoints)
    {
        BP.HardwareBreakpoint.Clear();
    }

    Sleep(500);

    RemoveVectoredExceptionHandler(HandlerHandle);

    Tracing = false;
    return;
}

void DebuggerTestDebugWindow::BreakpointInfo::Draw()
{
    ANNO_FORMAT(enable_label, "check %llx", (uint64_t)this);
    ImGui::PushID(enable_label.c_str());
    ImGui::Checkbox(" ", &enabled);
    ImGui::SameLine();

    ImGui::Text(label.c_str());
    ImGui::SameLine();
    ImGui::PushItemWidth(150);
    ANNO_FORMAT(id_input, "buffer %llx", (uint64_t)this);
    ImGui::PushID(id_input.c_str());
    if (ImGui::InputText(" ", addressbuffer, sizeof(addressbuffer), ImGuiInputTextFlags_CharsHexadecimal))
    {
        std::stringstream ss;
        ss << std::hex << addressbuffer;
        ss >> actual_address;
    }
    ImGui::PopID();
    ImGui::PopItemWidth();

    ImGui::SameLine();
    ImGui::Text("On Write");
    ImGui::SameLine();
    ANNO_FORMAT(check_label, "check %llx", (uint64_t)this);
    ImGui::PushID(check_label.c_str());
    ImGui::Checkbox(" ", &write);
    ImGui::PopID();
}
