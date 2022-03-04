// Auto generated code
#include "structs.gen.h"

bool Serialize(const bool& data, std::vector<uint8_t>& stream);
bool Deserialize(bool* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const uint8_t& data, std::vector<uint8_t>& stream);
bool Deserialize(uint8_t* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const int32_t& data, std::vector<uint8_t>& stream);
bool Deserialize(int32_t* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const uint32_t& data, std::vector<uint8_t>& stream);
bool Deserialize(uint32_t* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const int64_t& data, std::vector<uint8_t>& stream);
bool Deserialize(int64_t* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const uint64_t& data, std::vector<uint8_t>& stream);
bool Deserialize(uint64_t* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const float& data, std::vector<uint8_t>& stream);
bool Deserialize(float* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const double& data, std::vector<uint8_t>& stream);
bool Deserialize(double* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const std::string& data, std::vector<uint8_t>& stream);
bool Deserialize(std::string* data, const std::vector<uint8_t>& stream, size_t* offset);
