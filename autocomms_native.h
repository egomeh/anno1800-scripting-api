#pragma once
#include <windows.h>
#include "Q/Tools.h"
#include "Q/autocomms.h"
#include "Q/remote_call_shared.h"
#include "globals.h"

namespace AutoComms
{{

{function_signatures}

inline bool HandleScriptCall(SOCKET socket)
{{
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
    {{
{handle_cases}
    default:
        break;
    }}

    std::vector<uint8_t> finalBuffer;
    uint64_t payloadSize = outputBuffer.size();
    uint64_t totalSize = payloadSize + 8 + 1;

    if (!AutoComms::Serialize(totalSize, finalBuffer))
        return false;

    if (!AutoComms::Serialize(success, finalBuffer))
        return false;

    finalBuffer.insert(finalBuffer.end(), outputBuffer.begin(), outputBuffer.end());

    int bytesSent = send(socket, (char*)finalBuffer.data(), (int)finalBuffer.size(), 0);

    if (bytesSent == SOCKET_ERROR)
        return false;

    return true;
}}

}}

