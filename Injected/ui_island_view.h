#pragma once

#include "ui.h"

class IslandViewDebugWindow : public DebugWindow
{
public:
	IslandViewDebugWindow();

	virtual void Render() override;
	virtual const char* GetName() override;

private:

	void FetchIslandData(uint64_t island_address);

	uint16_t island_id = 0;
	std::string island_name = "";
};


