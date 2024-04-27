#pragma once
#include "helper.h"
#include "World/chunk.h"
#include <unordered_map>
#include <list>

class ChunkStorage
{
private:
	inline static glex::Lock s_lock { 127 };
	inline static uint32_t s_cacheCount;
	inline static std::list<Chunk*> s_unloadedList;
	std::unordered_map<glm::ivec2, std::pair<Chunk*, std::list<Chunk*>::iterator>> m_storage;
private:
	static void CheckCacheCap();
public:
	static void SetCacheCount(uint32_t count);
	Chunk*& operator[](glm::ivec2 const& pos) { return m_storage[pos].first; }
	Chunk* Get(glm::ivec2 const& pos);
	Chunk* LoadChunk(World* world, glm::ivec2 const& pos);
	void UnloadChunk(glm::ivec2 const& pos);
	void FreeChunk(Chunk* chunk) { m_storage.erase(chunk->Position()); }
	void Save();
};