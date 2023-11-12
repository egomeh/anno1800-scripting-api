#pragma once

#include "ui.h"

class TimeVieweDebugWindow : public DebugWindow
{
public:
	TimeVieweDebugWindow();

	virtual void Render() override;
	virtual const char* GetName() override;
};


