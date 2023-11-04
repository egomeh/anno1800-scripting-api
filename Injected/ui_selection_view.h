#pragma once

#include "ui.h"

class SelectionVieweDebugWindow : public DebugWindow
{
public:
	SelectionVieweDebugWindow();

	virtual void Render() override;
	virtual const char* GetName() override;
};


