#pragma once
#include <windows.h>
#include "Q/Tools.h"
#include "Q/autocomms.h"
#include "Q/remote_call_shared.h"
#include "globals.h"

//bool RemoteInGetTime(uint64_t* output);
//bool RemoteInSetShipsWaypoint(const ShipWaypointSetInputData& input);
//bool RemoteInExit();
//bool RemoteInGetShipName(const uint64_t& input, std::string* output);
//bool RemoteInGetShipsByName(const std::string& input, std::vector<uint64_t>* output);
//bool RemoteInGetAllShipsInWorld(const uint64_t& input, std::vector<uint64_t>* output);
//bool RemoteInGetIslandsByName(const std::string& input, std::vector<uint64_t>* output);
//bool RemoteInGetResourceFromIsland(const IslandResourceRecordRequst& input, IslandResourceRecordResult* output);
//bool RemoteInGetShipCargo(const uint64_t& input, ShipCargoResult* output);
//bool RemoteInDumpCargo(const ShipCargoDumpRequest& input);
//bool RemoteInLoadCargo(const ShipCargoLoadRequest& input);
//bool RemoteInGetShipMoveData(const uint64_t& input, ShipMoveData* output);
//bool RemoteInGetIslandBuildings(const uint64_t& input, IslandBuildingData* output);
//bool RemoteInTransferShipsToWorld(const ShipTransferRegionInputData& input);

inline bool HandleRemoteCall(SOCKET socket)
{
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> outputBuffer;

    if (!ReadMessageFromSocket(socket, buffer))
        return false;

    outputBuffer.clear();

    size_t offset = 0;

    uint64_t message_size = 0;
    uint64_t function_id = 0;

    if (!AutoComms::Deserialize(&message_size, buffer, &offset))
        return false;

    if (!AutoComms::Deserialize(&function_id, buffer, &offset))
        return false;

    bool success = false;

    switch (function_id)
    {
    default:
        break;
    }

    std::vector<uint8_t> finalBuffer;
    uint64_t payloadSize = outputBuffer.size();
    uint64_t totalSize = payloadSize + 8 + 1;

    if (!AutoComms::Serialize(totalSize, finalBuffer))
        return false;

    if (!AutoComms::Serialize(success, finalBuffer))
        return false;

    finalBuffer.insert(finalBuffer.end(), outputBuffer.begin(), outputBuffer.end());

    int bytesSent = send(socket, (char*)finalBuffer.data(), finalBuffer.size(), 0);

    if (bytesSent == SOCKET_ERROR)
        return true;

    return true;
}

    //std::vector<uint8_t> buffer(0x1000);
    //std::vector<uint8_t> outputBuffer;
    //
    //int bytesRead = recv(socket, (char*)buffer.data(), buffer.size(), 0);

    //if (bytesRead == SOCKET_ERROR)
    //    return false;



    //buffer.resize(bytesRead);

    //size_t offset = 0;

    //uint64_t inSize = 0;

    //Deserialize((uint64_t*)&inSize, buffer, &offset);

    //while (buffer.size() != inSize)
    //{
    //    std::vector<uint8_t> tempBuffer(0x1000);
    //    int extraBytesRead = recv(socket, (char*)tempBuffer.data(), tempBuffer.size(), 0);

    //    if ( extraBytesRead == SOCKET_ERROR)
    //        return false;

    //    tempBuffer.resize(extraBytesRead);

    //    buffer.insert(buffer.end(), tempBuffer.begin(), tempBuffer.end());
    //}

    //HostToTargetCall messageType;
    //Deserialize((uint64_t*)&messageType, buffer, &offset);

    //switch (messageType)
    //{
    //case HostToTargetCall::GetTime:
    //    {
    //        // input;
    //        uint64_t output;

    //        uint64_t success = (uint64_t)RemoteInGetTime(&output);
    //        Serialize(success, outputBuffer);
    //        Serialize(output, outputBuffer);
    //        break;
    //    }

    //case HostToTargetCall::SetShipsWaypoint:
    //    {
    //        ShipWaypointSetInputData input;
    //        // output;
    //        Deserialize(&input, buffer, &offset);
    //        uint64_t success = (uint64_t)RemoteInSetShipsWaypoint(input);
    //        Serialize(success, outputBuffer);

    //        break;
    //    }

    //case HostToTargetCall::Exit:
    //    {
    //        // input;
    //        // output;

    //        uint64_t success = (uint64_t)RemoteInExit();
    //        Serialize(success, outputBuffer);

    //        break;
    //    }

    //case HostToTargetCall::GetShipName:
    //    {
    //        uint64_t input;
    //        std::string output;
    //        Deserialize(&input, buffer, &offset);
    //        uint64_t success = (uint64_t)RemoteInGetShipName(input, &output);
    //        Serialize(success, outputBuffer);
    //        Serialize(output, outputBuffer);
    //        break;
    //    }

    //case HostToTargetCall::GetShipsByName:
    //    {
    //        std::string input;
    //        std::vector<uint64_t> output;
    //        Deserialize(&input, buffer, &offset);
    //        uint64_t success = (uint64_t)RemoteInGetShipsByName(input, &output);
    //        Serialize(success, outputBuffer);
    //        Serialize(output, outputBuffer);
    //        break;
    //    }

    //case HostToTargetCall::GetAllShipsInWorld:
    //    {
    //        uint64_t input;
    //        std::vector<uint64_t> output;
    //        Deserialize(&input, buffer, &offset);
    //        uint64_t success = (uint64_t)RemoteInGetAllShipsInWorld(input, &output);
    //        Serialize(success, outputBuffer);
    //        Serialize(output, outputBuffer);
    //        break;
    //    }

    //case HostToTargetCall::GetIslandsByName:
    //    {
    //        std::string input;
    //        std::vector<uint64_t> output;
    //        Deserialize(&input, buffer, &offset);
    //        uint64_t success = (uint64_t)RemoteInGetIslandsByName(input, &output);
    //        Serialize(success, outputBuffer);
    //        Serialize(output, outputBuffer);
    //        break;
    //    }

    //case HostToTargetCall::GetResourceFromIsland:
    //    {
    //        IslandResourceRecordRequst input;
    //        IslandResourceRecordResult output;
    //        Deserialize(&input, buffer, &offset);
    //        uint64_t success = (uint64_t)RemoteInGetResourceFromIsland(input, &output);
    //        Serialize(success, outputBuffer);
    //        Serialize(output, outputBuffer);
    //        break;
    //    }

    //case HostToTargetCall::GetShipCargo:
    //    {
    //        uint64_t input;
    //        ShipCargoResult output;
    //        Deserialize(&input, buffer, &offset);
    //        uint64_t success = (uint64_t)RemoteInGetShipCargo(input, &output);
    //        Serialize(success, outputBuffer);
    //        Serialize(output, outputBuffer);
    //        break;
    //    }

    //case HostToTargetCall::DumpCargo:
    //    {
    //        ShipCargoDumpRequest input;
    //        // output;
    //        Deserialize(&input, buffer, &offset);
    //        uint64_t success = (uint64_t)RemoteInDumpCargo(input);
    //        Serialize(success, outputBuffer);

    //        break;
    //    }

    //case HostToTargetCall::LoadCargo:
    //    {
    //        ShipCargoLoadRequest input;
    //        // output;
    //        Deserialize(&input, buffer, &offset);
    //        uint64_t success = (uint64_t)RemoteInLoadCargo(input);
    //        Serialize(success, outputBuffer);

    //        break;
    //    }

    //case HostToTargetCall::GetShipMoveData:
    //    {
    //        uint64_t input;
    //        ShipMoveData output;
    //        Deserialize(&input, buffer, &offset);
    //        uint64_t success = (uint64_t)RemoteInGetShipMoveData(input, &output);
    //        Serialize(success, outputBuffer);
    //        Serialize(output, outputBuffer);
    //        break;
    //    }

    //case HostToTargetCall::GetIslandBuildings:
    //    {
    //        uint64_t input;
    //        IslandBuildingData output;
    //        Deserialize(&input, buffer, &offset);
    //        uint64_t success = (uint64_t)RemoteInGetIslandBuildings(input, &output);
    //        Serialize(success, outputBuffer);
    //        Serialize(output, outputBuffer);
    //        break;
    //    }

    //case HostToTargetCall::TransferShipsToWorld:
    //    {
    //        ShipTransferRegionInputData input;
    //        // output;
    //        Deserialize(&input, buffer, &offset);
    //        uint64_t success = (uint64_t)RemoteInTransferShipsToWorld(input);
    //        Serialize(success, outputBuffer);

    //        break;
    //    }

    //default:
    //    break;
    //}

    //std::vector<uint8_t> finalBuffer;
    //uint64_t payloadSize = outputBuffer.size();
    //uint64_t totalSize = payloadSize + 8;

    //Serialize(totalSize, finalBuffer);
    //finalBuffer.insert(finalBuffer.end(), outputBuffer.begin(), outputBuffer.end());

    //int bytesSent = send(socket, (char*)finalBuffer.data(), finalBuffer.size(), 0);

    //return true;
//}

