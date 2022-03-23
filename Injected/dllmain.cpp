#include "global_includes.h"
#include <string>
#include "structs.gen.h"
#include "serialization.gen.h"
#include "testing.h"
#include "injected.h"
#include "process.h"
#include "log.h"

void entry(HMODULE module)
{
    Log::Get().Initialize();

    char filename[1024];
    GetModuleFileNameA(NULL, filename, sizeof(filename));

    size_t fullSize = strlen(filename);

    size_t lastSeperator = 0;
    size_t exeStart = 0;
    for (size_t i = 0; i < fullSize; ++i)
    {
        if (filename[i] == '\\' || filename[i] == '/')
            lastSeperator = i;

        if (i <= (fullSize - 4) && memcmp((const void*)&filename[i], ".exe", 4) == 0)
        {
            exeStart = i;
        }
    }

    std::string mainModuleName = std::string(&filename[lastSeperator + 1], exeStart - lastSeperator - 1);

    // If we're loaded in the test program, just call the test code
    if (mainModuleName.compare("TestMonocle") == 0)
        testing();
    else
        injected();

    ANNO_LOG("Injected leaveing");
    Log::Get().Shutdown();
    FreeLibraryAndExitThread(module, 0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&entry, (LPVOID)hModule, NULL, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

