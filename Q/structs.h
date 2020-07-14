#pragma once

#include <functional>
#include <unordered_map>
#include "windows.h"

#define FAIL(str) { error = str; return false; }

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

class RunOnExit
{
public:
	explicit RunOnExit(std::function<void()> function)
	{
		m_function = function;
		active = true;
	}

	~RunOnExit()
	{
		if (active)
			m_function();
	}

	RunOnExit(RunOnExit&& other) noexcept
	{
		other.active = false;
		m_function = std::move(other.m_function);
		active = true;
	}

private:
	std::function<void()> m_function;
	bool active;
};

struct MemoryRegion
{
    uint64_t baseAddress;
    uint64_t size;
    DWORD protection;
};

struct AssumedFunction
{
    uint64_t offset;
    uint64_t length;
    uint64_t locals;
    uint64_t arguments;
};

enum class MessageType : uint8_t
{
    Exit,
    Print,
    PrintNoLabel,
    RegisterIslandResources,
    PytonExec,
    MoveShip,
};

struct Message
{
    MessageType type;
    uint8_t payload[4096];
};

struct CodePutback
{
    uint64_t address;
    std::vector<uint8_t> code;
    bool timeout = false;
    double lifetime = 0.;
};

struct MemoryAllocation 
{
    uint64_t address;
    bool timeout = false;
    double lifetime = 0.;
};

struct ShipMoveOrder
{
    uint64_t coordinateAddress;
    uint64_t shipListAddress;
    uint64_t endOfShipListAddress;
    std::vector<std::wstring> shipNames;
    std::vector<uint64_t> shipIds;
    std::wstring islandTargetName;
    uint64_t resolvedIslandId;
    bool populatedCoordsForIslandTarget;

    float x;
    float y;
};

struct ShipDumpOrder
{
    std::string name;
    int slotID;
};

struct ShipCargoSetOrder
{
    std::string ShipName;
    std::string islandName;
    uint64_t typeID;
    uint64_t amount;
};

struct InternalShipInfo
{
    uint64_t id;
    std::string name;
};

struct BuildingCache
{
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>> map;
};

#define PP_CAT(a, b) PP_CAT_I(a, b)
#define PP_CAT_I(a, b) PP_CAT_II(~, a ## b)
#define PP_CAT_II(p, res) res

#define UNIQUE_NAME(base) PP_CAT(base, __COUNTER__)


#define SCOPE_GUARD(code) RunOnExit UNIQUE_NAME(roe) ([&] () { code; })
#define SCOPE_LOCK(lock) EnterCriticalSection(lock); SCOPE_GUARD(LeaveCriticalSection(lock));
