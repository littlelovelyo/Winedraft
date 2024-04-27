#include "chunk.h"
#include "world.h"
#include "Entity/player.h"
#ifdef _DEBUG
#include <iostream>
#include <iomanip>
#endif

bool ChunkGroup::Loaded() const
{
	if (m_chunks[4] == nullptr)
		return false;
	for (Chunk* chunk : m_chunks)
	{
		if (chunk == nullptr)
			continue;
		if (!chunk->Loaded())
			return false;
	}
	return true;
}

bool ChunkGroup::Modified() const
{
	for (Chunk* chunk : m_chunks)
	{
		if (chunk != nullptr && chunk->Modified())
			return true;
	}
	return false;
}

void ChunkGroup::ClearDirtyFlag()
{
	for (Chunk* chunk : m_chunks)
	{
		if (chunk != nullptr)
			chunk->Modified(false);
	}
}

Block& ChunkGroup::GetBlock(glm::ivec3 const& pos)
{
	auto [cx, lx] = div(pos.x + Config::ChunkSize, static_cast<int>(Config::ChunkSize));
	auto [cz, lz] = div(pos.z + Config::ChunkSize, static_cast<int>(Config::ChunkSize));
	return m_chunks[cz * 3 + cx]->GetBlock(glm::ivec3(lx, pos.y, lz));
}

Light& ChunkGroup::GetLight(glm::ivec3 const& pos)
{
	auto [cx, lx] = div(pos.x + Config::ChunkSize, static_cast<int>(Config::ChunkSize));
	auto [cz, lz] = div(pos.z + Config::ChunkSize, static_cast<int>(Config::ChunkSize));
	return m_chunks[cz * 3 + cx]->GetLight(glm::ivec3(lx, pos.y, lz));
}

bool ChunkGroup::Exposed(glm::ivec3 const& pos)
{
	auto [cx, lx] = div(pos.x + Config::ChunkSize, static_cast<int>(Config::ChunkSize));
	auto [cz, lz] = div(pos.z + Config::ChunkSize, static_cast<int>(Config::ChunkSize));
	return m_chunks[cz * 3 + cx]->Exposed(glm::ivec3(lx, pos.y, lz));
}

void ChunkGroup::SpreadSkylight(glm::ivec3 const& pos)
{
	s_increaseQueue.push({ pos, GetLight(pos).Skylight() });
	while (!s_increaseQueue.empty())
	{
		auto [curPos, skylight] = s_increaseQueue.front();
		s_increaseQueue.pop();
		for (uint32_t i = 0; i < 6; i++)
		{
			glm::i16vec3 nextPos = curPos + sblockNormals[i];
			if (nextPos.y < 0 || nextPos.y >= Config::ChunkHeight)
				continue;
			Light& nextLight = GetLight(nextPos);
			int nextSkylight = nextLight.Skylight();
			if (skylight <= nextSkylight)
				continue;
			int targetLight = skylight - 1 - GetBlock(nextPos).GetDescriptor().lightOpacity;
			if (targetLight > nextLight.Skylight())
			{
				nextLight.Skylight(targetLight);
				s_increaseQueue.push({ nextPos, targetLight });
			}
		}
	}
}

void ChunkGroup::DecreaseSkylight(glm::ivec3 const& pos)
{
	glm::i16vec3 curPos = pos;
	Light& light = GetLight(pos);
	int skylightLevel = light.Skylight();
	light.Skylight(0);
	s_decreaseQueue.push({ pos, skylightLevel });
	for (curPos.y--; curPos.y >= 0; curPos.y--)
	{
		Light& light = GetLight(curPos);
		int skylight = light.Skylight();
		if (skylight == 0 || skylight > skylightLevel)
			break;
		light.Skylight(0);
		s_decreaseQueue.push({ curPos, skylight });
		skylightLevel = skylight;
	}
	while (!s_decreaseQueue.empty())
	{
		auto [curPos, skylight] = s_decreaseQueue.front();
		s_decreaseQueue.pop();
		for (uint32_t i = 0; i < 6; i++)
		{
			glm::i16vec3 nextPos = curPos + sblockNormals[i];
			if (nextPos.y < 0 || nextPos.y >= Config::ChunkHeight)
				continue;
			Light& nextLight = GetLight(nextPos);
			int nextSkylight = nextLight.Skylight();
			if (nextSkylight != 0)
			{
				if (nextSkylight < skylight)
				{
					s_decreaseQueue.push({ nextPos, nextSkylight });
					nextLight.Skylight(0);
				}
				else
					s_increaseQueue.push({ nextPos, nextSkylight });
			}
		}
	}
	int defaultSkylight = m_chunks[4]->World()->SkylightLevel();
	curPos = pos;
	curPos.y++;
	skylightLevel = curPos.y == Config::ChunkHeight ? defaultSkylight : GetLight(curPos).Skylight();
	int unexposed = !Exposed(curPos);
	for (curPos.y--; curPos.y >= 0; curPos.y--)
	{
		Block block = GetBlock(curPos);
		skylightLevel -= block.GetDescriptor().lightOpacity + unexposed;
		if (skylightLevel <= 0)
			break;
		Light& light = GetLight(curPos);
		if (light.Skylight() >= skylightLevel)
			break;
		light.Skylight(skylightLevel);
		s_increaseQueue.push({ curPos, skylightLevel });
		if (block.id != Block::Air)
			unexposed = 1;
	}
	while (!s_increaseQueue.empty())
	{
		auto [curPos, skylight] = s_increaseQueue.front();
		s_increaseQueue.pop();
		for (uint32_t i = 0; i < 6; i++)
		{
			glm::i16vec3 nextPos = curPos + sblockNormals[i];
			if (nextPos.y < 0 || nextPos.y >= Config::ChunkHeight)
				continue;
			Light& nextLight = GetLight(nextPos);
			int nextSkylight = nextLight.Skylight();
			if (skylight <= nextSkylight)
				continue;
			int targetLight = skylight - 1 - GetBlock(nextPos).GetDescriptor().lightOpacity;
			if (targetLight > nextLight.Skylight())
			{
				nextLight.Skylight(targetLight);
				s_increaseQueue.push({ nextPos, targetLight });
			}
		}
	}
}

void ChunkGroup::IncreaseSkylight(glm::ivec3 const& pos)
{
	glm::i16vec3 curPos = pos;
	int skylightLevel = GetLight(pos).Skylight();
	s_increaseQueue.push({ pos, skylightLevel });
	int unexposed = !Exposed(pos);
	for (curPos.y--; curPos.y >= 0; curPos.y--)
	{
		Light& light = GetLight(curPos);
		if (light.Skylight() >= skylightLevel)
			break;
		Block block = GetBlock(curPos);
		skylightLevel -= block.GetDescriptor().lightOpacity + unexposed;
		if (skylightLevel <= 0)
			break;
		light.Skylight(skylightLevel);
		s_increaseQueue.push({ curPos, skylightLevel });
		if (block.id != Block::Air)
			unexposed = 1;
	}
	while (!s_increaseQueue.empty())
	{
		auto [curPos, skylight] = s_increaseQueue.front();
		s_increaseQueue.pop();
		for (uint32_t i = 0; i < 6; i++)
		{
			glm::i16vec3 nextPos = curPos + sblockNormals[i];
			if (nextPos.y < 0 || nextPos.y >= Config::ChunkHeight)
				continue;
			Light& nextLight = GetLight(nextPos);
			int nextSkylight = nextLight.Skylight();
			if (skylight <= nextSkylight)
				continue;
			int targetLight = skylight - 1 - GetBlock(nextPos).GetDescriptor().lightOpacity;
			if (targetLight > nextLight.Skylight())
			{
				nextLight.Skylight(targetLight);
				s_increaseQueue.push({ nextPos, targetLight });
			}
		}
	}
}

void ChunkGroup::DecreaseBlockLight(glm::ivec3 const& pos)
{
	Light& light = GetLight(pos);
	s_decreaseQueue.push({ pos, light.BlockLight() });
	light.BlockLight(GetBlock(pos).DefaultBlockLight());
	while (!s_decreaseQueue.empty())
	{
		auto [curPos, blockLight] = s_decreaseQueue.front();
		s_decreaseQueue.pop();
		for (uint32_t i = 0; i < 6; i++)
		{
			glm::ivec3 nextPos = curPos + sblockNormals[i];
			if (nextPos.y < 0 || nextPos.y >= Config::ChunkHeight)
				continue;
			Light& nextLight = GetLight(nextPos);
			int nextBlockLight = nextLight.BlockLight();
			if (nextBlockLight < blockLight)
			{
				if (nextBlockLight == 0)
					continue;
				s_decreaseQueue.push({ nextPos, nextBlockLight });
				int defaultBlockLight = GetBlock(nextPos).DefaultBlockLight();
				nextLight.BlockLight(defaultBlockLight);
				if (defaultBlockLight != 0)
					s_increaseQueue.push({ nextPos, defaultBlockLight });
			}
			else
				s_increaseQueue.push({ nextPos, nextBlockLight });
		}
	}
	while (!s_increaseQueue.empty())
	{
		auto [curPos, blockLight] = s_increaseQueue.front();
		s_increaseQueue.pop();
		for (uint32_t i = 0; i < 6; i++)
		{
			glm::i16vec3 nextPos = curPos + sblockNormals[i];
			if (nextPos.y < 0 || nextPos.y >= Config::ChunkHeight)
				continue;
			Light& nextLight = GetLight(nextPos);
			if (nextLight.BlockLight() > blockLight - 1)
				continue;
			int targetLight = blockLight - 1 - GetBlock(nextPos).GetDescriptor().lightOpacity;
			if (targetLight > nextLight.BlockLight())
			{
				nextLight.BlockLight(targetLight);
				s_increaseQueue.push({ nextPos, targetLight });
			}
		}
	}
}

void ChunkGroup::IncreaseBlockLight(glm::ivec3 const& pos)
{
	s_increaseQueue.push({ pos, GetLight(pos).BlockLight() });
	while (!s_increaseQueue.empty())
	{
		auto [curPos, blockLight] = s_increaseQueue.front();
		s_increaseQueue.pop();
		for (uint32_t i = 0; i < 6; i++)
		{
			glm::i16vec3 nextPos = curPos + sblockNormals[i];
			if (nextPos.y < 0 || nextPos.y >= Config::ChunkHeight)
				continue;
			Light& nextLight = GetLight(nextPos);
			if (nextLight.BlockLight() > blockLight - 1)
				continue;
			int targetLight = blockLight - 1 - GetBlock(nextPos).GetDescriptor().lightOpacity;
			if (targetLight > nextLight.BlockLight())
			{
				nextLight.BlockLight(targetLight);
				s_increaseQueue.push({ nextPos, targetLight });
			}
		}
	}
}

ChunkGroup Chunk::GetNeighborChunks(bool cornersRequired)
{
	Chunk* neighbors[9] = {};
	bool success = (neighbors[1] = m_world->GetChunk(m_pos + glm::ivec2(0, -1))) != nullptr &&
		(neighbors[3] = m_world->GetChunk(m_pos + glm::ivec2(-1, 0))) != nullptr &&
		(neighbors[5] = m_world->GetChunk(m_pos + glm::ivec2(1, 0))) != nullptr &&
		(neighbors[7] = m_world->GetChunk(m_pos + glm::ivec2(0, 1))) != nullptr;
	if (cornersRequired)
	{
		success &= (neighbors[0] = m_world->GetChunk(m_pos + glm::ivec2(-1, -1))) != nullptr &&
			(neighbors[2] = m_world->GetChunk(m_pos + glm::ivec2(1, -1))) != nullptr &&
			(neighbors[6] = m_world->GetChunk(m_pos + glm::ivec2(-1, 1))) != nullptr &&
			(neighbors[8] = m_world->GetChunk(m_pos + glm::ivec2(1, 1))) != nullptr;
	}
	neighbors[4] = success ? this : nullptr;
	return ChunkGroup(neighbors);
}

void Chunk::Load()
{
	m_world->LoadChunk(this);
	m_loaded = true;
}

bool Chunk::Exposed(glm::ivec3 const& pos)
{
	glm::ivec3 curPos = pos;
	for (curPos.y++; curPos.y < Config::ChunkHeight; curPos.y++)
	{
		if (GetBlock(curPos).id != Block::Air)
			return false;
	}
	return true;
}

void Chunk::CalculateInitialSkylight()
{
	for (int x = 0; x < Config::ChunkSize; x++)
	{
		for (int z = 0; z < Config::ChunkSize; z++)
		{
			int skylight = 15, unexposed = 0;
			for (int y = Config::ChunkHeight - 1; y >= 0; y--)
			{
				Block block = GetBlock(glm::ivec3(x, y, z));
				skylight -= block.GetDescriptor().lightOpacity + unexposed;
				if (skylight <= 0)
					break;
				GetLight(glm::ivec3(x, y, z)).Skylight(skylight);
				if (block.id != Block::Air)
					unexposed = 1;
			}
		}
	}
}

void Chunk::PopulateLight()
{
	ChunkGroup chunks = GetNeighborChunks(true);
	if (!chunks.Loaded())
		return;
	for (int x = 0; x < Config::ChunkSize; x++)
	{
		for (int z = 0; z < Config::ChunkSize; z++)
		{
			for (int y = Config::ChunkHeight - 1; y >= 0; y--)
			{
				glm::ivec3 pos = glm::ivec3(x, y, z);
				Light& light = GetLight(pos);
				if (light.Skylight() == 0)
					break;
				chunks.SpreadSkylight(pos);
			}
		}
	}
	m_lightPopulated = true;
}