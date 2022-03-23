#include "log.h"

Log::Log()
{
}

void Log::Initialize()
{
    char logFilePath[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, logFilePath);
    std::string strLogPath = logFilePath;
    strLogPath.append("\\anno_script_log.txt");

    log_file_handle = CreateFileA(strLogPath.c_str(), // name of the write
        GENERIC_WRITE,          // open for writing
        FILE_SHARE_READ,        // Allow others to read while open
        NULL,                   // default security
        CREATE_ALWAYS,          // create new file only
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL);                  // no attr. template
}

void Log::Shutdown()
{
    CloseHandle(log_file_handle);
}

Log& Log::Get()
{
    static Log hook_manager;
    return hook_manager;
}

void Log::Write(const char* line)
{
    DWORD bytesWritten = 0;
    WriteFile(
        log_file_handle,            // open file handle
        line,                       // start of data to write
        (DWORD)strlen(line),        // number of bytes to write
        &bytesWritten,              // number of bytes that were written
        NULL);                      // no overlapped structure

    char newline[] = "\n";
    WriteFile(
        log_file_handle,            // open file handle
        newline,                    // start of data to write
        1,                          // number of bytes to write
        &bytesWritten,              // number of bytes that were written
        NULL);                      // no overlapped structure
}
