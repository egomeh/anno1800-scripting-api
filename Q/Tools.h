#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <tlhelp32.h>
#include <inttypes.h>
#include <shlwapi.h>
#include <stdio.h>
#include <psapi.h>
#include <vector>

#include "autocomms.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define FATAL_ERROR(messge) { MessageBoxA(0, messge, "Error", MB_OK); exit(-1); }

#include <stdint.h>
#include <string>

struct MemoryRegion;
struct AssumedFunction;
struct Message;

// From: https://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte
std::string utf8_encode(const std::wstring& wstr);
std::wstring utf8_decode(const std::string& str);

bool GetProcessByName(const char* processName, HANDLE* processHandle);
bool GetProcessModules(const HANDLE& processHandle, HMODULE** moduleList, uint64_t& numberOfModules);
bool EndsWith(const char* string, const char* check);
bool FindModule(HANDLE process, HMODULE* moduleList, uint64_t numberOfModules, const char* nameEnd, HMODULE& result);
bool GetProcessName(HANDLE process, wchar_t **name);
bool IsExecutable(DWORD protection);
bool GetExecutableRegions(HANDLE process, uint64_t regionBegin, uint64_t regionEnd, MemoryRegion** regions, uint64_t& nRegions);
bool GetThreadList(DWORD process);
bool IsReadable(void* address, size_t byteCount);
bool IsWriteable(void* address, size_t byteCount);
bool IsReadablePointer(uint64_t address);
uint64_t ReadU64(uint64_t address);
uint32_t ReadU32(uint64_t address);
float ReadF32(uint64_t address);

std::string resourceIDToString(uint32_t id);
uint32_t stringToResourceID(const char* name);

bool GetAssumedFunctions(const char* filename, const char* section, AssumedFunction** functions, uint64_t &entries);

bool WriteToPipeBlocking(HANDLE pipe, const char* message);

bool HackSendMessage(SOCKET socket, Message* message);
bool HackReceiveMessage(SOCKET socket, Message *message);

uint64_t FindAddressByPointerList(const uint64_t* addresses, uint32_t n, const uint64_t address, bool followFirst, bool followLast);

bool PlotAsterisk(size_t width, size_t height, float* data, std::vector<std::string>& output);

bool shipname(const char*& string, std::string& name);
bool number(const char*& string, float& value);
bool integer(const char*& string, int& value);
bool ParseMoveOrder(const char* string, std::vector<std::string>& name, std::string& islandName, float& x, float& y);

bool IsTradingPost(uint64_t);

SOCKET WaitForScriptingSocket();

bool ReadMessageFromSocket(SOCKET socket, std::vector<uint8_t>& buffer);


uint64_t AreaToAreaID(AutoComms::Area area);
uint64_t ResourceToID(AutoComms::Resource resource);

AutoComms::ShipType IDToShipType(uint64_t shipTypeId);

AutoComms::Resource IDToResourceType(uint64_t resourceId);

AutoComms::Building IDToBuildingType(uint64_t buildingID);
