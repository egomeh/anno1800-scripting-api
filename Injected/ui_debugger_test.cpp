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

static DebuggerTestDebugWindow g_AreaVieweDebugWindow;

static bool Tracing = false;
static CRITICAL_SECTION TraceCS;

std::vector<uint32_t> hits;

long WINAPI TestDebugHandler(PEXCEPTION_POINTERS exception)
{
   // if (exception->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
    //{
   //     LeaveCriticalSection(&TraceCS);
   //     return EXCEPTION_CONTINUE_SEARCH;
   // }

    exception->ContextRecord->EFlags &= ~(1 << 8);
    exception->ContextRecord->EFlags |= 0x00010000;

    if (!Tracing)
        return EXCEPTION_CONTINUE_EXECUTION;

    EnterCriticalSection(&TraceCS);

    DWORD CurrentThread = GetCurrentThreadId();

    // Eoah dude, this is hit so many times! :O
    //ANNO_LOG("Hit here %lx", CurrentThread);

    //hits.push_back(CurrentThread);

    ZyanU64 runtime_address = exception->ContextRecord->Rip;

    ZydisDisassembledInstruction instruction;
    ZydisDisassembleIntel(ZYDIS_MACHINE_MODE_LONG_64, runtime_address, (void*)runtime_address, 0x20, &instruction);

    ANNO_LOG("Broke on %llx instruction was %s", runtime_address, instruction.text);

    //exception->ContextRecord->Rip += (uint64_t)instruction.info.length;

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
    if (Tracing)
    {
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

    bool TraceButtonPressed = ImGui::Button("Start Trace");

    for (auto& BP : Breakpoints)
    {
        BP.Draw();
    }

    if (TraceButtonPressed)
    {
        ANNO_LOG("Running trace");
        hits.clear();

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