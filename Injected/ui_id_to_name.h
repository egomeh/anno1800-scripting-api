#pragma once

#include "ui.h"

class IdToNameDebugWindow : public DebugWindow
{
public:
	IdToNameDebugWindow();

	virtual void Render() override;
	virtual const char* GetName() override;

private:

	bool IsHex;
	std::string result;
};


