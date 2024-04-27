#pragma once
#include "comhdr.h"
#include <unordered_map>

/* class ResourceManager
{
private:
	std::unordered_map<uint32_t, uint32_t> m_refCount;
public:
	void Increase(uint32_t handle);
	bool Decrease(uint32_t handle); // true if count is zero
	bool Empty() const { return m_refCount.empty(); }
};

class Resources
{
	STATIC_CLASS(Resources)
public:
	// inline static ResourceManager s_vaoList, s_vboList, s_eboList, s_uboList, s_fboList;
	// inline static ResourceManager s_shaderList, s_textureList;
};

/*class Resource
{
	STATIC_CLASS(Resource)
private:
	inline static std::vector<uint32_t> s_refCount;
	inline static std::vector<uint64_t> s_bitmap;
	inline static uint32_t s_firstAvailable;
public:
	static uint32_t New();
	static void Increase(uint32_t index);
	static bool Decrease(uint32_t index);
#ifdef _DEBUG
	static bool Empty() { return s_refCount.empty(); }
#endif
};*/