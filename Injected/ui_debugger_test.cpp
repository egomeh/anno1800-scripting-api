#include "ui_debugger_test.h"

#include <sstream>
#include <iostream>
#include <sstream>
#include <atomic>
#include <algorithm>

#include "Windows.h"

#include "ui.h"
#include "log.h"
#include "anno_tools.h"
#include "anno_native.h"

#include "Zydis.h"

extern "C"
{
    uint64_t get_area_from_tls();
}

static DebuggerTestDebugWindow g_DebuggerVieweDebugWindow;

static uint64_t frame_count_at_request = 0;
static bool Tracing = false;
static bool Collect_CriticalSections = false;
static CRITICAL_SECTION TraceCS;

std::vector<std::string> hits;
std::vector<uint64_t> CriticalSections_list;

class SpinLock {
    std::atomic_flag locked = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (locked.test_and_set(std::memory_order_acquire)) { ; }
    }
    void unlock() {
        locked.clear(std::memory_order_release);
    }
};

SpinLock HitLock;

long WINAPI TestDebugHandler(PEXCEPTION_POINTERS exception)
{
    // Rest trap flag
    exception->ContextRecord->EFlags &= ~(1 << 8);

    // Set Continue flag
    exception->ContextRecord->EFlags |= 0x00010000;

    if (!Tracing)
        return EXCEPTION_CONTINUE_EXECUTION;

    uint64_t TimestructOffset = AnnoDataOffset(g_BinaryCRC, DataOffset::TimeStructOffset);
    uint64_t ActualOffset = TimestructOffset + g_ModuleBase;

    uint64_t TimestructBaseAddress = *(uint64_t*)(ActualOffset);
    uint64_t FrameNumber = *(uint64_t*)(TimestructBaseAddress + 0x70);

    if ((FrameNumber <= frame_count_at_request) || (FrameNumber > (frame_count_at_request + g_DebuggerVieweDebugWindow.FramesToTrace)))
    {
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    DWORD CurrentThread = GetCurrentThreadId();
    HANDLE CurrentThreadHandle = GetCurrentThread();

    ZyanU64 runtime_address = exception->ContextRecord->Rip;

    ZydisDisassembledInstruction instruction;
    ZydisDisassembleIntel(ZYDIS_MACHINE_MODE_LONG_64, runtime_address, (void*)runtime_address, 0x20, &instruction);

    uint64_t area_in_tls = get_area_from_tls();

    std::string area_name = "nullptr";

    uint64_t area_base_address = 0;

    if (area_in_tls)
    {
        area_base_address = *(uint64_t*)(area_in_tls);

        if (area_base_address)
        {
            uint32_t area_code = *(uint32_t*)(area_base_address + 0x8);
            area_name = GetNameFromGUID(g_ModuleBase, g_BinaryCRC, area_code);
        }
    }

    std::string breakpoint_name = "unkonwn breakpoint";

    if (runtime_address == g_DebuggerVieweDebugWindow.Breakpoints[0].actual_address)
        breakpoint_name = "Breakpoint 1";
    if (runtime_address == g_DebuggerVieweDebugWindow.Breakpoints[1].actual_address)
        breakpoint_name = "Breakpoint 2";
    if (runtime_address == g_DebuggerVieweDebugWindow.Breakpoints[2].actual_address)
        breakpoint_name = "Breakpoint 3";
    if (runtime_address == g_DebuggerVieweDebugWindow.Breakpoints[3].actual_address)
        breakpoint_name = "Breakpoint 4";

    uint64_t argument1 = exception->ContextRecord->Rcx;

    // ANNO_FORMAT(entry, "Thread %lx broke on %llx instruction was %s frame # %llu area is %llx %llx %s", CurrentThread, runtime_address, instruction.text, FrameNumber, area_in_tls, area_base_address, area_name.c_str());
    ANNO_FORMAT(entry, "Thread %lx broke on %s frame is %llu area %s argument 1 %llx", CurrentThread, breakpoint_name.c_str(), FrameNumber, area_name.c_str(), argument1);

    for (uint64_t Critical_section : CriticalSections_list)
    {
        LPCRITICAL_SECTION pCS = (LPCRITICAL_SECTION)Critical_section;
        if (pCS->OwningThread == CurrentThreadHandle)
        {
            ANNO_LOG("Was an owning thread");
        }
    }

    HitLock.lock();
    if (Collect_CriticalSections)
    {
        if (std::find(CriticalSections_list.begin(), CriticalSections_list.end(), argument1) == CriticalSections_list.end())
            CriticalSections_list.push_back(argument1);
    }
    else
    {
        hits.push_back(entry);
    }
    HitLock.unlock();

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

    ImGui::SameLine();

    bool CollectButtonPressed = ImGui::Button("Collect Critical Sections");

    ImGui::SameLine();

    ImGui::PushItemWidth(25);
    if (ImGui::InputText("Frames", FramesToTraceBuffer, 1024, ImGuiInputTextFlags_CharsDecimal))
    {
        std::stringstream ss;
        ss << std::dec << FramesToTraceBuffer;
        ss >> FramesToTrace;
    }
    ImGui::PopItemWidth();

    ImGui::Text("Currently collected %llu critical sections", (uint64_t)CriticalSections_list.size());

    for (auto& BP : Breakpoints)
    {
        BP.Draw();
    }

    for (std::string& hit : hits)
    {
        char buffer[1024];
        sprintf_s(buffer, hit.c_str());
        ImGui::PushID((uint64_t)hit.c_str());
        ImGui::PushItemWidth(ImGui::GetWindowSize().x);
        ImGui::InputText("", buffer, 1024, ImGuiInputTextFlags_ReadOnly);
        ImGui::PopItemWidth();
        ImGui::PopID();
    }

    if (TraceButtonPressed)
    {
        StartTrace();
    }

    if (CollectButtonPressed)
    {
        Collect_CriticalSections = true;
        StartTrace();
    }
    
    uint64_t TimestructOffset = AnnoDataOffset(g_BinaryCRC, DataOffset::TimeStructOffset);
    uint64_t ActualOffset = TimestructOffset + g_ModuleBase;

    uint64_t TimestructBaseAddress = *(uint64_t*)(ActualOffset);
    uint64_t FrameNumber = *(uint64_t*)(TimestructBaseAddress + 0x70);

    // Maybe critical section this??
    if (FrameNumber > frame_count_at_request + FramesToTrace)
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
    CriticalSections_list.clear();

    uint64_t TimestructOffset = AnnoDataOffset(g_BinaryCRC, DataOffset::TimeStructOffset);
    uint64_t ActualOffset = TimestructOffset + g_ModuleBase;

    uint64_t TimestructBaseAddress = *(uint64_t*)(ActualOffset);
    uint64_t FrameNumber = *(uint64_t*)(TimestructBaseAddress + 0x70);

    frame_count_at_request = FrameNumber;
    Tracing = true;

    HandlerHandle = AddVectoredExceptionHandler(TRUE, &TestDebugHandler);

    if (Collect_CriticalSections)
    {
        uint64_t TryEnterCriticalSectionAddress = (uint64_t)&TryEnterCriticalSection;

        ANNO_LOG("Setting breakpoint on TryEnterCriticalSection at location %llx", TryEnterCriticalSectionAddress);
        Breakpoints[0].HardwareBreakpoint.Set((void*)TryEnterCriticalSectionAddress, 1, HardwareBreakpoint::Condition::Execute);
    }
    else
    {
        for (auto& BP : Breakpoints)
        {
            if (!BP.enabled)
                continue;

            ANNO_LOG("Enabling breakpoint for address %llx", BP.actual_address);
            BP.HardwareBreakpoint.Set((void*)BP.actual_address, 1, HardwareBreakpoint::Condition::Execute);
        }
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
    Collect_CriticalSections = false;
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
