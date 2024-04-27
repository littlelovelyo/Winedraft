#include "memory.h"

using namespace glex;

void StackAllocator::Init(uint32_t pageSize)
{
	static_assert(sizeof(uint8_t*) == 8, "Glex only supports x64 environments.");
	GLEX_ASSERT(pageSize != 0 && (pageSize & 0xffffffc0) == pageSize, "Page size of stack allocator must be a multiple of 64.") {}
	s_pageSize = pageSize;
	s_currentPage = reinterpret_cast<uint8_t*>(operator new(s_pageSize));
	*reinterpret_cast<uint8_t**>(s_currentPage) = nullptr; // prev
	*reinterpret_cast<uint8_t**>(s_currentPage + 8) = nullptr; // next
	s_stackPointer = s_currentPage + 16;
#ifdef _DEBUG
	s_sizeAllocated = s_pageSize;
	s_sizeUsed = 16;
#endif
}

void StackAllocator::Shutdown()
{
#ifdef _DEBUG
	if (*reinterpret_cast<uint8_t**>(s_currentPage) != nullptr || s_stackPointer != s_currentPage + 16)
		std::cout << GLEX_LOG_WARNING "Warning from memory manager: stack allocated memory leaked.\n";
#endif
	for (uint8_t* p = *reinterpret_cast<uint8_t**>(s_currentPage + 8); p != nullptr;)
	{
		uint8_t* next = *reinterpret_cast<uint8_t**>(p + 8);
		operator delete(p);
		p = next;
	}
	for (uint8_t* p = *reinterpret_cast<uint8_t**>(s_currentPage); p != nullptr;)
	{
		uint8_t* prev = *reinterpret_cast<uint8_t**>(p);
		operator delete(p);
		p = prev;
	}
	operator delete(s_currentPage);
}

void* StackAllocator::Allocate(uint32_t size)
{
	GLEX_ASSERT(size < s_pageSize - 24, "Size per each allocation cannot be larger than pageSize - 24.") {}
	if (size + 8 > s_pageSize - (s_stackPointer - s_currentPage))
	{
		uint8_t*& next = *reinterpret_cast<uint8_t**>(s_currentPage + 8);
		if (next == nullptr)
		{
			next = reinterpret_cast<uint8_t*>(operator new(s_pageSize));
			if (next == nullptr)
				return nullptr;
			*reinterpret_cast<uint8_t**>(next) = s_currentPage; // prev
			*reinterpret_cast<uint8_t**>(next + 8) = nullptr; // next
#ifdef _DEBUG
			s_sizeAllocated += s_pageSize;
			s_sizeUsed += 16;
#endif
		}
		*reinterpret_cast<uint64_t*>(s_currentPage + s_pageSize - 8) = *reinterpret_cast<uint64_t*>(s_stackPointer - 8) + (s_pageSize - (s_stackPointer - s_currentPage));
		s_currentPage = next;
		s_stackPointer = next + 16;
	}
	void* addr = s_stackPointer;
	s_stackPointer += size;
	*reinterpret_cast<uint64_t*>(s_stackPointer) = size;
	s_stackPointer += 8;
#ifdef _DEBUG
	s_sizeUsed += size + 8;
#endif
	return addr;
}

void StackAllocator::Deallocate()
{
	if (s_stackPointer == s_currentPage + 16)
	{
		s_currentPage = *reinterpret_cast<uint8_t**>(s_currentPage); // prev
		GLEX_ASSERT(s_currentPage != nullptr, "Error from stack allocator: stack is already empty.") {}
		s_stackPointer = s_currentPage + s_pageSize;
	}
	s_stackPointer -= 8;
	uint64_t size = *reinterpret_cast<uint64_t*>(s_stackPointer);
	s_stackPointer -= size;
#ifdef _DEBUG
	s_sizeUsed -= size + 8;
#endif
}