// Auto generated code
#include "remote_call_handler_base.gen.h"
#include "serialization.gen.h"

bool HandleRemoteCall(SocketHandler& socketHandler, RemoteCallHandlerBase& callHandler)
{
    std::vector<uint8_t> buffer_in;
    std::vector<uint8_t> buffer_out;

    socketHandler.Receive(buffer_in);

    uint64_t function_id = 0;
    size_t offset = 0;

    if (!Deserialize(&function_id, buffer_in, &offset)) return false;

    bool success = false;

    switch (function_id)
    {
        case (1): // WriteToLog
        {
            std::string message;
            if (!Deserialize(&message, buffer_in, &offset))
                return false;

            success = callHandler.WriteToLog(message);

            if (!Serialize(success, buffer_out))
                return false;

            break;
        }
        case (2): // GetGameTime
        {
            uint64_t time;

            uint64_t frame;

            success = callHandler.GetGameTime(&time, &frame);

            if (!Serialize(success, buffer_out))
                return false;

            if (!Serialize(time, buffer_out))
                return false;

            if (!Serialize(frame, buffer_out))
                return false;

            break;
        }
        case (3): // GetShipWaypoints
        {
            std::string something;
            if (!Deserialize(&something, buffer_in, &offset))
                return false;

            uint64_t shipID;
            if (!Deserialize(&shipID, buffer_in, &offset))
                return false;

            std::vector<Coordinate> waypoints;

            success = callHandler.GetShipWaypoints(something, shipID, &waypoints);

            if (!Serialize(success, buffer_out))
                return false;

            if (!Serialize(waypoints, buffer_out))
                return false;

            break;
        }
        case (4): // GetIslandIds
        {
            std::vector<uint64_t> ids;

            success = callHandler.GetIslandIds(&ids);

            if (!Serialize(success, buffer_out))
                return false;

            if (!Serialize(ids, buffer_out))
                return false;

            break;
        }
        case (5): // GetIslandNames
        {
            int32_t Area;
            if (!Deserialize(&Area, buffer_in, &offset))
                return false;

            std::vector<std::string> names;

            success = callHandler.GetIslandNames(Area, &names);

            if (!Serialize(success, buffer_out))
                return false;

            if (!Serialize(names, buffer_out))
                return false;

            break;
        }
    }

    return socketHandler.Send(buffer_out);
}
