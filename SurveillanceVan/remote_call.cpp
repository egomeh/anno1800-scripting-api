
#include "remote_call.h"
#include <vector>
#include <windows.h>

#include "Q/remote_call_shared.h"



//
//bool TargetCallGetTime(uint64_t* output)
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::GetTime;
//    Serialize((uint64_t)messageType, stream);
//    
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    Deserialize(output, inBuffer, &offset);
//
//    return (bool)success;
//}
//
//
//
//bool TargetCallSetShipsWaypoint(const ShipWaypointSetInputData& input)
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::SetShipsWaypoint;
//    Serialize((uint64_t)messageType, stream);
//    Serialize(input, stream);
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    return (bool)success;
//}
//
//
//
//bool TargetCallExit()
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::Exit;
//    Serialize((uint64_t)messageType, stream);
//    
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    return (bool)success;
//}
//
//
//
//bool TargetCallGetShipName(const uint64_t& input, std::string* output)
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::GetShipName;
//    Serialize((uint64_t)messageType, stream);
//    Serialize(input, stream);
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    Deserialize(output, inBuffer, &offset);
//
//    return (bool)success;
//}
//
//
//
//bool TargetCallGetShipsByName(const std::string& input, std::vector<uint64_t>* output)
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::GetShipsByName;
//    Serialize((uint64_t)messageType, stream);
//    Serialize(input, stream);
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    Deserialize(output, inBuffer, &offset);
//
//    return (bool)success;
//}
//
//
//
//bool TargetCallGetAllShipsInWorld(const uint64_t& input, std::vector<uint64_t>* output)
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::GetAllShipsInWorld;
//    Serialize((uint64_t)messageType, stream);
//    Serialize(input, stream);
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    Deserialize(output, inBuffer, &offset);
//
//    return (bool)success;
//}
//
//
//
//bool TargetCallGetIslandsByName(const std::string& input, std::vector<uint64_t>* output)
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::GetIslandsByName;
//    Serialize((uint64_t)messageType, stream);
//    Serialize(input, stream);
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    Deserialize(output, inBuffer, &offset);
//
//    return (bool)success;
//}
//
//
//
//bool TargetCallGetResourceFromIsland(const IslandResourceRecordRequst& input, IslandResourceRecordResult* output)
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::GetResourceFromIsland;
//    Serialize((uint64_t)messageType, stream);
//    Serialize(input, stream);
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    Deserialize(output, inBuffer, &offset);
//
//    return (bool)success;
//}
//
//
//
//bool TargetCallGetShipCargo(const uint64_t& input, ShipCargoResult* output)
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::GetShipCargo;
//    Serialize((uint64_t)messageType, stream);
//    Serialize(input, stream);
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    Deserialize(output, inBuffer, &offset);
//
//    return (bool)success;
//}
//
//
//
//bool TargetCallDumpCargo(const ShipCargoDumpRequest& input)
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::DumpCargo;
//    Serialize((uint64_t)messageType, stream);
//    Serialize(input, stream);
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    return (bool)success;
//}
//
//
//
//bool TargetCallLoadCargo(const ShipCargoLoadRequest& input)
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::LoadCargo;
//    Serialize((uint64_t)messageType, stream);
//    Serialize(input, stream);
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    return (bool)success;
//}
//
//
//
//bool TargetCallGetShipMoveData(const uint64_t& input, ShipMoveData* output)
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::GetShipMoveData;
//    Serialize((uint64_t)messageType, stream);
//    Serialize(input, stream);
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    Deserialize(output, inBuffer, &offset);
//
//    return (bool)success;
//}
//
//
//
//bool TargetCallGetIslandBuildings(const uint64_t& input, IslandBuildingData* output)
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::GetIslandBuildings;
//    Serialize((uint64_t)messageType, stream);
//    Serialize(input, stream);
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    Deserialize(output, inBuffer, &offset);
//
//    return (bool)success;
//}
//
//
//
//bool TargetCallTransferShipsToWorld(const ShipTransferRegionInputData& input)
//{
//    std::vector<uint8_t> stream;
//    uint64_t messageType = (uint64_t)HostToTargetCall::TransferShipsToWorld;
//    Serialize((uint64_t)messageType, stream);
//    Serialize(input, stream);
//
//    uint64_t messageSize = stream.size() + 8;
//    std::vector<uint8_t> serializedSize;
//    Serialize(messageSize, serializedSize);
//
//    stream.insert(stream.begin(), serializedSize.begin(), serializedSize.end());
//
//    int result = send(g_TargetCallSocket, (char*)stream.data(), (int)stream.size(), 0);
//    if (result == SOCKET_ERROR)
//        return false;
//
//    std::vector<uint8_t> inBuffer;
//
//    uint8_t tempBuffer[1024];
//
//    int bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//    if (bytesRead == SOCKET_ERROR)
//        return false;
//
//    inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//
//    size_t offset = 0;
//
//    uint64_t inSize;
//    Deserialize(&inSize, inBuffer, &offset);
//
//    while (inBuffer.size() < inSize)
//    {
//        bytesRead = recv(g_TargetCallSocket, (char*)tempBuffer, sizeof(tempBuffer), 0);
//
//        if (bytesRead == SOCKET_ERROR)
//            return false;
//
//        inBuffer.insert(inBuffer.end(), tempBuffer, tempBuffer + bytesRead);
//    }
//
//    uint64_t success = 0;
//    Deserialize(&success, inBuffer, &offset);
//
//
//
//    return (bool)success;
//}
//
//
