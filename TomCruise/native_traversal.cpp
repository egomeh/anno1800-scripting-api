#include "pch.h"
#include "native_traversal.h"
#include "native_callbacks.h"
#include "globals.h"

#include "Q/Tools.h"

std::string GetStringFromAnnoString(uint64_t stringAddress)
{
    if (!stringAddress)
        return "[Could not retreive]";

    if (!IsReadable((void*)(stringAddress + 0x18), 8))
        return "[Could not read longString]";

    if (!IsReadable((void*)(stringAddress + 0x10), 8))
        return "[Could not read length]";

    if (!IsReadable((void*)(stringAddress), 16))
        return "[Could not read string address]";

    uint64_t longString = ReadU64(stringAddress + 0x18);
    uint64_t length = ReadU64(stringAddress + 0x10);

    if (length > 64)
        return "[The name cannot be that long...]";

    bool isLong = longString > 0x7;

    wchar_t buffer[256];

    wchar_t* stringLocation = (wchar_t*)(stringAddress);

    if (isLong)
        stringLocation = (wchar_t*)ReadU64(stringAddress);

    if (!IsReadable((void*)(stringLocation), length))
        return "[Could not read pointer to string]";

    wmemset(buffer, 0, _countof(buffer));
    wmemcpy(buffer, stringLocation, length);

    std::wstring wideString = buffer;
    std::string name = utf8_encode(wideString);

    return name;
}

std::string GetStringFromNameComponent(uint64_t componentAddress)
{
    if (!componentAddress)
        return "[Could not retreive]";

    return GetStringFromAnnoString(componentAddress + 0x20);
}

bool GetIslandById(const uint64_t id, uint64_t* islandBaseAddress)
{
    for (uint64_t regionBaseAddress : regionBases)
    {
        uint64_t listSizePtr = regionBaseAddress + 0x88;
        uint64_t regionIslandList = regionBaseAddress + 0x90;

        if (!IsReadable((void*)listSizePtr, 8))
            continue;

        if (!IsReadable((void*)regionBaseAddress, 8))
            continue;

        uint64_t listSize = ReadU64(listSizePtr);
        uint64_t listBase = ReadU64(regionIslandList);

        if (!listBase)
            continue;

        if (!listSize)
            continue;

        for (uint64_t i = 0; i <= listSize; ++i)
        {
            uint64_t islandPtr = listBase + i * 8;

            if (!IsReadable((void*)islandPtr, 8))
                continue;

            uint64_t islandBase = ReadU64(islandPtr);

            // Follow linked list pointers
            for (; islandBase; islandBase = ReadU64(islandBase))
            {
                uint64_t islandIdAddress = islandBase + 0x10;

                if (!IsReadable((void*)islandIdAddress, 8))
                    continue;

                uint64_t islandId = ReadU64(islandIdAddress);

                if (islandId == id)
                {
                    *islandBaseAddress = islandBase;
                    return true;
                }
            }
        }
    }

    return false;
}

bool GetIslandIDsByName(const std::string& name, std::vector<uint64_t>* ids)
{
    ids->clear();

    for (uint64_t regionBaseAddress : regionBases)
    {
        uint64_t listSizePtr = regionBaseAddress + 0x88;
        uint64_t regionIslandList = regionBaseAddress + 0x90;

        if (!IsReadable((void*)listSizePtr, 8))
            continue;

        if (!IsReadable((void*)regionBaseAddress, 8))
            continue;

        uint64_t listSize = ReadU64(listSizePtr);
        uint64_t listBase = ReadU64(regionIslandList);

        if (!listBase)
            continue;

        if (!listSize)
            continue;

        for (uint64_t i = 0; i <= listSize; ++i)
        {
            uint64_t islandPtr = listBase + i * 8;

            if (!IsReadable((void*)islandPtr, 8))
                continue;

            uint64_t islandBase = ReadU64(islandPtr);

            // Follow linked list pointers
            for (; islandBase; islandBase = ReadU64(islandBase))
            {
                uint64_t nameAddress = islandBase + 0x168;
                std::string islandName = GetStringFromAnnoString(nameAddress);

                if (name.compare(islandName) != 0)
                    continue;

                uint64_t islandIdAddress = islandBase + 0x10;

                if (!IsReadable((void*)islandIdAddress, 8))
                    continue;

                uint64_t islandId = ReadU64(islandIdAddress);

                // I suspect that the ID is really only a 32-bit value
                islandId &= 0x00000000FFFFFFFF;

                if (std::find(ids->begin(), ids->end(), islandId) != ids->end())
                    continue;

                ids->push_back(islandId);
                SEND_FORMATTED("Found '%s' with at address %llx", islandName.c_str(), islandBase);
            }
        }
    }

    return true;
}

bool GetAllIslands(std::vector<AutoComms::IslandData>* islands)
{
    islands->clear();
    std::vector<uint64_t> alreadyFoundIds;

    for (uint64_t regionBaseAddress : regionBases)
    {
        uint64_t listSizePtr = regionBaseAddress + 0x88;
        uint64_t regionIslandList = regionBaseAddress + 0x90;

        if (!IsReadable((void*)listSizePtr, 8))
            continue;

        if (!IsReadable((void*)regionBaseAddress, 8))
            continue;

        uint64_t listSize = ReadU64(listSizePtr);
        uint64_t listBase = ReadU64(regionIslandList);

        if (!listBase)
            continue;

        if (!listSize)
            continue;

        for (uint64_t i = 0; i <= listSize; ++i)
        {
            uint64_t islandPtr = listBase + i * 8;

            if (!IsReadable((void*)islandPtr, 8))
                continue;

            uint64_t islandBase = ReadU64(islandPtr);

            // Follow linked list pointers
            for (; islandBase; islandBase = ReadU64(islandBase))
            {
                uint64_t nameAddress = islandBase + 0x168;
                std::string islandName = GetStringFromAnnoString(nameAddress);

                uint64_t islandIdAddress = islandBase + 0x10;

                if (!IsReadable((void*)islandIdAddress, 8))
                    continue;

                uint64_t islandId = ReadU64(islandIdAddress);

                // I suspect that the ID is really only a 32-bit value
                islandId &= 0x00000000FFFFFFFF;

                if (std::find(alreadyFoundIds.begin(), alreadyFoundIds.end(), islandId) != alreadyFoundIds.end())
                    continue;

                AutoComms::IslandData island;
                island.id = islandId;
                island.name = islandName;
                alreadyFoundIds.push_back(islandId);
                islands->push_back(island);
                SEND_FORMATTED("Found '%s' with at address %llx", islandName.c_str(), islandBase);
            }
        }
    }

    return true;
}

bool GetIslandResource(const IslandResourceRecordRequst& request, IslandResourceRecordResult* result)
{
    result->amount = 0;
    result->capacity = 0;

    for (uint64_t regionBaseAddress : regionBases)
    {
        uint64_t listSizePtr = regionBaseAddress + 0x88;
        uint64_t regionIslandList = regionBaseAddress + 0x90;

        if (!IsReadable((void*)listSizePtr, 8))
            continue;

        if (!IsReadable((void*)regionBaseAddress, 8))
            continue;

        uint64_t listSize = ReadU64(listSizePtr);
        uint64_t listBase = ReadU64(regionIslandList);

        if (!listBase)
            continue;

        if (!listSize)
            continue;

        for (uint64_t i = 0; i <= listSize; ++i)
        {
            uint64_t islandPtr = listBase + i * 8;

            if (!IsReadable((void*)islandPtr, 8))
                continue;

            uint64_t islandBase = ReadU64(islandPtr);

            // Follow linked list pointers
            for (; islandBase; islandBase = ReadU64(islandBase))
            {
                uint64_t islandIdAddress = islandBase + 0x10;

                if (!IsReadable((void*)islandIdAddress, 8))
                    continue;

                uint64_t islandId = ReadU64(islandIdAddress);

                if (islandId == request.islandId)
                {
                    uint64_t resourceStructPtr = islandBase + 0x1A8;

                    if (!IsReadable((void*)resourceStructPtr, 8))
                        return false;

                    uint64_t resourceStructBase = ReadU64(resourceStructPtr);

                    uint64_t resourceTableSizePtr = resourceStructBase + 0x60;
                    uint64_t resourceTableBasePtr = resourceStructBase + 0x68;

                    if (!IsReadable((void*)resourceTableSizePtr, 8))
                        return false;

                    if (!IsReadable((void*)resourceTableBasePtr, 8))
                        return false;

                    uint64_t size = ReadU64(resourceTableSizePtr);
                    uint64_t tableBase = ReadU64(resourceTableBasePtr);

                    result->amount = tableBase;
                    result->capacity = 2;

                    for (uint64_t k = 0; k <= size; ++k)
                    {
                        uint64_t recordBase = ReadU64(tableBase + k * 8);

                        for (; recordBase; recordBase = ReadU64(recordBase))
                        {
                            uint64_t capacityPtr = recordBase + 0x1C;
                            uint64_t typePtr = recordBase + 0x10;
                            uint64_t amountPtr = recordBase + 0x20;

                            uint32_t amount = ReadU32(amountPtr);
                            uint32_t type = ReadU32(typePtr);
                            uint32_t capacity = ReadU32(capacityPtr);

                            if (type != (uint32_t)request.resourceType)
                                continue;

                            SEND_FORMATTED("Found resource %llx at island base address %llx Region %llx", type, islandBase, regionBaseAddress);

                            result->amount = amount;
                            result->capacity = capacity;

                            return true;
                        }
                    }

                    return true;
                }
            }
        }
    }

    return false;
}

bool GetShipById(const uint64_t id, uint64_t* shipBaseAddress)
{
    bool found = false;

    for (uint64_t address : shipLists)
    {
        uint64_t listPtr = ReadU64(address + 0x20);
        uint64_t size = ReadU64(address + 0x28);

        for (uint64_t i = 0; i < size; ++i)
        {
            uint64_t shipId = ReadU64(listPtr + i * 0x20);
            uint64_t shipPtr = ReadU64(listPtr + i * 0x20 + 0x8);

            if (id == shipId)
            {
                *shipBaseAddress = shipPtr;
                found = true;
            }
        }
    }

    return found;
}

bool GetIslandBuildings(const uint64_t islandID, std::vector<AutoComms::BuildingData>* buildings, BuildingCache* cache)
{
    buildings->clear();

    // SCOPE_LOCK(&shipServiceLock);

    uint64_t islandBaseAddress = 0;

    if (!GetIslandById(islandID, &islandBaseAddress))
        return false;

    if (cache)
    {
        if (cache->map.find(islandID) == cache->map.end())
        {
            cache->map[islandID] = std::unordered_map<uint64_t, uint64_t>();
        }

        cache->map[islandID].clear();
    }

    uint64_t buildingListSizePtr = islandBaseAddress + 0x88;
    uint64_t buildingListPtr = islandBaseAddress + 0x90;

    uint64_t listSize = ReadU64(buildingListSizePtr);
    uint64_t buildingListBase = ReadU64(buildingListPtr);

    //SEND_FORMATTED("found base of island %llx", islandBaseAddress);

    //SEND_FORMATTED("listSize %llu", listSize);
    //SEND_FORMATTED("listPtr %llx", buildingListPtr);

    std::set<uint64_t> preCollectionList;

    int preCollectionIterations = 0;

    for (uint64_t i = 0; i < listSize; ++i)
    {
        uint64_t ptr = ReadU64(buildingListBase + i * 8);

        while (ptr)
        {
            ++preCollectionIterations;
            preCollectionList.insert(ptr);
            ptr = ReadU64(ptr);
        }
    }

    int secondCollectionIterations = 0;

    std::set<uint64_t> secondCollectionList;

    for (uint64_t address : preCollectionList)
    {
        if (!IsReadable((void*)address, 8))
            continue;

        uint64_t secondSize = ReadU32(address + 0x38);
        uint64_t secondPassList = ReadU64(address + 0x40);

        if (!secondPassList)
            continue;

        if (!IsReadablePointer(secondPassList))
            continue;

        std::set<uint64_t> visitedPointers;

        for (uint64_t i = 0; i < secondSize; ++i)
        {
            if (!IsReadablePointer(secondPassList + i * 8))
                continue;

            uint64_t ptr = ReadU64(secondPassList + i * 8);

            while (ptr)
            {
                if (visitedPointers.find(ptr) != visitedPointers.end())
                    break;

                visitedPointers.insert(ptr);

               //+counter;
                if (!IsReadablePointer(ptr + 0x10))
                    break;

                uint64_t test1 = ReadU64(ptr + 0x10);
                uint64_t currentPtr = ptr;
                ptr = ReadU64(ptr);

                test1 = (0x0000FFFF00000000 & test1) >> 0x20;

                if (test1 != islandID)
                    continue;

                uint64_t baseAddressPtr = currentPtr + 0x18;

                if (!IsReadablePointer(baseAddressPtr))
                {
                    SEND_FORMATTED("could not read at address %lls", baseAddressPtr);
                    continue;
                }

                uint64_t buildingAddress = ReadU64(baseAddressPtr);
                secondCollectionList.insert(buildingAddress);
                ++secondCollectionIterations;
            }
        }
    }

    for (uint64_t buildingAddress : secondCollectionList)
    {
        AutoComms::BuildingData building;

        building.id = ReadU32(buildingAddress + 0x28) & 0xFFFFFFFF;
        building.position.x = ReadF32(buildingAddress + 0x3c);
        building.position.y = ReadF32(buildingAddress + 0x44);
        building.rawBuildingTypeID = ReadU32(buildingAddress + 0x8);
        building.buidlingType = IDToBuildingType(building.rawBuildingTypeID);
        building.debug_address = buildingAddress;

        cache->map[islandID][building.id] = buildingAddress;
        buildings->push_back(building);
    }

    //SEND_FORMATTED("Collection 1 %d", preCollectionIterations);
    //SEND_FORMATTED("Collection 1 size %d", (int)preCollectionList.size());
    //SEND_FORMATTED("Collection 2 %d", secondCollectionIterations);
    //SEND_FORMATTED("Collection 2 size %d", (int)secondCollectionList.size());

    return true;
}

bool GetBuildingAddress(const uint64_t islandID, const uint64_t buildingID, BuildingCache* buildingCache, uint64_t* address)
{
    auto islandBuildingMap = buildingCache->map.find(islandID);

    std::vector<AutoComms::BuildingData> buildingData;

    if (islandBuildingMap == buildingCache->map.end())
    {
        SEND_FORMATTED("Island not registered");
        GetIslandBuildings(islandID, &buildingData, buildingCache);
    }

    auto buildingAddress = islandBuildingMap->second.find(buildingID);

    if (buildingAddress == islandBuildingMap->second.end())
        return false;

    uint64_t finalAddress = buildingAddress->second;

    uint64_t tentativeID = ReadU32(finalAddress + 0x28) & 0xFFFFFFFF;

    bool hasCorrectAddress = tentativeID == buildingID;
    if (!hasCorrectAddress)
    {
        GetIslandBuildings(islandID, &buildingData, buildingCache);

        for (auto building : buildingData)
        {
            if (building.id == buildingID)
            {
                finalAddress = building.debug_address;
                hasCorrectAddress = true;
                break;
            }
        }
    }

    if (!hasCorrectAddress)
        return false;

    *address = finalAddress;

    return true;
}
