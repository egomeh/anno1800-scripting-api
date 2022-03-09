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
        case (1): // WriteMessageBox
        {
            std::string message;
            if (!Deserialize(&message, buffer_in, &offset))
                return false;

            success = callHandler.WriteMessageBox(message);

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
        case (3): // DebugGetResourceInfoFromAddress
        {
            uint64_t address;
            if (!Deserialize(&address, buffer_in, &offset))
                return false;

            IslandResource resource;

            success = callHandler.DebugGetResourceInfoFromAddress(address, &resource);

            if (!Serialize(success, buffer_out))
                return false;

            if (!Serialize(resource, buffer_out))
                return false;

            break;
        }
    }

    return socketHandler.Send(buffer_out);
}
