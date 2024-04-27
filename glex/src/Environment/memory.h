#pragma once
#include <stdint.h>
#include "comhdr.h"

namespace glex
{
	class StackAllocator
	{
		GLEX_STATIC_CLASS(StackAllocator)
	private:
		inline static uint32_t s_pageSize;
		inline static uint8_t* s_currentPage, *s_stackPointer;
#ifdef _DEBUG
		inline static uint32_t s_sizeAllocated, s_sizeUsed;
#endif
	public:
		static void Init(uint32_t pageSize);
		static void Shutdown();
		static void* Allocate(uint32_t size);
		static void Deallocate();
#ifdef _DEBUG
		static uint32_t SizeAllocated() {
			return s_sizeAllocated;
		}
		static uint32_t SizeUsed() {
			return s_sizeUsed;
		}
#endif
	};
}