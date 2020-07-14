#pragma once
#include <inttypes.h>
#include <vector>
#include <string>

namespace AutoComms
{{

{struct_defs}

bool Serialize(const bool& data, std::vector<uint8_t>& stream);
bool Deserialize(bool *data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const uint8_t& data, std::vector<uint8_t>& stream);
bool Deserialize(uint8_t *data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const uint64_t& data, std::vector<uint8_t>& stream);
bool Deserialize(uint64_t *data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const float& data, std::vector<uint8_t>& stream);
bool Deserialize(float* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const double& data, std::vector<uint8_t>& stream);
bool Deserialize(double* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const double& data, std::vector<uint8_t>& stream);
bool Deserialize(double* data, const std::vector<uint8_t>& stream, size_t* offset);

bool Serialize(const std::string& data, std::vector<uint8_t>& stream);
bool Deserialize(std::string* data, const std::vector<uint8_t>& stream, size_t* offset);

{serialization_defs}

template<typename T>
bool Serialize(const std::vector<T>& data, std::vector<uint8_t> &stream)
{{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (const T& item : data)
    {{
        if (!Serialize(item, stream))
            return false;
    }}

    return true;
}}


template<typename T>
bool Deserialize(std::vector<T>* data, const std::vector<uint8_t>& stream, size_t* offset)
{{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {{
        T item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back(item);
    }}

    return true;
}}

}}
