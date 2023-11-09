#include "ui_debugger_test.h"

#include <sstream>
#include <iostream>
#include <sstream>

#include "Windows.h"

#include "ui.h"
#include "log.h"
#include "anno_tools.h"
#include "anno_native.h"

static DebuggerTestDebugWindow g_AreaVieweDebugWindow;

static bool Tracing = false;
static CRITICAL_SECTION TraceCS;

std::vector<uint32_t> hits;

long WINAPI TestDebugHandler(PEXCEPTION_POINTERS exception)
{
    EnterCriticalSection(&TraceCS);

    DWORD CurrentThread = GetCurrentThreadId();

    // Eoah dude, this is hit so many times! :O
    ANNO_LOG("Hit here %lx", CurrentThread);

    hits.push_back(CurrentThread);

    LeaveCriticalSection(&TraceCS);

    return EXCEPTION_CONTINUE_EXECUTION;
}

DebuggerTestDebugWindow::DebuggerTestDebugWindow() : DebugWindow()
{
    TraceStartBP.label = "Trace Start";

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
    if (!Tracing)
    {
        bool TraceButtonPressed = ImGui::Button("Start Trace");

        TraceStartBP.Draw();

        for (auto& BP : Breakpoints)
        {
            BP.Draw();
        }

        if (TraceButtonPressed)
        {
            hits.clear();

            Tracing = true;

            HandlerHandle = AddVectoredExceptionHandler(1, &TestDebugHandler);
            TraceStartBP.HardwareBreakpoint.Set((void*)TraceStartBP.actual_address, 1, HardwareBreakpoint::Condition::Execute);
        }
    }
    else
    {
        bool StopTraceButtonPressed = ImGui::Button("Stop Trace");

        if (StopTraceButtonPressed)
        {
            Tracing = false;

            TraceStartBP.HardwareBreakpoint.Clear();
            RemoveVectoredContinueHandler(HandlerHandle);
        }
    }

    //EnterCriticalSection(&TraceCS);
    //for (const uint32_t& thread_id : hits)
    //{
    //    ImGui::Text("%lx", thread_id);
    //}
    //LeaveCriticalSection(&TraceCS);
}

const char* DebuggerTestDebugWindow::GetName()
{
    return "Debugger test";
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
