#include "chunkStorage.h"
#include "World/world.h"

void ChunkStorage::CheckCacheCap()
{
	s_lock.Aquire();
	while (s_unloadedList.size() > s_cacheCount)
	{
		Chunk* chunk = s_unloadedList.back();
		s_unloadedList.pop_back();
		chunk->World()->FreeChunk(chunk);
		Slave::UnloadChunkLater(chunk);
	}
	s_lock.Release();
}

void ChunkStorage::SetCacheCount(uint32_t count)
{
	s_cacheCount = count;
	CheckCacheCap();
}

Chunk* ChunkStorage::Get(glm::ivec2 const& pos)
{
	Chunk* ret;
	s_lock.Aquire();
	auto iter = m_storage.find(pos);
	ret = iter == m_storage.end() ? nullptr : iter->second.first;
	s_lock.Release();
	return ret;
}

Chunk* ChunkStorage::LoadChunk(World* world, glm::ivec2 const& pos)
{
	s_lock.Aquire();
	auto& [chunk, iter] = m_storage[pos];
	s_lock.Release();
	if (chunk == nullptr)
	{
		Chunk* chk = Slave::NewChunk();
		chk->Reset(world, pos);
		Slave::LoadChunkLater(chk);
		chunk = chk;
		iter = s_unloadedList.end();
	}
	else if (chunk->Unloaded())
	{
		chunk->Unloaded(false);
		if (!chunk->Loaded())
			Slave::LoadChunkLater(chunk);
		s_unloadedList.erase(iter);
		iter = s_unloadedList.end();
	}
	return chunk;
}

void ChunkStorage::UnloadChunk(glm::ivec2 const& pos)
{
	s_lock.Aquire();
	auto find = m_storage.find(pos);
	if (find != m_storage.end())
	{
		auto& [chunk, iter] = find->second;
		s_lock.Release();
		if (iter == s_unloadedList.end())
		{
			chunk->Unloaded(true);
			s_unloadedList.push_front(chunk);
			iter = s_unloadedList.begin();
			CheckCacheCap();
		}
	}
	else
		s_lock.Release();
}

void ChunkStorage::Save()
{
	for (auto& [key, val] : m_storage)
	{
		val.first->Unloaded(true);
		Slave::UnloadChunkLater(val.first);
	}
	m_storage.clear();
	s_unloadedList.clear();
}