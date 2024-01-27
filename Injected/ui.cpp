#include "ui.h"

#include <cstdint>
#include <windows.h>
#include <array>

#include "hook.h"
#include "memory.h"
#include "log.h"

// To get the address of present
#pragma comment (lib, "d3d11.lib")

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

typedef LRESULT(*WndProcPointer)(HWND, UINT, WPARAM, LPARAM);
typedef HRESULT(WINAPI* PresentHookFn)(uint64_t thisptr, UINT SyncInterval, UINT Flags);

namespace
{
    WndProcPointer OriginalProcPointerAnno;
    PresentHookFn OriginalPresentFunctionDx11;
}

#define VMT_PRESENT (UINT)8

const D3D_DRIVER_TYPE DRIVER_TYPE_LIST[4] = { D3D_DRIVER_TYPE_REFERENCE, D3D_DRIVER_TYPE_SOFTWARE, D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP };
#define SAFE_RELEASE(p) if (p) { p->Release(); p = nullptr; }

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT OverriddenWindProc(HWND WindowHandle, UINT uMsg, WPARAM WParam, LPARAM LParam)
{
    if (UI::Get().OnWinProc(WindowHandle, uMsg, WParam, LParam))
        return true;

    return OriginalProcPointerAnno(WindowHandle, uMsg, WParam, LParam);
}

HRESULT PresentHook(uint64_t thisptr, UINT SyncInterval, UINT Flags)
{
    UI::Get().OnPresentHook(thisptr, SyncInterval, Flags);
    return OriginalPresentFunctionDx11(thisptr, SyncInterval, Flags);
}

UI& UI::Get()
{
    static UI ui;
    return ui;
}

void UI::RegsiterDebugWindow(DebugWindow* Window)
{
    DebugWindowInfo NewInfo;
    NewInfo.Enabled = false;
    NewInfo.Window = Window;
    DebugWindows.push_back(NewInfo);
}

void UI::UnregisterDebugWindow(DebugWindow* Window)
{
    for (auto it = DebugWindows.begin(); it != DebugWindows.end(); ++it)
    {
        if (it->Window != Window)
            continue;

        DebugWindows.erase(it);
        break;
    }
}

void UI::EnableHook()
{
    hwnd = FindWindowA(NULL, "Anno 1800");
    std::array<TCHAR, 256> RegisteredClassNameBuffer;
    ::GetClassName(hwnd, RegisteredClassNameBuffer.data(), (int)RegisteredClassNameBuffer.size());
    std::wstring RegisteredClassNameW = RegisteredClassNameBuffer.data();
    std::string RegisteredClassNameA = utf8_encode(RegisteredClassNameW);

    WNDCLASSEX wce;
    GetClassInfoEx(GetModuleHandle(nullptr), RegisteredClassNameW.c_str(), &wce);

    uint64_t WinProcFunction = (uint64_t)wce.lpfnWndProc;

    uint64_t PresentFunctionVTableEntry = 0;
    ID3D11Device* pDevice = nullptr;
    IDXGISwapChain* pSwapchain = nullptr;

    // Create a dummy device, get swapchain vmt, hook present.
    D3D_FEATURE_LEVEL featLevel;
    DXGI_SWAP_CHAIN_DESC sd{ 0 };
    sd.BufferCount = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.Height = 800;
    sd.BufferDesc.Width = 600;
    sd.BufferDesc.RefreshRate = { 60, 1 };
    sd.OutputWindow = GetForegroundWindow();
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &pSwapchain, &pDevice, &featLevel, nullptr);

    if (hr == 0 && pSwapchain != nullptr)
    {
        void** pVMT = *(void***)pSwapchain;

        OriginalPresentFunctionDx11 = (PresentHookFn)pVMT[VMT_PRESENT];
        PresentFunctionVTableEntry = (uint64_t)pVMT + 8 * sizeof(uint64_t);

        SAFE_RELEASE(pSwapchain);
        SAFE_RELEASE(pDevice);
    }
    else
    {
        if (hr == 0)
            ANNO_LOG("Unable to create D3D device");

        if (!pSwapchain)
            ANNO_LOG("Unable to create swapchain");
    }

    ANNO_LOG("Present Function VTable Address: %llx", (uint64_t)PresentFunctionVTableEntry);
    ANNO_LOG("Present Function Address: %llx", (uint64_t)OriginalPresentFunctionDx11);

    OriginalProcPointerAnno = (WndProcPointer)WinProcFunction;

    if (hwnd)
    {
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)OverriddenWindProc);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(hwnd);

        HookedWinProc = true;
    }

    ANNO_LOG("Address to place hook %llx", (uint64_t)PresentFunctionVTableEntry);
    ANNO_LOG("Hook target %llx", (uint64_t)&PresentHook);
    ANNO_LOG("original function %llx", (uint64_t)&OriginalPresentFunctionDx11);
    PresentHookReplacement.SetMemory
    ({
        EIGHT_BYTES((uint64_t)&PresentHook)
    });
    PresentHookReplacement.Emplace((void*)(PresentFunctionVTableEntry));

    ShutdownEvent = CreateEventA(NULL, FALSE, FALSE, "ui_shutdown_event");
    ResetEvent(ShutdownEvent);
}

void UI::DisableHook()
{
    PresentHookReplacement.Undo();

    if (HookedDX11)
    {
        ImGui_ImplDX11_Shutdown();
    }

    if (HookedWinProc)
    {
        ImGui_ImplWin32_Shutdown();
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)OriginalProcPointerAnno);
    }

    ImGui::DestroyContext();
}

void UI::OnPresentHook(uint64_t thisptr, UINT SyncInterval, UINT Flags)
{
    if (!HookedDX11)
    {
        IDXGISwapChain* swapchian = (IDXGISwapChain*)(thisptr);

        HRESULT hr = swapchian->GetDevice(__uuidof(ID3D11Device), (void**)&DX11Device);
        DX11Device->GetImmediateContext(&DX11Context);
        ImGui_ImplDX11_Init(DX11Device, DX11Context);

        HookedDX11 = true;
    }

    if (HookedDX11)
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        this->Render();

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
}

bool UI::OnWinProc(HWND WindowHandle, UINT uMsg, WPARAM WParam, LPARAM LParam)
{
    // If device is null, we're probably not running dx11 and we won't do anything
    if (HookedWinProc)
    {
        if (ImGui_ImplWin32_WndProcHandler(WindowHandle, uMsg, WParam, LParam))
            return true;

        if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard)
            return true;
    }
    return false;
}

void UI::Render()
{
    bool bWindowOpen = true;
    ImGui::Begin("Anno Scriptig API debug menu", &bWindowOpen);

    ImGui::Text("Filter: ");
    ImGui::SameLine();

    static char filter_buffer[64] = "";
    ImGui::InputText(" ", filter_buffer, 64);
    for (auto& DebugWindow : DebugWindows)
    {
        if (strlen(filter_buffer) > 0)
        {
            if (!strstr(DebugWindow.Window->GetName(), filter_buffer))
                continue;
        }

        ImGui::PushID((void*)&DebugWindow);
        ImGui::Checkbox("", &DebugWindow.Enabled);
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::Text(DebugWindow.Window->GetName());
    }

    ImGui::End(); // Main window

    for (auto& DebugWindow : DebugWindows)
    {
        if (!DebugWindow.Enabled)
            continue;

        ImGui::Begin(DebugWindow.Window->GetName(), &DebugWindow.Enabled);
        DebugWindow.Window->Render();
        ImGui::End();
    }

    if (!bWindowOpen)
    {
        SetEvent(ShutdownEvent);
    }
}

DebugWindow::DebugWindow()
{
    UI::Get().RegsiterDebugWindow(this);
}

DebugWindow::~DebugWindow()
{
    UI::Get().UnregisterDebugWindow(this);
}

