#include "pch.h"
#include "globals.h"

bool KeepGoing = true;
HANDLE g_logFile = NULL;

void SendMessageToHost(const char* message)
{
    DWORD bytesWritten = 0;
    WriteFile(
        g_logFile,           // open file handle
        message,      // start of data to write
        strlen(message),  // number of bytes to write
        &bytesWritten, // number of bytes that were written
        NULL);            // no overlapped structure

    char newline[] = "\n";
    WriteFile(
        g_logFile,           // open file handle
        newline,      // start of data to write
        1,  // number of bytes to write
        &bytesWritten, // number of bytes that were written
        NULL);            // no overlapped structure
}

void SendMessageToHostNoLabel(const char* message)
{
    DWORD bytesWritten = 0;
    WriteFile(
        g_logFile,           // open file handle
        message,      // start of data to write
        (DWORD)strlen(message),  // number of bytes to write
        &bytesWritten, // number of bytes that were written
        NULL);            // no overlapped structure

    char newline[] = "\n";
    WriteFile(
        g_logFile,           // open file handle
        newline,      // start of data to write
        1,  // number of bytes to write
        &bytesWritten, // number of bytes that were written
        NULL);            // no overlapped structure
}

uint64_t ComponentIdName = 289;
uint64_t ComponentIdCargo = 618;
uint64_t ComponentIdMoveData = 268;

uint64_t ComponentIdBuildingProduction = 613;
uint64_t ComponentIdBuildingProductionNoOuput = 609;
