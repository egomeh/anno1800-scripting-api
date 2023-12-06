#pragma once

#include "ui.h"
#include "hardware_breakpoints.h"

#include <array>

class DebuggerTestDebugWindow : public DebugWindow
{
public:
	DebuggerTestDebugWindow();
	~DebuggerTestDebugWindow();

	virtual void Render() override;
	virtual const char* GetName() override;

	bool VehDebuggingEnabled;
	PVOID HandlerHandle;

	struct BreakpointInfo
	{
		std::string label;
		bool enabled;
		uint64_t address;
		uint32_t size;
		char addressbuffer[64];
		bool write;
		uint64_t actual_address;
		HardwareBreakpoint HardwareBreakpoint;

		void Draw();
	};

	std::array<BreakpointInfo, 4> Breakpoints;

	void StartTrace();
	void StopTrace();

	char FramesToTraceBuffer[1024];
	uint32_t FramesToTrace;
};


