#include "Q/Tools.h"
#include <iostream>

SOCKET g_TargetCallSocket = NULL;

bool HandleScriptRequest(SOCKET socket)
{
    std::vector<uint8_t> buffer;

    // Read the request from Python
    if (!ReadMessageFromSocket(socket, buffer))
        return false;

    // Relay that resquest to the target code
    int to_target = send(g_TargetCallSocket, (char*)buffer.data(), (int)buffer.size(), 0);
    if (to_target == SOCKET_ERROR)
        return false;

    // Get the response back from the target
    if (!ReadMessageFromSocket(g_TargetCallSocket, buffer))
        return false;

    // Send the response back to python
    int to_python = send(socket, (char*)buffer.data(), (int)buffer.size(), 0);
    if (to_python == SOCKET_ERROR)
        return false;
}
    //bool expectedShutDown = false;

    //std::vector<uint8_t> stream(0x1000);
    //std::vector<uint8_t> outputBuffer;
    //int bytesRead = recv(socket, (char*)stream.data(), stream.size(), 0);

    //if (bytesRead == SOCKET_ERROR)
    //    return false;

    //stream.resize(bytesRead);

    //size_t offset = 0;

    //uint64_t inSize = 0;

    //Deserialize(&inSize, stream, &offset);

    //while (stream.size() < inSize)
    //{
    //    std::vector<uint8_t> tempBuffer(0x1000);

    //    int extraBytes = recv(socket, (char*)tempBuffer.data(), tempBuffer.size(), 0);

    //    if (extraBytes == SOCKET_ERROR)
    //        return false;

    //    tempBuffer.resize(extraBytes);

    //    stream.insert(stream.begin(), tempBuffer.begin(), tempBuffer.end());
    //}

    //uint64_t requestType = 0;
    //Deserialize(&requestType, stream, &offset);

    //switch (requestType)
    //{
    //case 1: // exit
    //{
    //    // uint64_t res = (uint64_t)TargetCallExit();
    //    uint64_t res = 1;
    //    Serialize(res, outputBuffer);
    //    expectedShutDown = true;
    //    break;
    //}
    //case 2: // Get ships by name
    //{
    //    std::string name;
    //    Deserialize(&name, stream, &offset);

    //    std::vector<uint64_t> shipList;

    //    uint64_t res = (uint64_t)TargetCallGetShipsByName(name, &shipList);
    //    
    //    Serialize(res, outputBuffer);
    //    Serialize(shipList, outputBuffer);

    //    break;
    //}
    //case 3: // Set ships waypoint
    //{
    //    ShipWaypointSetInputData dispatchData;

    //    Deserialize(&dispatchData.shipIds, stream, &offset);
    //    Deserialize(&dispatchData.x, stream, &offset);
    //    Deserialize(&dispatchData.y, stream, &offset);

    //    uint64_t res = (uint64_t)TargetCallSetShipsWaypoint(dispatchData);

    //    Serialize(res, outputBuffer);

    //    break;
    //}
    //case 4: // Set ships waypoint
    //{
    //    ShipWaypointSetInputData dispatchData;

    //    std::vector<uint64_t> shipList;
    //    uint64_t worldId = 0;

    //    Deserialize(&worldId, stream, &offset);

    //    uint64_t res = (uint64_t)TargetCallGetAllShipsInWorld(worldId, &shipList);

    //    Serialize(res, outputBuffer);
    //    Serialize(shipList, outputBuffer);

    //    break;
    //}
    //case 5: // Get islands by name
    //{
    //    std::string name;
    //    Deserialize(&name, stream, &offset);

    //    std::cout << "Get island request '" << name << "'" << std::endl;

    //    std::vector<uint64_t> islands;

    //    uint64_t res = (uint64_t)TargetCallGetIslandsByName(name, &islands);

    //    Serialize(res, outputBuffer);
    //    Serialize(islands, outputBuffer);

    //    break;
    //}
    //case 6: // Get resource from island
    //{
    //    uint64_t islandId;
    //    Deserialize(&islandId, stream, &offset);

    //    uint64_t resourceType;
    //    Deserialize(&resourceType, stream, &offset);

    //    uint64_t amount;
    //    uint64_t capacity;

    //    IslandResourceRecordRequst request;
    //    request.islandId = islandId;
    //    request.resourceType = resourceType;

    //    IslandResourceRecordResult result;

    //    uint64_t res = (uint64_t)TargetCallGetResourceFromIsland(request, &result);

    //    Serialize(res, outputBuffer);
    //    Serialize(result, outputBuffer);

    //    break;
    //}
    //case 7: // Get ship cargo
    //{
    //    uint64_t shipId;
    //    Deserialize(&shipId, stream, &offset);

    //    ShipCargoResult result;
    //    uint64_t res = (uint64_t)TargetCallGetShipCargo(shipId, &result);

    //    Serialize(res, outputBuffer);
    //    Serialize(result, outputBuffer);

    //    break;
    //}
    //case 8: // Ship dump cargo
    //{
    //    uint64_t shipId;
    //    Deserialize(&shipId, stream, &offset);

    //    uint64_t slotIndex;
    //    Deserialize(&slotIndex, stream, &offset);

    //    float x;
    //    Deserialize(&x, stream, &offset);

    //    float y;
    //    Deserialize(&y, stream, &offset);

    //    ShipCargoDumpRequest request;
    //    request.shipId = shipId;
    //    request.cargoSlotIndex = slotIndex;
    //    request.x = x;
    //    request.y = y;

    //    uint64_t res = (uint64_t)TargetCallDumpCargo(request);

    //    Serialize(res, outputBuffer);

    //    break;
    //}
    //case 9: // Load cargo between ship and island
    //{
    //    ShipCargoLoadRequest request;

    //    Deserialize(&request.islandId, stream, &offset);
    //    Deserialize(&request.shipId, stream, &offset);
    //    Deserialize(&request.resourceId, stream, &offset);
    //    Deserialize(&request.amount, stream, &offset);
    //    Deserialize(&request.slotIndex, stream, &offset);
    //    Deserialize(&request.offloading, stream, &offset);

    //    uint64_t res = TargetCallLoadCargo(request);
    //    uint64_t resultAmount = 1;

    //    Serialize(res, outputBuffer);
    //    Serialize(resultAmount, outputBuffer);

    //    break;
    //}
    //case 10: // Get ship move data
    //{
    //    uint64_t shipId;

    //    Deserialize(&shipId, stream, &offset);

    //    ShipMoveData moveData;
    //    uint64_t res = TargetCallGetShipMoveData(shipId, &moveData);

    //    Serialize(res, outputBuffer);
    //    Serialize(moveData, outputBuffer);

    //    break;
    //}
    //case 11: // Get buildings on island
    //{
    //    uint64_t islandId;

    //    Deserialize(&islandId, stream, &offset);

    //    IslandBuildingData data;
    //    uint64_t res = TargetCallGetIslandBuildings(islandId, &data);

    //    Serialize(res, outputBuffer);
    //    Serialize(data, outputBuffer);

    //    break;
    //}
    //case 12: // Begin transfer of a group of ships to another region
    //{
    //    ShipTransferRegionInputData moveData;

    //    Deserialize(&moveData, stream, &offset);

    //    uint64_t res = (uint64_t)TargetCallTransferShipsToWorld(moveData);

    //    Serialize(res, outputBuffer);

    //    break;
    //}
    //default:
    //    break;
    //}

    //uint64_t finalSize = outputBuffer.size() + 8;

    //std::vector<uint8_t> finalBuffer;
    //
    //Serialize(finalSize, finalBuffer);

    //finalBuffer.insert(finalBuffer.end(), outputBuffer.begin(), outputBuffer.end());

    //int sent = send(socket, (char*)finalBuffer.data(), finalBuffer.size(), 0);

    //if (sent == SOCKET_ERROR)
    //    return false;

    //if (expectedShutDown)
    //    return false;

    //return true;
//}
