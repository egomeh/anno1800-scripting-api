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


bool Serialize(const IslandResource& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.type_id, stream))
        return false;

    if (!Serialize(data.name, stream))
        return false;

    if (!Serialize(data.amount, stream))
        return false;

    if (!Serialize(data.capacity, stream))
        return false;

    if (!Serialize(data.amount_ptr, stream))
        return false;

    return true;
}

bool Deserialize(IslandResource* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->type_id, stream, offset))
        return false;

    if (!Deserialize(&data->name, stream, offset))
        return false;

    if (!Deserialize(&data->amount, stream, offset))
        return false;

    if (!Deserialize(&data->capacity, stream, offset))
        return false;

    if (!Deserialize(&data->amount_ptr, stream, offset))
        return false;

    return true;
}


bool Serialize(const ResourceConsumption& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.type_id, stream))
        return false;

    if (!Serialize(data.name, stream))
        return false;

    if (!Serialize(data.rate, stream))
        return false;

    return true;
}

bool Deserialize(ResourceConsumption* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->type_id, stream, offset))
        return false;

    if (!Deserialize(&data->name, stream, offset))
        return false;

    if (!Deserialize(&data->rate, stream, offset))
        return false;

    return true;
}


bool Serialize(const IslandInfo& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.island_id, stream))
        return false;

    if (!Serialize(data.name, stream))
        return false;

    if (!Serialize(data.debug_address, stream))
        return false;

    return true;
}

bool Deserialize(IslandInfo* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->island_id, stream, offset))
        return false;

    if (!Deserialize(&data->name, stream, offset))
        return false;

    if (!Deserialize(&data->debug_address, stream, offset))
        return false;

    return true;
}


bool Serialize(const Vector& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.x, stream))
        return false;

    if (!Serialize(data.y, stream))
        return false;

    return true;
}

bool Deserialize(Vector* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->x, stream, offset))
        return false;

    if (!Deserialize(&data->y, stream, offset))
        return false;

    return true;
}


bool Serialize(const CargoSlot& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.occupied, stream))
        return false;

    if (!Serialize(data.type_name, stream))
        return false;

    if (!Serialize(data.type_id, stream))
        return false;

    if (!Serialize(data.amount, stream))
        return false;

    return true;
}

bool Deserialize(CargoSlot* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->occupied, stream, offset))
        return false;

    if (!Deserialize(&data->type_name, stream, offset))
        return false;

    if (!Deserialize(&data->type_id, stream, offset))
        return false;

    if (!Deserialize(&data->amount, stream, offset))
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


bool Serialize(const ShipInfo& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.id, stream))
        return false;

    if (!Serialize(data.debug_address, stream))
        return false;

    if (!Serialize(data.name, stream))
        return false;

    if (!Serialize(data.idle, stream))
        return false;

    if (!Serialize(data.position, stream))
        return false;

    if (!Serialize(data.rotation, stream))
        return false;

    if (!Serialize(data.cargo, stream))
        return false;

    return true;
}

bool Deserialize(ShipInfo* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->id, stream, offset))
        return false;

    if (!Deserialize(&data->debug_address, stream, offset))
        return false;

    if (!Deserialize(&data->name, stream, offset))
        return false;

    if (!Deserialize(&data->idle, stream, offset))
        return false;

    if (!Deserialize(&data->position, stream, offset))
        return false;

    if (!Deserialize(&data->rotation, stream, offset))
        return false;

    if (!Deserialize(&data->cargo, stream, offset))
        return false;

    return true;
}


bool Serialize(const DebugComponent& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.id, stream))
        return false;

    if (!Serialize(data.address, stream))
        return false;

    return true;
}

bool Deserialize(DebugComponent* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->id, stream, offset))
        return false;

    if (!Deserialize(&data->address, stream, offset))
        return false;

    return true;
}


bool Serialize(const std::vector<IslandInfo>& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (const IslandInfo& item : data)
    {
        if (!Serialize(item, stream))
            return false;
    }

    return true;
}

bool Deserialize(std::vector<IslandInfo>* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {
        IslandInfo item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back(item);
    }

    return true;
}


bool Serialize(const std::vector<uint32_t>& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (const uint32_t& item : data)
    {
        if (!Serialize(item, stream))
            return false;
    }

    return true;
}

bool Deserialize(std::vector<uint32_t>* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {
        uint32_t item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back(item);
    }

    return true;
}


bool Serialize(const std::vector<IslandResource>& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (const IslandResource& item : data)
    {
        if (!Serialize(item, stream))
            return false;
    }

    return true;
}

bool Deserialize(std::vector<IslandResource>* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {
        IslandResource item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back(item);
    }

    return true;
}


bool Serialize(const std::vector<ResourceConsumption>& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (const ResourceConsumption& item : data)
    {
        if (!Serialize(item, stream))
            return false;
    }

    return true;
}

bool Deserialize(std::vector<ResourceConsumption>* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {
        ResourceConsumption item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back(item);
    }

    return true;
}


bool Serialize(const std::vector<ShipInfo>& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (const ShipInfo& item : data)
    {
        if (!Serialize(item, stream))
            return false;
    }

    return true;
}

bool Deserialize(std::vector<ShipInfo>* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {
        ShipInfo item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back(item);
    }

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


bool Serialize(const std::vector<DebugComponent>& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (const DebugComponent& item : data)
    {
        if (!Serialize(item, stream))
            return false;
    }

    return true;
}

bool Deserialize(std::vector<DebugComponent>* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    data->clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (size_t i = 0; i < size; ++i)
    {
        DebugComponent item;
        if (!Deserialize(&item, stream, offset))
            return false;

        data->push_back(item);
    }

    return true;
}

