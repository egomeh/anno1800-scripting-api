#include "tools.h"

std::string utf8_encode(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring utf8_decode(const std::string& str)
{
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}


bool GetProcessByName(const char* processName, HANDLE* processHandle)
{
    *processHandle = nullptr;

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            std::wstring processNameW = utf8_decode(processName);

            if (wcscmp(entry.szExeFile, processNameW.c_str()) == 0)
            {
                HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                if (handle == NULL)
                    return false;

                *processHandle = handle;

                return true;
            }
        }
    }

    return false;
}

bool GetProcessModules(const HANDLE& processHandle, HMODULE** moduleList, uint64_t& numberOfModules)
{
    if (processHandle == NULL)
        return false;

    numberOfModules = 0;

    auto modulesAllocator = ScopedAllocator<HMODULE>(1024);
    HMODULE* modules = modulesAllocator.data();

    DWORD neededBytes = 0;

    if (!EnumProcessModules(processHandle, modules, sizeof(modules), &neededBytes))
        return false;

    numberOfModules = neededBytes / sizeof(HMODULE);

    *moduleList = new HMODULE[numberOfModules];

    for (uint64_t i = 0; i < numberOfModules; ++i)
    {
        (*moduleList)[i] = modules[i];
    }

    return true;
}

bool EndsWith(const char* string, const char* check)
{
    size_t stringLength = strlen(string);
    size_t checkLength = strlen(check);

    if (stringLength < checkLength)
        return false;

    for (size_t i = 0; i < checkLength; ++i)
    {
        if (string[stringLength - i - 1] != check[checkLength - i - 1])
            return false;
    }

    return true;
}


bool FindModule(HANDLE process, HMODULE* moduleList, uint64_t numberOfModules, const char* nameEnd, HMODULE& result)
{
    for (int i = 0; i < numberOfModules; ++i)
    {
        char nameBuffer[4096];
        if (!GetModuleFileNameExA(process, moduleList[i], nameBuffer, sizeof(nameBuffer)))
            return false;

        if (EndsWith(nameBuffer, nameEnd))
        {
            result = moduleList[i];
            return true;
        }
    }

    return false;
}
