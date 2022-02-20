// Auto generated code
#include "structs.gen.h"
#include "serialization.gen.h"

bool Serialize(const bool& data, std::vector<uint8_t>& stream)
{
    uint8_t value = (uint8_t)data;
    stream.insert(stream.end(), value);
    return true;
}

bool Deserialize(bool* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (stream.size() < *offset + 1)
        return false;

    uint8_t imm = 0;
    imm = *(uint8_t*)(stream.data() + *offset);
    *offset += 1;
    *data = imm;

    return true;
}


bool Serialize(const uint8_t& data, std::vector<uint8_t>& stream)
{
    for (int i = 0; i < sizeof(uint8_t); ++i)
    {
        stream.insert(stream.end(), *(((uint8_t*)&data) + i));
    }

    return true;
}

bool Deserialize(uint8_t* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (stream.size() < (*offset + sizeof(uint8_t)))
        return false;

    *data = *(uint8_t*)(stream.data() + *offset);
    *offset += sizeof(uint8_t);

    return true;
}


bool Serialize(const int32_t& data, std::vector<uint8_t>& stream)
{
    for (int i = 0; i < sizeof(int32_t); ++i)
    {
        stream.insert(stream.end(), *(((uint8_t*)&data) + i));
    }

    return true;
}

bool Deserialize(int32_t* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (stream.size() < (*offset + sizeof(int32_t)))
        return false;

    *data = *(int32_t*)(stream.data() + *offset);
    *offset += sizeof(int32_t);

    return true;
}


bool Serialize(const uint32_t& data, std::vector<uint8_t>& stream)
{
    for (int i = 0; i < sizeof(uint32_t); ++i)
    {
        stream.insert(stream.end(), *(((uint8_t*)&data) + i));
    }

    return true;
}

bool Deserialize(uint32_t* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (stream.size() < (*offset + sizeof(uint32_t)))
        return false;

    *data = *(uint32_t*)(stream.data() + *offset);
    *offset += sizeof(uint32_t);

    return true;
}


bool Serialize(const int64_t& data, std::vector<uint8_t>& stream)
{
    for (int i = 0; i < sizeof(int64_t); ++i)
    {
        stream.insert(stream.end(), *(((uint8_t*)&data) + i));
    }

    return true;
}

bool Deserialize(int64_t* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (stream.size() < (*offset + sizeof(int64_t)))
        return false;

    *data = *(int64_t*)(stream.data() + *offset);
    *offset += sizeof(int64_t);

    return true;
}


bool Serialize(const uint64_t& data, std::vector<uint8_t>& stream)
{
    for (int i = 0; i < sizeof(uint64_t); ++i)
    {
        stream.insert(stream.end(), *(((uint8_t*)&data) + i));
    }

    return true;
}

bool Deserialize(uint64_t* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (stream.size() < (*offset + sizeof(uint64_t)))
        return false;

    *data = *(uint64_t*)(stream.data() + *offset);
    *offset += sizeof(uint64_t);

    return true;
}


bool Serialize(const float& data, std::vector<uint8_t>& stream)
{
    for (int i = 0; i < sizeof(float); ++i)
    {
        stream.insert(stream.end(), *(((uint8_t*)&data) + i));
    }

    return true;
}

bool Deserialize(float* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (stream.size() < (*offset + sizeof(float)))
        return false;

    *data = *(float*)(stream.data() + *offset);
    *offset += sizeof(float);

    return true;
}


bool Serialize(const double& data, std::vector<uint8_t>& stream)
{
    for (int i = 0; i < sizeof(double); ++i)
    {
        stream.insert(stream.end(), *(((uint8_t*)&data) + i));
    }

    return true;
}

bool Deserialize(double* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (stream.size() < (*offset + sizeof(double)))
        return false;

    *data = *(double*)(stream.data() + *offset);
    *offset += sizeof(double);

    return true;
}


bool Serialize(const std::string& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.size();
    if (!Serialize(size, stream))
        return false;
    for (const char& item : data)
    {
        if (!Serialize((uint8_t)item, stream))
            return false;
    }
    return true;
}

bool Deserialize(std::string* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {
        uint8_t item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back((char)item);
    }

    return true;
}


bool Serialize(const Coordinate& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.x, stream))
        return false;

    if (!Serialize(data.y, stream))
        return false;

    return true;
}

bool Deserialize(Coordinate* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->x, stream, offset))
        return false;

    if (!Deserialize(&data->y, stream, offset))
        return false;

    return true;
}


bool Serialize(const CargoSlot& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.itemID, stream))
        return false;

    if (!Serialize(data.volume, stream))
        return false;

    return true;
}

bool Deserialize(CargoSlot* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->itemID, stream, offset))
        return false;

    if (!Deserialize(&data->volume, stream, offset))
        return false;

    return true;
}


bool Serialize(const std::vector<uint64_t>& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (const uint64_t& item : data)
    {
        if (!Serialize(item, stream))
            return false;
    }

    return true;
}

bool Deserialize(std::vector<uint64_t>* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {
        uint64_t item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back(item);
    }

    return true;
}


bool Serialize(const SomeListyBoi& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.ids, stream))
        return false;

    return true;
}

bool Deserialize(SomeListyBoi* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->ids, stream, offset))
        return false;

    return true;
}


bool Serialize(const Ship& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.name, stream))
        return false;

    return true;
}

bool Deserialize(Ship* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->name, stream, offset))
        return false;

    return true;
}


bool Serialize(const std::vector<CargoSlot>& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (const CargoSlot& item : data)
    {
        if (!Serialize(item, stream))
            return false;
    }

    return true;
}

bool Deserialize(std::vector<CargoSlot>* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {
        CargoSlot item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back(item);
    }

    return true;
}


bool Serialize(const ShipCargo& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.cargo, stream))
        return false;

    return true;
}

bool Deserialize(ShipCargo* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->cargo, stream, offset))
        return false;

    return true;
}


bool Serialize(const std::vector<Coordinate>& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (const Coordinate& item : data)
    {
        if (!Serialize(item, stream))
            return false;
    }

    return true;
}

bool Deserialize(std::vector<Coordinate>* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {
        Coordinate item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back(item);
    }

    return true;
}


bool Serialize(const std::vector<std::string>& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (const std::string& item : data)
    {
        if (!Serialize(item, stream))
            return false;
    }

    return true;
}

bool Deserialize(std::vector<std::string>* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {
        std::string item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back(item);
    }

    return true;
}

