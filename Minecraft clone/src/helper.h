#pragma once
#include "World/chunk.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include <string.h>

#if 0
template <typename T>
class Quadarray
{
private:
	std::vector<T> m_storage;
	T* m_center = nullptr;
	uint32_t m_sideLength = 0;

public:
	uint32_t RenderDistance() const {
		return (m_sideLength + 1) / 2;
	}

	// Sadly multidimentional subscipt is not supported by MSVC.
	T* operator[](int x)
	{
		return m_center + x * m_sideLength;
	}

	template <typename Op> requires std::invocable<Op, T&>
	void Foreach(Op op)
	{
		std::ranges::for_each(m_storage, [&](T& item) {
			op(item);
		});
	}

	template <typename Op> requires std::invocable<Op, T&, int, int>
	void Foreach(Op op)
	{
		uint32_t i = 0;
		int origin = m_sideLength / 2;
		for (uint32_t x = 0; x < m_sideLength; x++)
		{
			for (uint32_t z = 0; z < m_sideLength; z++)
				op(m_storage[i++], static_cast<int>(x) - origin, static_cast<int>(z) - origin);
		}
	}

	template <typename Op> requires std::invocable<Op, T&, int, int>
	void Expand(uint32_t renderDistance, Op op)
	{
		uint32_t newSideLength = renderDistance * 2 - 1;
		int origin = renderDistance - 1;
		// Move elements from back to front.
		m_storage.resize(newSideLength * newSideLength);
		uint32_t idxOld = m_sideLength * m_sideLength - 1;		// index of last old element
		uint32_t padding = (newSideLength - m_sideLength) / 2;	// as the name suggests
		uint32_t idxNew = padding + m_sideLength - 1;
		idxNew = idxNew * newSideLength + idxNew;				// where the last element should be
		uint32_t step = newSideLength - m_sideLength;			// step between lines
		for (uint32_t x = 0; x < m_sideLength; x++)
		{
			for (uint32_t y = 0; y < m_sideLength; y++)
				m_storage[idxNew--] = std::move(m_storage[idxOld--]);
			idxNew -= step;
		}
		// Then we fill the blanks with new elements. Start with upper side.
		uint32_t i = 0;
		uint32_t x;
		for (x = 0; x < padding; x++)
		{
			for (uint32_t y = 0; y < newSideLength; y++)
				op(m_storage[i++], static_cast<int>(x) - origin, static_cast<int>(y) - origin);
		}
		// Left and right sides.
		uint32_t xstop = x + m_sideLength;
		for (; x < xstop; x++)
		{
			for (uint32_t y = 0; y < padding; y++)
				op(m_storage[i++], static_cast<int>(x) - origin, static_cast<int>(y) - origin);
			i += m_sideLength;
			for (uint32_t y = padding + m_sideLength; y < newSideLength; y++)
				op(m_storage[i++], static_cast<int>(x) - origin, static_cast<int>(y) - origin);
		}
		// Bottom side.
		for (; x < newSideLength; x++)
		{
			for (uint32_t y = 0; y < newSideLength; y++)
				op(m_storage[i++], static_cast<int>(x) - origin, static_cast<int>(y) - origin);
		}
		m_sideLength = newSideLength;
		m_center = m_storage.data() + origin * newSideLength + origin;
	}

	template <typename Op> requires std::invocable<Op, T&, int, int>
	void Shrink(uint32_t renderDistance, Op op)
	{
		uint32_t newSideLength = renderDistance * 2 - 1;
		int origin = m_sideLength / 2;
		uint32_t padding = (m_sideLength - newSideLength) / 2;
		// Delete elements. Start with upper side.
		uint32_t i = 0;
		uint32_t x;
		for (x = 0; x < padding; x++)
		{
			for (uint32_t y = 0; y < m_sideLength; y++)
				op(m_storage[i++], static_cast<int>(x) - origin, static_cast<int>(y) - origin);
		}
		// Left and right sides.
		uint32_t xstop = x + newSideLength;
		for (; x < xstop; x++)
		{
			for (uint32_t y = 0; y < padding; y++)
				op(m_storage[i++], static_cast<int>(x) - origin, static_cast<int>(y) - origin);
			i += newSideLength;
			for (uint32_t y = padding + newSideLength; y < m_sideLength; y++)
				op(m_storage[i++], static_cast<int>(x) - origin, static_cast<int>(y) - origin);
		}
		// Lower side.
		for (; x < m_sideLength; x++)
		{
			for (uint32_t y = 0; y < m_sideLength; y++)
				op(m_storage[i++], static_cast<int>(x) - origin, static_cast<int>(y) - origin);
		}
		// Move elements from front to back.
		uint32_t idxNew = 0;								// where the first element should be
		uint32_t idxOld = padding * m_sideLength + padding;	// index of the first element
		uint32_t step = m_sideLength - newSideLength;		// step between lines
		for (uint32_t x = 0; x < newSideLength; x++)
		{
			for (uint32_t y = 0; y < newSideLength; y++)
				m_storage[idxNew++] = std::move(m_storage[idxOld++]);
			idxOld += step;
		}
		m_storage.resize(newSideLength * newSideLength);
		m_sideLength = newSideLength;
		origin = renderDistance - 1;
		m_center = m_storage.data() + origin * newSideLength + origin;
	}

	template <typename Op1, typename Op2> requires std::invocable<Op1, T&, int, int> &&std::invocable<Op2, T&, int, int>
	void Move(int dx, int dy, Op1 old, Op2 nju)
	{
		int size = static_cast<int>(m_sideLength);
		int origin = m_sideLength / 2;
		// Delete old elements that are out of range.
		int xst = 0, xed = size;
		if (dx > 0)
		{
			uint32_t i = 0;
			xst = std::min(dx, size);
			for (int x = 0; x < xst; x++)
			{
				for (int y = 0; y < size; y++)
					old(m_storage[i++], x - origin, y - origin);
			}
		}
		else if (dx < 0)
		{
			xed = std::max(0, size + dx);
			uint32_t i = xed * m_sideLength;
			for (int x = xed; x < size; x++)
			{
				for (int y = 0; y < size; y++)
					old(m_storage[i++], x - origin, y - origin);
			}
		}
		if (dy > 0)
		{
			int yend = std::min(dy, size);
			uint32_t i = xst * m_sideLength;
			uint32_t step = m_sideLength - yend;
			for (int x = xst; x < xed; x++)
			{
				for (int y = 0; y < yend; y++)
					old(m_storage[i++], x - origin, y - origin);
				i += step;
			}
		}
		else if (dy < 0)
		{
			int ystart = std::max(0, size + dy);
			uint32_t i = xst * m_sideLength + ystart;
			for (int x = xst; x < xed; x++)
			{
				for (int y = ystart; y < size; y++)
					old(m_storage[i++], x - origin, y - origin);
				i += ystart;
			}
		}
		// Move elements.
		if (dy >= size || dy <= -size)
			goto moveend;
		{
			/**
			 * dx = move between lines
			 * dy = move within lines
			 * dy must be clamped so we don't accidentally move across lines
			 */
			int offset = dx * size + dy;
			if (offset > 0)
			{
				// Elements must be moved in an opposite way with respect to offset.
				uint32_t ed = m_sideLength * m_sideLength - offset;
				for (uint32_t i = 0; i < ed; i++)
					m_storage[i] = std::move(m_storage[i + offset]);
			}
			else if (offset < 0)
			{
				offset = -offset;
				for (uint32_t i = m_sideLength * m_sideLength - 1; i >= offset; i--)
					m_storage[i] = std::move(m_storage[i - offset]);
			}
		}
	moveend:
		// Add new elements.
		xst = 0, xed = m_sideLength;
		if (dx > 0)
		{
			xed = std::max(size - dx, 0);
			uint32_t i = xed * m_sideLength;
			for (int x = xed; x < size; x++)
			{
				for (int y = 0; y < size; y++)
					nju(m_storage[i++], x - origin, y - origin);
			}
		}
		else if (dx < 0)
		{
			xst = std::min(size, -dx);
			uint32_t i = 0;
			for (int x = 0; x < xst; x++)
			{
				for (int y = 0; y < size; y++)
					nju(m_storage[i++], x - origin, y - origin);
			}
		}
		if (dy > 0)
		{
			int ystart = std::max(size - dy, 0);
			uint32_t i = xst * m_sideLength + ystart;
			for (int x = xst; x < xed; x++)
			{
				for (int y = ystart; y < size; y++)
					nju(m_storage[i++], x - origin, y - origin);
				i += ystart;
			}
		}
		else if (dy < 0)
		{
			int yend = std::min(size, -dy);
			uint32_t i = xst * m_sideLength;
			uint32_t step = m_sideLength - yend;
			for (int x = xst; x < xed; x++)
			{
				for (int y = 0; y < yend; y++)
					nju(m_storage[i++], x - origin, y - origin);
				i += step;
			}
		}
	}
};
#endif

inline std::pair<int, int> floor_divmod(int a, int b)
{
	int d = a / b;
	int r = a % b;
	if (r != 0 && a < 0)
	{
		d--;
		r += b;
	}
	return { d, r };
}

template<>
class std::hash<glm::ivec3>
{
public:
	std::size_t operator()(glm::ivec3 const& pos) const noexcept
	{
		// 64 bits = 32 bits + 32 bits = 28 bits + 8 bits + 28 bits
		return static_cast<uint64_t>(pos.x) << 36 ^ static_cast<uint64_t>(pos.y) << 28 ^ static_cast<uint64_t>(pos.z);
	};
};

template<>
class std::hash<glm::ivec2>
{
public:
	std::size_t operator()(glm::ivec2 const& pos) const noexcept
	{
		// 64 bits = 32 bits + 32 bits = 28 bits + 8 bits + 28 bits
		return static_cast<uint64_t>(pos.x) << 32 | static_cast<uint64_t>(pos.y);
	};
};

template <typename T>
class VirtualStorage
{
private:
	uint8_t m_storage[sizeof(T)];
public:
	T* operator->() { return reinterpret_cast<T*>(m_storage); }
	VirtualStorage& operator=(T const& another) { memcpy(m_storage, &another, sizeof(T)); return *this; }
};