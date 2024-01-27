#pragma once

#include "memory.h"

#include "imgui.h"
#include <d3d11.h>

class DebugWindow
{
public:
	DebugWindow();
	virtual ~DebugWindow();

	virtual void Render() = 0;
	virtual const char* GetName() = 0;
};

class UI
{
public:

	void EnableHook();
	void DisableHook();

	void OnPresentHook(uint64_t thisptr, UINT SyncInterval, UINT Flags);
	bool OnWinProc(HWND WindowHandle, UINT uMsg, WPARAM WParam, LPARAM LParam);

	void WriteConfig();
	void ReadConfig();

	void Render();

	static UI& Get();

	void RegsiterDebugWindow(DebugWindow* Window);
	void UnregisterDebugWindow(DebugWindow* Window);

	HANDLE ShutdownEvent;

private:

	struct DebugWindowInfo
	{
		bool Enabled;
		DebugWindow* Window;
	};

	std::vector<DebugWindowInfo> DebugWindows;

	bool HookedWinProc = false;
	bool HookedDX11 = false;

	ID3D11Device* DX11Device = nullptr;
	ID3D11DeviceContext* DX11Context = nullptr;

	HWND hwnd;

	MemoryReplacement PresentHookReplacement;
};

