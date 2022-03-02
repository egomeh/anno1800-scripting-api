

#include "socket.h"
#include "global_includes.h"
#include "remote_call_handler_base.gen.h"
#include "serialization.gen.h"

bool SocketHandler::Initialize()
{
	if (!CheckIfWinSocketIsInitialized())
		InitializeWinSocket();

	constexpr const char* defaultPort = "4050";

	SOCKET temporary_socket = INVALID_SOCKET;
	addrinfo* result = NULL, *ptr = NULL, hints;
	int i_result;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	i_result = getaddrinfo("localhost", defaultPort, &hints, &result);
	if (i_result != 0)
		return false;

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

	    // Create a SOCKET for connecting to server
	    temporary_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (temporary_socket == INVALID_SOCKET)
			return false;

	    // Connect to server.
	    i_result = connect(temporary_socket, ptr->ai_addr, (int)ptr->ai_addrlen);

	    if (i_result == SOCKET_ERROR)
	    {
	        closesocket(temporary_socket);
	        temporary_socket = INVALID_SOCKET;
	        continue;
	    }
	    break;
	}

	freeaddrinfo(result);
	result = NULL;

	if (temporary_socket == INVALID_SOCKET)
		return false;

	m_socket = temporary_socket;

	return true;
}

bool SocketHandler::Receive(std::vector<uint8_t>& buffer)
{
	buffer.clear();
	buffer.resize(1024);

	int bytes_read = recv(m_socket, (char*)buffer.data(), static_cast<int>(buffer.size()), 0);

	if (bytes_read == SOCKET_ERROR)
		return false;

	buffer.resize(bytes_read);

	uint64_t in_size = 0;
	size_t offset = 0;

	if (!Deserialize(&in_size, buffer, &offset))
		return false;

	while (buffer.size() < in_size)
	{
		std::vector<uint8_t> tempBuffer(0x100);

		int extrabytes = recv(m_socket, (char*)tempBuffer.data(), static_cast<int>(tempBuffer.size()), 0);

		if (extrabytes == SOCKET_ERROR)
			return false;

		tempBuffer.resize(extrabytes);

		buffer.insert(buffer.end(), tempBuffer.begin(), tempBuffer.end());
	}

	buffer.erase(buffer.begin(), buffer.begin() + sizeof(size_t));

	return true;
}

bool SocketHandler::Send(std::vector<uint8_t>& buffer)
{
	std::vector<uint8_t> finalBuffer;
	uint64_t totalSize = buffer.size() + 8;

	if (!Serialize(totalSize, finalBuffer))
		return false;

	// Now finalBuffer is the size of the buffer followed by the payload
	finalBuffer.insert(finalBuffer.end(), buffer.begin(), buffer.end());

	int bytesSent = send(m_socket, (char*)finalBuffer.data(), (int)finalBuffer.size(), 0);

	return bytesSent == totalSize;
}

bool SocketHandler::CheckIfWinSocketIsInitialized()
{
	SOCKET temporary_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (temporary_socket == INVALID_SOCKET && WSAGetLastError() == WSANOTINITIALISED)
	{
		return false;
	}

	closesocket(temporary_socket);
	return true;
}

bool SocketHandler::InitializeWinSocket()
{
	WSADATA wsa_data;
	DWORD result = WSAStartup(MAKEWORD(2, 2), &wsa_data);

	if (!result)
		return false;

	return true;
}
