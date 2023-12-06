#pragma once

#include "ui.h"

class SelectionVieweDebugWindow : public DebugWindow
{
public:
	SelectionVieweDebugWindow();

	virtual void Render() override;
	virtual const char* GetName() override;

private:

	char XCoordinateTextBuffer[128];
	char YCoordinateTextBuffer[128];

	float XCoordinate;
	float YCoordinate;

	uint64_t _tmp;
};


