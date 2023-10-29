#pragma once

#include "memory.h"

#include <d3d11.h>

class UI
{
public:

	void EnableHook();
	void DisableHook();

	void OnPresentHook(uint64_t thisptr, UINT SyncInterval, UINT Flags);
	bool OnWinProc(HWND WindowHandle, UINT uMsg, WPARAM WParam, LPARAM LParam);

	void Render();

	static UI& Get();

private:

	bool HookedWinProc = false;
	bool HookedDX11 = false;

	ID3D11Device* DX11Device = nullptr;
	ID3D11DeviceContext* DX11Context = nullptr;

	HWND hwnd;

	MemoryReplacement PresentHookReplacement;
};

