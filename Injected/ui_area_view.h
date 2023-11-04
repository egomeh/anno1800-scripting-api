#pragma once

#include "ui.h"

class AreaVieweDebugWindow : public DebugWindow
{
public:
	AreaVieweDebugWindow();

	virtual void Render() override;
	virtual const char* GetName() override;
};


