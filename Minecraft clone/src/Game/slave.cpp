#include "slave.h"
#include "Game/game.h"
#include "World/world.h"
#include "World/chunkRenderer.h"
#include "Entity/player.h"

DWORD Slave::DoWork(LPVOID param)
{
	t_threadIdentifier = ThreadIdentifier::Slave;
	/**
	 * s_goOn
	 * 2 - do your work
	 * 1 - prepare to shutdown, one more loop
	 * 0 - done
	 */
	while (s_goOn != 0)
	{
		// should be in reverse order to prevent deadlock
		WaitForSingleObject(s_hasWorkEvent, INFINITE);
		if (s_goOn == 1)
			s_goOn = 0;

		// Order must be: Unload - Load - Light propagation - Mesh generation - Delete
		uint32_t size = s_chunksToUnload.size();
		for (uint32_t i = 0; i < size; i++)
		{
			s_lock.Aquire();
			Chunk* chunk = s_chunksToUnload.front();
			s_chunksToUnload.pop();
			/**
			 * Slave::s_chunksToUnload queue is filled by main thread,
			 * so chunk state can't be ChunkState::PerformingOperation or something like that set by main thread.
			 * Operation must be done before Slave::UnloadChunkLater() is called.
			 * Chunk state can't be ChunkState::PerformingOperation set by slave thread either
			 * because operation hasn't started yet.
			 * So it's safe to set chunk state to ChunkState::Unloaded?
			 * What if chunk is unloaded and loaded back again right away?
			 * No, it's still safe because the chunk pointer will be a new one in that case.
			 * We first set chunk state to ChunkState::Unloaded, then the operation following will fail,
			 * only after that can we safely delete the chunk.
			 *
			 * Maybe we should cache unloaded chunks somewhere in case it's loaded back again?
			 * We can cache unloaded chunks but the pointer returned by Slave::NewChunk() must be a different one.
			 */
			s_chunksToDelete.push(chunk);
			s_lock.Release();
		}

		size = s_chunksToLoad.size();
		for (uint32_t i = 0; i < size; i++)
		{
			s_lock.Aquire();
			Chunk* chunk = s_chunksToLoad.front();
			s_chunksToLoad.pop();
			s_lock.Release();
			if (chunk->Unloaded())
				continue;
			if (!chunk->Loaded())
				chunk->Load();
			if (!chunk->LightPopulated())
				chunk->PopulateLight();
			if (chunk->LightPopulated())
				Player::GenerateMeshIfVisible(chunk->Position());
			else
			{
				s_lock.Aquire();
				s_chunksToLoad.push(chunk);
				s_lock.Release();
			}
		}

		size = s_posesToRecalculateLight.size();
		for (uint32_t i = 0; i < size; i++)
		{
			s_lock.Aquire();
			auto [world, pos, sky, block] = s_posesToRecalculateLight.front();
			s_posesToRecalculateLight.pop();
			s_lock.Release();
		}

		// Needs some proper fix.
		for (uint32_t i = 0; ; i++)
		{
			s_lock.Aquire();
			if (i >= s_chunksToGenerateMesh.size())
			{
				s_lock.Release();
				break;
			}
			ChunkRenderer* chunk = s_chunksToGenerateMesh[i];
			s_chunkIndices[s_chunksToGenerateMesh[i] = s_chunksToGenerateMesh.back()] = i;
			s_chunksToGenerateMesh.pop_back();
			s_chunkIndices.erase(chunk);
			s_chunkGeneratingMesh = chunk;
			s_lock.Release();
			if (!chunk->Build())
			{
				s_lock.Aquire();
				if (!MeshGenerationCanceled() && !s_chunkIndices.contains(chunk))
				{
					s_chunkIndices[chunk] = s_chunksToGenerateMesh.size();
					s_chunksToGenerateMesh.push_back(chunk);
				}
				s_lock.Release();
			}
		}

		// Now it's safe to delete.
		size = s_chunksToDelete.size();
		for (uint32_t i = 0; i < size; i++)
		{
			s_lock.Aquire();
			Chunk* chunk = s_chunksToDelete.top();
			s_chunksToDelete.pop();
			if (s_freeChunks.size() < s_freeChunksCap)
			{
				s_freeChunks.push(chunk);
				chunk = nullptr;
			}
			s_lock.Release();
			if (chunk != nullptr)
				delete chunk;
		}
	}
	return 0;
}

void Slave::Startup()
{
	// 37 is somewhat arbitrary
	s_freeChunksCap = 4 * (Config::RenderDistance() * 2 - 1);
	t_threadIdentifier = ThreadIdentifier::Main;
	s_hasWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // auto-reset event
	s_hThread = CreateThread(NULL, NULL, DoWork, NULL, CREATE_SUSPENDED, NULL);
	if (s_hasWorkEvent == NULL || s_hThread == NULL)
		throw glex::Exception("Cannot create working thread.");
	SetThreadPriority(s_hThread, THREAD_PRIORITY_BELOW_NORMAL);
	ResumeThread(s_hThread);
}

void Slave::Shutdown()
{
	s_freeChunksCap = 0;
	s_goOn = 1;
	s_lock.Aquire();
	s_chunksToGenerateMesh.clear();
	while (!s_freeChunks.empty())
	{
		delete s_freeChunks.top();
		s_freeChunks.pop();
	}
	s_freeChunksCap = 0;
	s_lock.Release();
	SetEvent(s_hasWorkEvent);
}

void Slave::WaitForExiting()
{
	WaitForSingleObject(s_hThread, INFINITE);
	CloseHandle(s_hThread);
	CloseHandle(s_hasWorkEvent);
}

void Slave::OnSetRenderDistance()
{
	s_freeChunksCap = 4 * (Config::RenderDistance() * 2 - 1);
	bool awake = false;
	s_lock.Aquire();
	while (s_freeChunks.size() > s_freeChunksCap)
	{
		Chunk* chunk = s_freeChunks.top();
		s_freeChunks.pop();
		s_chunksToDelete.push(chunk);
		awake = true;
	}
	s_lock.Release();
	if (awake)
		SetEvent(s_hasWorkEvent);
}

void Slave::LoadChunkLater(Chunk* chunk)
{
	s_lock.Aquire();
	s_chunksToLoad.push(chunk);
	s_lock.Release();
	SetEvent(s_hasWorkEvent);
}

void Slave::UnloadChunkLater(Chunk* chunk)
{
	s_lock.Aquire();
	s_chunksToUnload.push(chunk);
	s_lock.Release();
	SetEvent(s_hasWorkEvent);
}

void Slave::CalculateLightLater(World* world, std::vector<std::tuple<glm::ivec3, bool, bool>>& poses)
{
	s_lock.Aquire();
	for (auto& [pos, sky, block] : poses)
		s_posesToRecalculateLight.emplace(world, pos, sky, block);
	s_lock.Release();
	SetEvent(s_hasWorkEvent);
	poses.clear();
}

void Slave::GenerateMeshLater(ChunkRenderer* chunk)
{
	s_lock.Aquire();
	if (!s_chunkIndices.contains(chunk))
	{
		s_chunkIndices[chunk] = s_chunksToGenerateMesh.size();
		s_chunksToGenerateMesh.push_back(chunk);
	}
	s_lock.Release();
	SetEvent(s_hasWorkEvent);
}

void Slave::StopGeneratingMesh(ChunkRenderer* chunk)
{
	s_lock.Aquire();
	if (s_chunkIndices.contains(chunk))
	{
		uint32_t index = s_chunkIndices[chunk];
		s_chunkIndices[s_chunksToGenerateMesh[index] = s_chunksToGenerateMesh.back()] = index;
		s_chunksToGenerateMesh.pop_back();
		s_chunkIndices.erase(chunk);
	}
	s_lock.Release();
}

Chunk* Slave::NewChunk()
{
	Chunk* chunk;
	s_lock.Aquire();
	if (s_freeChunks.empty())
		chunk = nullptr;
	else
	{
		chunk = s_freeChunks.top();
		s_freeChunks.pop();
	}
	s_lock.Release();
	if (chunk == nullptr)
		chunk = new Chunk;
	return chunk;
}