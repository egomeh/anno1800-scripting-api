#include "pch.h"
#include "autocomms.h"
#include <vector>

namespace AutoComms
{{

bool Serialize(const bool& data, std::vector<uint8_t>& stream)
{{
    uint8_t value = data;
    stream.insert(stream.end(), value);
    return true;
}}

bool Deserialize(bool *data, const std::vector<uint8_t>& stream, size_t* offset)
{{
    if (stream.size() < *offset + 1)
        return false;

    uint8_t imm = 0;
    imm = *(uint8_t*)(stream.data() + *offset);
    *offset += 1;
    *data = imm;

    return true;
}}

bool Serialize(const uint8_t& data, std::vector<uint8_t>& stream)
{{
    stream.insert(stream.end(), data);
    return true;
}}

bool Deserialize(uint8_t *data, const std::vector<uint8_t>& stream, size_t* offset)
{{
    if (stream.size() < *offset + 1)
        return false;

    *data = *(uint8_t*)(stream.data() + *offset);
    *offset += 1;

    return true;
}}


bool Serialize(const uint64_t& data, std::vector<uint8_t>& stream)
{{
    uint8_t b1 = ((uint8_t*)&data)[0];
    uint8_t b2 = ((uint8_t*)&data)[1];
    uint8_t b3 = ((uint8_t*)&data)[2];
    uint8_t b4 = ((uint8_t*)&data)[3];
    uint8_t b5 = ((uint8_t*)&data)[4];
    uint8_t b6 = ((uint8_t*)&data)[5];
    uint8_t b7 = ((uint8_t*)&data)[6];
    uint8_t b8 = ((uint8_t*)&data)[7];
    stream.insert(stream.end(), {{ b1, b2, b3, b4, b5, b6, b7, b8 }});
    return true;
}}

bool Deserialize(uint64_t* data, const std::vector<uint8_t>& stream, size_t* offset)
{{
    if (stream.size() < *offset + 8)
        return false;

    *data = *(uint64_t*)(stream.data() + *offset);
    *offset += 8;

    return true;
}}

bool Serialize(const float& data, std::vector<uint8_t>& stream)
{{
    uint8_t b1 = ((uint8_t*)&data)[0];
    uint8_t b2 = ((uint8_t*)&data)[1];
    uint8_t b3 = ((uint8_t*)&data)[2];
    uint8_t b4 = ((uint8_t*)&data)[3];
    stream.insert(stream.end(), {{ b1, b2, b3, b4 }});
    return true;
}}

bool Deserialize(float* data, const std::vector<uint8_t>& stream, size_t* offset)
{{
    if (stream.size() < *offset + 4)
        return false;

    *data = *(float*)(stream.data() + *offset);
    *offset += 4;

    return true;
}}

bool Serialize(const double& data, std::vector<uint8_t>& stream)
{{
    uint8_t b1 = ((uint8_t*)&data)[0];
    uint8_t b2 = ((uint8_t*)&data)[1];
    uint8_t b3 = ((uint8_t*)&data)[2];
    uint8_t b4 = ((uint8_t*)&data)[3];
    uint8_t b5 = ((uint8_t*)&data)[4];
    uint8_t b6 = ((uint8_t*)&data)[5];
    uint8_t b7 = ((uint8_t*)&data)[6];
    uint8_t b8 = ((uint8_t*)&data)[7];
    stream.insert(stream.end(), {{ b1, b2, b3, b4, b5, b6, b7, b8 }});
    return true;
}}

bool Deserialize(double* data, const std::vector<uint8_t>& stream, size_t* offset)
{{
    if (stream.size() < *offset + 8)
        return false;

    *data = *(float*)(stream.data() + *offset);
    *offset += 8;

    return true;
}}

bool Serialize(const std::string& data, std::vector<uint8_t>& stream)
{{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (const char& item : data)
    {{
        if (!Serialize((uint8_t)item, stream))
            return false;
    }}

    return true;
}}

bool Deserialize(std::string* data, const std::vector<uint8_t>& stream, size_t* offset)
{{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {{
        uint8_t item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back((char)item);
    }}

    return true;
}}


{serialization_impl}

}}
