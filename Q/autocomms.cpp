#include "pch.h"
#include "autocomms.h"
#include <vector>

namespace AutoComms
{

bool Serialize(const bool& data, std::vector<uint8_t>& stream)
{
    uint8_t value = data;
    stream.insert(stream.end(), value);
    return true;
}

bool Deserialize(bool *data, const std::vector<uint8_t>& stream, size_t* offset)
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
    stream.insert(stream.end(), data);
    return true;
}

bool Deserialize(uint8_t *data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (stream.size() < *offset + 1)
        return false;

    *data = *(uint8_t*)(stream.data() + *offset);
    *offset += 1;

    return true;
}


bool Serialize(const uint64_t& data, std::vector<uint8_t>& stream)
{
    uint8_t b1 = ((uint8_t*)&data)[0];
    uint8_t b2 = ((uint8_t*)&data)[1];
    uint8_t b3 = ((uint8_t*)&data)[2];
    uint8_t b4 = ((uint8_t*)&data)[3];
    uint8_t b5 = ((uint8_t*)&data)[4];
    uint8_t b6 = ((uint8_t*)&data)[5];
    uint8_t b7 = ((uint8_t*)&data)[6];
    uint8_t b8 = ((uint8_t*)&data)[7];
    stream.insert(stream.end(), { b1, b2, b3, b4, b5, b6, b7, b8 });
    return true;
}

bool Deserialize(uint64_t* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (stream.size() < *offset + 8)
        return false;

    *data = *(uint64_t*)(stream.data() + *offset);
    *offset += 8;

    return true;
}

bool Serialize(const float& data, std::vector<uint8_t>& stream)
{
    uint8_t b1 = ((uint8_t*)&data)[0];
    uint8_t b2 = ((uint8_t*)&data)[1];
    uint8_t b3 = ((uint8_t*)&data)[2];
    uint8_t b4 = ((uint8_t*)&data)[3];
    stream.insert(stream.end(), { b1, b2, b3, b4 });
    return true;
}

bool Deserialize(float* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (stream.size() < *offset + 4)
        return false;

    *data = *(float*)(stream.data() + *offset);
    *offset += 4;

    return true;
}

bool Serialize(const double& data, std::vector<uint8_t>& stream)
{
    uint8_t b1 = ((uint8_t*)&data)[0];
    uint8_t b2 = ((uint8_t*)&data)[1];
    uint8_t b3 = ((uint8_t*)&data)[2];
    uint8_t b4 = ((uint8_t*)&data)[3];
    uint8_t b5 = ((uint8_t*)&data)[4];
    uint8_t b6 = ((uint8_t*)&data)[5];
    uint8_t b7 = ((uint8_t*)&data)[6];
    uint8_t b8 = ((uint8_t*)&data)[7];
    stream.insert(stream.end(), { b1, b2, b3, b4, b5, b6, b7, b8 });
    return true;
}

bool Deserialize(double* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (stream.size() < *offset + 8)
        return false;

    *data = *(float*)(stream.data() + *offset);
    *offset += 8;

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


bool Serialize(const Area& data, std::vector<uint8_t>& stream)
{
    return Serialize((const uint64_t)data, stream);
}

bool Deserialize(Area *data, const std::vector<uint8_t>& stream, size_t* offset)
{
    return Deserialize((uint64_t*)data, stream, offset);
}

bool Serialize(const ShipType& data, std::vector<uint8_t>& stream)
{
    return Serialize((const uint64_t)data, stream);
}

bool Deserialize(ShipType *data, const std::vector<uint8_t>& stream, size_t* offset)
{
    return Deserialize((uint64_t*)data, stream, offset);
}

bool Serialize(const Resource& data, std::vector<uint8_t>& stream)
{
    return Serialize((const uint64_t)data, stream);
}

bool Deserialize(Resource *data, const std::vector<uint8_t>& stream, size_t* offset)
{
    return Deserialize((uint64_t*)data, stream, offset);
}

bool Serialize(const Building& data, std::vector<uint8_t>& stream)
{
    return Serialize((const uint64_t)data, stream);
}

bool Deserialize(Building *data, const std::vector<uint8_t>& stream, size_t* offset)
{
    return Deserialize((uint64_t*)data, stream, offset);
}

bool Serialize(const GameTime& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.time, stream))
        return false;

    if (!Serialize(data.frame, stream))
        return false;

    return true;
}

bool Deserialize(GameTime* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->time, stream, offset))
        return false;

    if (!Deserialize(&data->frame, stream, offset))
        return false;

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

bool Serialize(const ShipMoveData& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.moving, stream))
        return false;

    if (!Serialize(data.position, stream))
        return false;

    if (!Serialize(data.waypoints, stream))
        return false;

    return true;
}

bool Deserialize(ShipMoveData* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->moving, stream, offset))
        return false;

    if (!Deserialize(&data->position, stream, offset))
        return false;

    if (!Deserialize(&data->waypoints, stream, offset))
        return false;

    return true;
}

bool Serialize(const ShipData& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.shipId, stream))
        return false;

    if (!Serialize(data.shipType, stream))
        return false;

    if (!Serialize(data.name, stream))
        return false;

    return true;
}

bool Deserialize(ShipData* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->shipId, stream, offset))
        return false;

    if (!Deserialize(&data->shipType, stream, offset))
        return false;

    if (!Deserialize(&data->name, stream, offset))
        return false;

    return true;
}

bool Serialize(const ShipCargoSlot& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.hasCargo, stream))
        return false;

    if (!Serialize(data.amount, stream))
        return false;

    if (!Serialize(data.resourceType, stream))
        return false;

    return true;
}

bool Deserialize(ShipCargoSlot* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->hasCargo, stream, offset))
        return false;

    if (!Deserialize(&data->amount, stream, offset))
        return false;

    if (!Deserialize(&data->resourceType, stream, offset))
        return false;

    return true;
}

bool Serialize(const IslandResourceRecord& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.resourceType, stream))
        return false;

    if (!Serialize(data.capacity, stream))
        return false;

    if (!Serialize(data.amount, stream))
        return false;

    return true;
}

bool Deserialize(IslandResourceRecord* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->resourceType, stream, offset))
        return false;

    if (!Deserialize(&data->capacity, stream, offset))
        return false;

    if (!Deserialize(&data->amount, stream, offset))
        return false;

    return true;
}

bool Serialize(const IslandData& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.id, stream))
        return false;

    if (!Serialize(data.name, stream))
        return false;

    return true;
}

bool Deserialize(IslandData* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->id, stream, offset))
        return false;

    if (!Deserialize(&data->name, stream, offset))
        return false;

    return true;
}

bool Serialize(const BuildingData& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.id, stream))
        return false;

    if (!Serialize(data.buidlingType, stream))
        return false;

    if (!Serialize(data.rawBuildingTypeID, stream))
        return false;

    if (!Serialize(data.position, stream))
        return false;

    if (!Serialize(data.debug_address, stream))
        return false;

    return true;
}

bool Deserialize(BuildingData* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->id, stream, offset))
        return false;

    if (!Deserialize(&data->buidlingType, stream, offset))
        return false;

    if (!Deserialize(&data->rawBuildingTypeID, stream, offset))
        return false;

    if (!Deserialize(&data->position, stream, offset))
        return false;

    if (!Deserialize(&data->debug_address, stream, offset))
        return false;

    return true;
}

bool Serialize(const ProductionNode& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.input, stream))
        return false;

    if (!Serialize(data.output, stream))
        return false;

    if (!Serialize(data.rate, stream))
        return false;

    return true;
}

bool Deserialize(ProductionNode* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->input, stream, offset))
        return false;

    if (!Deserialize(&data->output, stream, offset))
        return false;

    if (!Deserialize(&data->rate, stream, offset))
        return false;

    return true;
}

bool Serialize(const ConsumptionNode& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.resourceType, stream))
        return false;

    if (!Serialize(data.rate, stream))
        return false;

    return true;
}

bool Deserialize(ConsumptionNode* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->resourceType, stream, offset))
        return false;

    if (!Deserialize(&data->rate, stream, offset))
        return false;

    return true;
}

bool Serialize(const TradeNode& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.island, stream))
        return false;

    return true;
}

bool Deserialize(TradeNode* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->island, stream, offset))
        return false;

    return true;
}

bool Serialize(const TradeRoute& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.name, stream))
        return false;

    if (!Serialize(data.nodes, stream))
        return false;

    return true;
}

bool Deserialize(TradeRoute* data, const std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->name, stream, offset))
        return false;

    if (!Deserialize(&data->nodes, stream, offset))
        return false;

    return true;
}

}
