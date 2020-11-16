#include "pch.h"
#include "comms.h"


bool Serialize(uint64_t data, std::vector<uint8_t>& stream)
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

bool Serialize(float data, std::vector<uint8_t>& stream)
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

bool Serialize(const ShipWaypointSetInputData& data, std::vector<uint8_t>& stream)
{
    // Write id list

    // First write uint64_t with n elements
    uint64_t size = data.shipIds.size();

    if (!Serialize(size, stream))
        return false;

    for (uint64_t id : data.shipIds)
    {
        if (!Serialize(id, stream))
            return false;
    }

    if (!Serialize(data.x, stream))
        return false;

    if (!Serialize(data.y, stream))
        return false;

    return true;
}

bool Deserialize(ShipWaypointSetInputData* data, std::vector<uint8_t>& stream, size_t* offset)
{
    uint64_t nIds = 0;

    data->shipIds.clear();

    if (!Deserialize(&nIds, stream, offset))
        return false;

    for (int i = 0; i < nIds; ++i)
    {
        uint64_t nextId = 0;
        if (!Deserialize(&nextId, stream, offset))
            return false;

        data->shipIds.push_back(nextId);
    }

    if (!Deserialize(&data->x, stream, offset))
        return false;

    if (!Deserialize(&data->y, stream, offset))
        return false;

    return true;
}

bool Serialize(const std::string& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (char c : data)
        stream.push_back(c);

    return true;
}

bool Deserialize(std::string* data, std::vector<uint8_t>& stream, size_t* offset)
{
    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    data->clear();
    for (uint64_t i = 0; i < size; ++i)
    {
        data->push_back(stream[*offset + i]);
    }

    *offset += size;

    return true;
}

bool Serialize(const std::vector<uint64_t>& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.size();

    if (!Serialize(size, stream))
        return false;

    for (size_t i = 0; i < data.size(); ++i)
    {
        if (!Serialize(data[i], stream))
            return false;
    }

    return true;
}

bool Deserialize(std::vector<uint64_t>* data, std::vector<uint8_t>& stream, size_t* offset)
{
    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    data->clear();
    for (uint64_t i = 0; i < size; ++i)
    {
        uint64_t id;
        if (!Deserialize(&id, stream, offset))
            return false;

        data->push_back(id);
    }

    return true;
}

bool Serialize(const IslandResourceRecordRequst& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.islandId, stream))
        return false;

    if (!Serialize(data.resourceType, stream))
        return false;

    return true;
}

bool Deserialize(IslandResourceRecordRequst* data, std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->islandId, stream, offset))
        return false;

    if (!Deserialize(&data->resourceType, stream, offset))
        return false;

    return true;
}

bool Serialize(const IslandResourceRecordResult& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.capacity, stream))
        return false;

    if (!Serialize(data.amount, stream))
        return false;

    return true;
}

bool Deserialize(IslandResourceRecordResult* data, std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->capacity, stream, offset))
        return false;

    if (!Deserialize(&data->amount, stream, offset))
        return false;

    return true;
}

bool Serialize(const ShipCargoSlot& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.hasCargo, stream))
        return false;

    if (!Serialize(data.type, stream))
        return false;

    if (!Serialize(data.amount, stream))
        return false;

    return true;
}

bool Deserialize(ShipCargoSlot* data, std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->hasCargo, stream, offset))
        return false;

    if (!Deserialize(&data->type, stream, offset))
        return false;

    if (!Deserialize(&data->amount, stream, offset))
        return false;

    return true;
}

bool Serialize(const ShipCargoResult& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.slots.size();

    if (!Serialize(size, stream))
        return false;

    for (const ShipCargoSlot& slot : data.slots)
    {
        if (!Serialize(slot, stream))
            return false;
    }

    return true;
}

bool Deserialize(ShipCargoResult* data, std::vector<uint8_t>& stream, size_t* offset)
{
    data->slots.clear();

    uint64_t size = 0;

    if (!Deserialize(&size, stream, offset))
        return false;

    for (uint64_t i = 0; i < size; ++i)
    {
        ShipCargoSlot slot;
        
        if (!Deserialize(&slot, stream, offset))
            return false;

        data->slots.push_back(slot);
    }

    return true;
}

bool Serialize(const ShipCargoDumpRequest& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.shipId, stream))
        return false;

    if (!Serialize(data.cargoSlotIndex, stream))
        return false;

    if (!Serialize(data.x, stream))
        return false;

    if (!Serialize(data.y, stream))
        return false;

    return true;
}

bool Deserialize(ShipCargoDumpRequest* data, std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->shipId, stream, offset))
        return false;

    if (!Deserialize(&data->cargoSlotIndex, stream, offset))
        return false;

    if (!Deserialize(&data->x, stream, offset))
        return false;

    if (!Deserialize(&data->y, stream, offset))
        return false;

    return true;
}


bool Serialize(const ShipCargoLoadRequest& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.islandId, stream))
        return false;

    if (!Serialize(data.shipId, stream))
        return false;

    if (!Serialize(data.resourceId, stream))
        return false;

    if (!Serialize(data.amount, stream))
        return false;

    if (!Serialize(data.slotIndex, stream))
        return false;

    if (!Serialize(data.offloading, stream))
        return false;

    return true;
}

bool Deserialize(ShipCargoLoadRequest* data, std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->islandId, stream, offset))
        return false;

    if (!Deserialize(&data->shipId, stream, offset))
        return false;

    if (!Deserialize(&data->resourceId, stream, offset))
        return false;

    if (!Deserialize(&data->amount, stream, offset))
        return false;

    if (!Deserialize(&data->slotIndex, stream, offset))
        return false;

    if (!Deserialize(&data->offloading, stream, offset))
        return false;

    return true;
}

bool Serialize(const ShipMoveData& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.moves, stream))
        return false;

    if (!Serialize(data.x, stream))
        return false;

    if (!Serialize(data.y, stream))
        return false;

    if (!Serialize(data.rotation, stream))
        return false;

    return true;
}

bool Deserialize(ShipMoveData* data, std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->moves, stream, offset))
        return false;

    if (!Deserialize(&data->x, stream, offset))
        return false;

    if (!Deserialize(&data->y, stream, offset))
        return false;

    if (!Deserialize(&data->rotation, stream, offset))
        return false;

    return true;
}

bool Serialize(const BuildingBaseData& data, std::vector<uint8_t>& stream)
{
    if (!Serialize(data.buildingId, stream))
        return false;

    if (!Serialize(data.buildingType, stream))
        return false;

    if (!Serialize(data.xCoord, stream))
        return false;

    if (!Serialize(data.yCoord, stream))
        return false;

    return true;
}

bool Deserialize(BuildingBaseData* data, std::vector<uint8_t>& stream, size_t* offset)
{
    if (!Deserialize(&data->buildingId, stream, offset))
        return false;

    if (!Deserialize(&data->buildingType, stream, offset))
        return false;

    if (!Deserialize(&data->xCoord, stream, offset))
        return false;

    if (!Deserialize(&data->yCoord, stream, offset))
        return false;

    return true;
}

bool Serialize(const IslandBuildingData& data, std::vector<uint8_t>& stream)
{
    uint64_t listSize = data.buildings.size();

    if (!Serialize(listSize, stream))
        return false;

    for (uint64_t i = 0; i < listSize; ++i)
    {
        if (!Serialize(data.buildings[i], stream))
            return false;
    }

    return true;
}

bool Deserialize(IslandBuildingData* data, std::vector<uint8_t>& stream, size_t* offset)
{
    uint64_t listSize = 0;

    if (!Deserialize(&listSize, stream, offset))
        return false;

    data->buildings.clear();

    for (uint64_t i = 0; i < listSize; ++i)
    {
        BuildingBaseData entry;

        if (!Deserialize(&entry, stream, offset))
            return false;

        data->buildings.push_back(entry);
    }

    return true;
}

bool Serialize(const ShipTransferRegionInputData& data, std::vector<uint8_t>& stream)
{
    uint64_t size = data.shipIds.size();

    if (!Serialize(size, stream))
        return false;

    for (uint64_t id : data.shipIds)
    {
        if (!Serialize(id, stream))
            return false;
    }

    if (!Serialize(data.regionId, stream))
        return false;

    return true;
}


bool Deserialize(ShipTransferRegionInputData* data, std::vector<uint8_t>& stream, size_t* offset)
{
    uint64_t nIds = 0;

    data->shipIds.clear();

    if (!Deserialize(&nIds, stream, offset))
        return false;

    for (int i = 0; i < nIds; ++i)
    {
        uint64_t nextId = 0;
        if (!Deserialize(&nextId, stream, offset))
            return false;

        data->shipIds.push_back(nextId);
    }

    if (!Deserialize(&data->regionId, stream, offset))
        return false;

    return true;
}

