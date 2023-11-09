#pragma once

#include <windows.h>
#include <tlhelp32.h>

class HardwareBreakpoint
{
public:
	enum class Condition : unsigned char
	{
		Execute = 0,
		Write = 1,
		ReadWrite = 3
	};


	HardwareBreakpoint() : m_index(-1)
	{
	}

	~HardwareBreakpoint()
	{
		Clear();
	}

	bool Set(void* address, int size, Condition condition)
	{
		// This breakpoint is already set
		if (m_index != -1)
			return false;

		unsigned char sizeBytePattern = 0;
		switch (size)
		{
		case 1: sizeBytePattern = 0b00; break;
		case 2: sizeBytePattern = 0b01; break;
		case 4: sizeBytePattern = 0b11; break;
		case 8: sizeBytePattern = 0b10; break;
		default: return false; // Invalid range to watch
		}

		HANDLE currentThread = GetCurrentThread();
		CONTEXT currentContext = {};
		currentContext.ContextFlags |= CONTEXT_DEBUG_REGISTERS;

		if (!GetThreadContext(currentThread, &currentContext))
			return false;

		// find an unused debug register
		for (int i = 0; i < 4; ++i)
		{
			int mask = 0b11 << (i * 2);

			if ((currentContext.Dr7 & mask) > 0)
				continue;

			m_index = i;
			break;
		}

		// no available register :(
		if (m_index == -1)
			return false;

		ForEachThreadInProcess([&](HANDLE thread)
			{
				CONTEXT context = {};
				context.ContextFlags |= CONTEXT_DEBUG_REGISTERS;

				if (!GetThreadContext(thread, &context))
					return;

				ClearBitsForIndex(context.Dr7);

				// set local bp bit
				context.Dr7 |= (DWORD64)1 << (m_index * 2);

				// set the condition
				context.Dr7 |= (DWORD64)((int)condition & 0b11) << (16 + m_index * 4);

				// set the size
				context.Dr7 |= (DWORD64)sizeBytePattern << (18 + m_index * 4);

				// set the address register
				*((DWORD64*)&context.Dr0 + m_index) = (DWORD64)address;

				SetThreadContext(thread, &context);
			});

		return true;
	}

	void Clear()
	{
		if (m_index == -1)
			return;

		ForEachThreadInProcess([&](HANDLE thread)
			{
				CONTEXT context = {};
				context.ContextFlags |= CONTEXT_DEBUG_REGISTERS;

				if (!GetThreadContext(thread, &context))
					return;

				ClearBitsForIndex(context.Dr7);

				SetThreadContext(thread, &context);
			});

		m_index = -1;
	}

	void ClearBitsForIndex(DWORD64& Dr7)
	{
		// reset local and global bits
		Dr7 &= ~(((DWORD64)-1) & (0b11L << (m_index * 2)));

		// reset size and condition
		Dr7 &= ~(((DWORD64)-1) & (0b1111 << (16 + m_index * 4)));
	}

private:

	template<typename T>
	bool ForEachThreadInProcess(T&& function)
	{
		DWORD currentProcessID = GetCurrentProcessId();

		HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

		if (snapshotHandle == INVALID_HANDLE_VALUE)
			return false;

		THREADENTRY32 threadEntry;
		threadEntry.dwSize = sizeof(threadEntry);

		int counter = 0;

		if (Thread32First(snapshotHandle, &threadEntry))
		{
			do
			{
				if (threadEntry.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(threadEntry.th32OwnerProcessID))
				{
					if (threadEntry.th32OwnerProcessID == currentProcessID)
					{
						HANDLE thread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
						if (thread != NULL)
						{
							function(thread);
						}
					}
				}
				threadEntry.dwSize = sizeof(threadEntry);
			} while (Thread32Next(snapshotHandle, &threadEntry));
		}

		CloseHandle(snapshotHandle);
		return true;
	}

	int m_index;
};