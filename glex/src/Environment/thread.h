#pragma once
#include <synchapi.h>

namespace glex
{
	class Lock
	{
	private:
		CRITICAL_SECTION m_criticalSection;
	public:
		Lock(uint32_t spinCount) { InitializeCriticalSectionAndSpinCount(&m_criticalSection, spinCount); }
		~Lock() { DeleteCriticalSection(&m_criticalSection); }
		void Aquire() { EnterCriticalSection(&m_criticalSection); }
		void Release() { LeaveCriticalSection(&m_criticalSection); }
	};
}