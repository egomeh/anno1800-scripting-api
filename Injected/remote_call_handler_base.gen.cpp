// Auto generated code
#include "remote_call_handler_base.gen.h"
#include "serialization.gen.h"

bool HandleRemoteCall(SocketHandler& socketHandler, RemoteCallHandlerBase& callHandler)
{
    std::vector<uint8_t> buffer_in;
    std::vector<uint8_t> buffer_out;

    if (!socketHandler.Receive(buffer_in))
        return false;

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
        case (4): // DebugGetResourceChainInfoFromAddress
        {
            uint64_t address;
            if (!Deserialize(&address, buffer_in, &offset))
                return false;

            std::vector<IslandResource> resource;

            success = callHandler.DebugGetResourceChainInfoFromAddress(address, &resource);

            if (!Serialize(success, buffer_out))
                return false;

            if (!Serialize(resource, buffer_out))
                return false;

            break;
        }
        case (5): // DebugGetIslandNameFromAddress
        {
            uint64_t address;
            if (!Deserialize(&address, buffer_in, &offset))
                return false;

            std::string name;

            success = callHandler.DebugGetIslandNameFromAddress(address, &name);

            if (!Serialize(success, buffer_out))
                return false;

            if (!Serialize(name, buffer_out))
                return false;

            break;
        }
        case (6): // DebugReadStringFromAddress
        {
            uint64_t address;
            if (!Deserialize(&address, buffer_in, &offset))
                return false;

            std::string name;

            success = callHandler.DebugReadStringFromAddress(address, &name);

            if (!Serialize(success, buffer_out))
                return false;

            if (!Serialize(name, buffer_out))
                return false;

            break;
        }
        case (7): // DebugGetIslandResources
        {
            uint64_t address;
            if (!Deserialize(&address, buffer_in, &offset))
                return false;

            std::vector<IslandResource> resources;

            success = callHandler.DebugGetIslandResources(address, &resources);

            if (!Serialize(success, buffer_out))
                return false;

            if (!Serialize(resources, buffer_out))
                return false;

            break;
        }
        case (8): // DebugGetIslandChainFromAddress
        {
            uint64_t address;
            if (!Deserialize(&address, buffer_in, &offset))
                return false;

            std::vector<IslandInfo> islands;

            success = callHandler.DebugGetIslandChainFromAddress(address, &islands);

            if (!Serialize(success, buffer_out))
                return false;

            if (!Serialize(islands, buffer_out))
                return false;

            break;
        }
        case (9): // DebugGetFirstAreaStructAddress
        {
            uint64_t address;

            success = callHandler.DebugGetFirstAreaStructAddress(&address);

            if (!Serialize(success, buffer_out))
                return false;

            if (!Serialize(address, buffer_out))
                return false;

            break;
        }
        case (10): // DebugGetAreaWithCode
        {
            uint32_t code;
            if (!Deserialize(&code, buffer_in, &offset))
                return false;

            uint64_t address;

            success = callHandler.DebugGetAreaWithCode(code, &address);

            if (!Serialize(success, buffer_out))
                return false;

            if (!Serialize(address, buffer_out))
                return false;

            break;
        }
        case (11): // GetPlayerIslandsInWorld
        {
            uint32_t area;
            if (!Deserialize(&area, buffer_in, &offset))
                return false;

            std::vector<IslandInfo> islands;

            success = callHandler.GetPlayerIslandsInWorld(area, &islands);

            if (!Serialize(success, buffer_out))
                return false;

            if (!Serialize(islands, buffer_out))
                return false;

            break;
        }
    }

    return socketHandler.Send(buffer_out);
}
