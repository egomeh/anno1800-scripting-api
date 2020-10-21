
#include "Q/Tools.h"
#include "Q/structs.h"

#include <iostream>
#include <string>

#include <mutex>

#include <cstdio>
#include <cstdlib>

#include "remote_call.h"

#include "Q/comms.h"

#include "scripting.h"

std::mutex g_printlock;

#define LOCKED_PRINT(command) {g_printlock.lock(); command; g_printlock.unlock();}

#define FAIL(message) { LOCKED_PRINT(printf(message)); return 1; }

struct Context
{
    SOCKET socket;
    bool keepGoing;
};

bool InjectDLL(HANDLE process, const char* dllPath)
{
    bool success = false;

    LPVOID dllPathAddress = 0;
    FARPROC loadLibraryAddrress = 0;
    HANDLE remoteThread = 0;

    dllPathAddress = VirtualAllocEx(process,
        0,
        strlen(dllPath),
        MEM_RESERVE | MEM_COMMIT,
        PAGE_EXECUTE_READWRITE);

    SCOPE_GUARD(if (dllPathAddress) { VirtualFreeEx(process, dllPathAddress, strlen(dllPath), MEM_RELEASE); });

    if (dllPathAddress == 0)
        FAIL("Could not allocate memory in process");

    if (!WriteProcessMemory(process, dllPathAddress, dllPath, strlen(dllPath), NULL))
        FAIL("Could not write to process memory");

    loadLibraryAddrress = GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");

    remoteThread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddrress, dllPathAddress, 0, NULL);

    if (!remoteThread)
        FAIL("Failed to start thread in the target process");

    WaitForSingleObject(remoteThread, INFINITE);

    return true;
}

struct WaitOnSocket
{
    SOCKET listenSocket;
    SOCKET clientSocket;
};

DWORD WaitOnSocketAccept(void *socketParameter)
{
    WaitOnSocket &block = *(WaitOnSocket*)socketParameter;
    block.clientSocket = accept(block.listenSocket, NULL, NULL);
    return 0;
}

bool HandleOutgoingMessages(Context &context)
{
    Message message;
    std::string command;
    getline(std::cin, command);

    LOCKED_PRINT(command.c_str());

    memset(message.payload, 0, sizeof(message.payload));

    if (command.rfind("exit", 0) == 0)
    {
        message.type = MessageType::Exit;
    }
    else if (command.rfind("python", 0) == 0)
    {
        message.type = MessageType::PytonExec;
        memset(message.payload, 0, sizeof(message.payload));

        command.erase(0, 7);

        if (command.size() > sizeof(message.payload) - 1)
        {
            LOCKED_PRINT("Command too long");
            return true;
        }

        memcpy(message.payload, command.c_str(), command.size());
        message.payload[sizeof(message.payload) - 1] = 0;
    }
    else if (command.rfind("print", 0) == 0)
    {
        message.type = MessageType::Print;

        size_t printlength = strlen("print ");
        size_t argumentLenght = command.size() - printlength;
        size_t payloadLength = sizeof(Message::payload);

        if (command.size() > strlen("print "))
            memcpy(message.payload, command.c_str() + printlength, min(argumentLenght, payloadLength));
    }
    else if (command.rfind("rir", 0) == 0)
    {
        // Register Island resorce
        message.type = MessageType::RegisterIslandResources;

        size_t printlength = strlen("rir ");
        size_t argumentLenght = command.size() - printlength;
        size_t payloadLength = sizeof(Message::payload);

        if (command.size() > strlen("rir "))
            memcpy(message.payload, command.c_str() + printlength, min(argumentLenght, payloadLength));
    }
    else if (command.rfind("plot", 0) == 0)
    {
        // Open window to render
    }
    else if (command.rfind("move", 0) == 0)
    {
        message.type = MessageType::MoveShip;

        size_t printlength = strlen("move ");
        size_t argumentLenght = command.size() - printlength;
        size_t payloadLength = sizeof(Message::payload);

        if (command.size() > strlen("move "))
            memcpy(message.payload, command.c_str() + printlength, min(argumentLenght, payloadLength));
    }

    if (!HackSendMessage(context.socket, &message))
        return false;

    return true;
}

DWORD HandleIncommingMessage(void* arg)
{
    Context* context = (Context*)arg;

    // Handle a little better I think...
    if (context == nullptr)
        return -1;

    Message message;
    while (context->keepGoing)
    {
        if (!HackReceiveMessage(context->socket, &message))
            return -1;

        switch (message.type)
        {
        case MessageType::Exit:
            context->keepGoing = false;
            break;
        case MessageType::Print:
            LOCKED_PRINT(printf("From DLL: "));
            LOCKED_PRINT(printf((char*)message.payload));
            LOCKED_PRINT(printf("\n"));
            break;
        case MessageType::PrintNoLabel:
            LOCKED_PRINT(printf((char*)message.payload));
            LOCKED_PRINT(printf("\n"));
            break;
        default: break;
        }
    }

    return 0;
}

int main()
{
    constexpr int bufferSize = 4096;
    constexpr const char* defaultPort = "30010";

    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    WaitOnSocket block;
    block.clientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int recvbuflen = bufferSize;

    HANDLE socketWaitThread = 0;

    HANDLE receiverThread = 0;

    Context context;
    context.keepGoing = true;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
        FAIL("Failed to startup winsocket!");

    SCOPE_GUARD(WSACleanup());

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, defaultPort, &hints, &result);
    if (iResult != 0)
        FAIL("Get AddrInfo failed");

    SCOPE_GUARD(if (result != NULL) freeaddrinfo(result));

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
        FAIL("Could not create listening socket");

    SCOPE_GUARD(closesocket(ListenSocket));

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
        FAIL("bind socket failed");

    freeaddrinfo(result);
    result = NULL;

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
        FAIL("Failed to listen on socket");

    SCOPE_GUARD(closesocket(block.clientSocket));

    HANDLE annoProcess;
    if (!GetProcessByName("Anno1800.exe", &annoProcess))
        FAIL("Could not find anno process");

    char nameBuffer[1024];
    GetFullPathNameA("TomCruise.dll",
        sizeof(nameBuffer),
        nameBuffer,
        NULL);
    if (strlen(nameBuffer) == 0)
        FAIL("Couldn't find 'TomCruise.dll' in the current working directory");

    block.listenSocket = ListenSocket;

    socketWaitThread = CreateThread(NULL, NULL, WaitOnSocketAccept, &block, NULL, NULL);

    Sleep(500);

    if (!InjectDLL(annoProcess, nameBuffer))
        FAIL("Could not inject DLL");

    WaitForSingleObject(socketWaitThread, 10000);

    if (block.clientSocket == INVALID_SOCKET)
        FAIL("Could not connect to socket");

    // No longer need server socket
    closesocket(ListenSocket);

    context.socket = block.clientSocket;

    // receiverThread = CreateThread(NULL, NULL, HandleIncommingMessage, &context, NULL, NULL);

    g_TargetCallSocket = context.socket;

    char ok;
    int ready = recv(g_TargetCallSocket, &ok, 1, 0);

    std::cout << "Done setting up, waiting for scripting..." << std::endl;

    while (true)
    {
        std::cout << "Waiting for new script client..." << std::endl;
        SOCKET scriptingSocket = WaitForScriptingSocket();
        std::cout << "Got new client!" << std::endl;
        while (HandleScriptRequest(scriptingSocket));
        std::cout << "Client exited" << std::endl;
        closesocket(scriptingSocket);
    }

    return 0;
}

