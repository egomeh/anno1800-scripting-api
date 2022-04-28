#pragma once

#include <string>

bool ReadAnnoString(uint64_t address, std::string& result);

std::string GetNameFromGUID(uint64_t guid, bool& known);
uint64_t GetGuidFromName(std::string name);
