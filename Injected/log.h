#pragma once

#include "global_includes.h"

class Log
{
public:
    Log();
    void Initialize();
    void Shutdown();
    static Log& Get();
    void Write(const char* line);
private:
    HANDLE log_file_handle;
};

#define ANNO_LOG(format, ...) { \
    char __my_line_[512]; \
    sprintf_s(__my_line_, format, ##__VA_ARGS__); \
    Log::Get().Write(__my_line_); \
}

#define ANNO_FORMAT(result, format, ...) std::string result; { \
    char __my_line_[512]; \
    sprintf_s(__my_line_, format, ##__VA_ARGS__); \
    result = __my_line_; \
}

#define DEBUG_PTR(variable) ANNO_LOG(#variable " %llx", variable)

