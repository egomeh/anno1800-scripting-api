#pragma once

#include <functional>
#include "global_includes.h"

template<typename T>
class ScopedAllocator
{
public:
    ScopedAllocator(size_t amount)
    {
        m_data = new T[amount];
    }

    ScopedAllocator(T* data)
    {
        m_data = data;
    }

    ~ScopedAllocator()
    {
        delete[] m_data;
    }

    T* data()
    {
        return m_data;
    }

private:
    T* m_data;
};

std::string utf8_encode(const std::wstring& wstr);
std::wstring utf8_decode(const std::string& str);


bool GetProcessByName(const char* processName, HANDLE* processHandle);
bool GetProcessModules(const HANDLE& processHandle, HMODULE** moduleList, uint64_t& numberOfModules);


bool EndsWith(const char* string, const char* check);
bool FindModule(HANDLE process, HMODULE* moduleList, uint64_t numberOfModules, const char* nameEnd, HMODULE& result);


bool IsReadable(void* address, size_t byteCount);


class RunOnExit
{
public:
    RunOnExit()
    {
        m_function = nullptr;
    }

    ~RunOnExit()
    {
        if (m_function)
            m_function();
    }

    RunOnExit(RunOnExit&& other) noexcept
    {
        m_function = std::move(other.m_function);
        other.m_function = nullptr;
    }

    std::function<void()> m_function;
};


#define PP_CAT(a, b) PP_CAT_I(a, b)
#define PP_CAT_I(a, b) PP_CAT_II(~, a ## b)
#define PP_CAT_II(p, res) res

#define DIVE_UNIQUE_NAME(base) PP_CAT(base, __LINE__)

#define ON_EXIT RunOnExit DIVE_UNIQUE_NAME(roe); DIVE_UNIQUE_NAME(roe).m_function = [&] ()

#define FOUR_BYTES(value) (uint8_t)(value >> 0), (uint8_t)(value >> 8), (uint8_t)(value >> 16), (uint8_t)(value >> 24)
#define EIGHT_BYTES(value) (uint8_t)(value >> 0), (uint8_t)(value >> 8), (uint8_t)(value >> 16), (uint8_t)(value >> 24), \
                           (uint8_t)(value >> 32), (uint8_t)(value >> 40), (uint8_t)(value >> 48), (uint8_t)(value >> 56)
