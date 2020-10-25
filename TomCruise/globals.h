#pragma once

#include <windows.h>
#include <stdio.h>

#define ALLOW_FORMAT_SNED
#ifdef ALLOW_FORMAT_SNED
#define SEND_FORMATTED(format, ...) { \
    char __my_line_[512]; \
    sprintf_s(__my_line_, format, ##__VA_ARGS__); \
    SendMessageToHost(__my_line_); \
}
#define FAIL_MESSAGE(format, ...) { \
    char __my_line_[512]; \
    sprintf_s(__my_line_, format, ##__VA_ARGS__); \
    SendMessageToHost(__my_line_); \
    return false; \
}
#else
#define SEND_FORMATTED(format, ...)
#define FAIL_MESSAGE { return false; }
#endif

extern bool KeepGoing;
extern HANDLE g_logFile;

void SendMessageToHost(const char* message);
void SendMessageToHostNoLabel(const char* message);

extern uint64_t ComponentIdName;
extern uint64_t ComponentIdCargo;
extern uint64_t ComponentIdMoveData;

extern uint64_t ComponentIdBuildingProduction;
extern uint64_t ComponentIdBuildingProductionNoOuput;

