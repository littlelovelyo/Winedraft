#include "res.h"

/* void ResourceManager::Increase(uint32_t handle)
{
	m_refCount[handle]++;
}

bool ResourceManager::Decrease(uint32_t handle)
{
	if (--m_refCount[handle] == 0)
	{
		m_refCount.erase(handle);
		return true;
	}
	return false;
}

/*uint32_t Resource::New()
{
	for (; s_firstAvailable < s_bitmap.size(); s_firstAvailable)
	{
		if (s_bitmap[s_firstAvailable] != 0xffffffffffffffff)
			break;
	}
	if (s_firstAvailable < s_bitmap.size())
	{
		uint64_t& map = s_bitmap[s_firstAvailable];
		uint64_t mask = 0x8000000000000000;
		int innerOffset = 0;
		for (int i = 0; i < 64; i++)
		{
			if ((map & mask) == 0)
				break;
			innerOffset++;
			mask >>= 1;
		}
		map |= mask;
		uint32_t index = s_firstAvailable * 64 + innerOffset;
		if (map == 0xffffffffffffffff)
			s_firstAvailable++;
		while (index >= s_refCount.size())
			s_refCount.push_back(0);
		s_refCount[index] = 1;
		return index;
	}
	else
	{
		s_refCount.push_back(1);
		s_bitmap.push_back(0x8000000000000000);
		return s_firstAvailable * 64;
	}
}

void Resource::Increase(uint32_t index)
{
	s_refCount[index]++;
}

bool Resource::Decrease(uint32_t index)
{
	if (--s_refCount[index] == 0)
	{
		uint32_t outerIndex = index / 64;
		int innerOffset = index % 64;
		s_bitmap[outerIndex] &= ~(0x8000000000000000 >> innerOffset);
		if (s_refCount.back() == 0)
			s_refCount.pop_back();
		if (s_bitmap.back() == 0)
			s_bitmap.pop_back();
		if (outerIndex < s_firstAvailable)
			s_firstAvailable = outerIndex;
		return true;
	}
	else
		return false;
}*/