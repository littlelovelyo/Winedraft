#pragma once
#include "Game/slave.h"
#include "Data/block.h"
#include <glex.h>
#include <memory>
#include <vector>

class Light
{
private:
	uint8_t m_value;
public:
	Light() = default;
	Light(int skylight, int blockLight) { Skylight(skylight); BlockLight(blockLight); }
	int Skylight() const { return m_value >> 4; }
	int BlockLight() const { return m_value & 0x0f; }
	void Skylight(int level) { m_value = m_value & 0x0f | level << 4; }
	void BlockLight(int level) { m_value = m_value & 0xf0 | level; }
	uint8_t Value() const { return m_value; }
};

class Chunk;
class ChunkGroup
{
private:
	inline thread_local static std::queue<std::pair<glm::i16vec3, int16_t>> s_decreaseQueue;
	inline thread_local static std::queue<std::pair<glm::i16vec3, int16_t>> s_increaseQueue;
	Chunk* m_chunks[9];
private:
	static constexpr bool OutOfBounds(glm::ivec3 const& pos)
	{
		constexpr int cs = Config::ChunkSize;
		return pos.y < 0 || pos.y >= Config::ChunkHeight || pos.x < -cs || pos.z < -cs || pos.x >= 2 * cs || pos.z >= 2 * cs;
	}
public:
	ChunkGroup(Chunk** chunks) { memcpy(m_chunks, chunks, 72); }
	bool Loaded() const;
	bool Modified() const;
	void ClearDirtyFlag();
	Block& GetBlock(glm::ivec3 const& pos);
	Light& GetLight(glm::ivec3 const& pos);
	Light GetLightValue(glm::ivec3 const& pos) { return pos.y < 0 || pos.y >= Config::ChunkHeight ? Light(15, 0) : GetLight(pos); }
	bool Exposed(glm::ivec3 const& pos);
	void SpreadSkylight(glm::ivec3 const& pos);
	void DecreaseSkylight(glm::ivec3 const& pos);
	void IncreaseSkylight(glm::ivec3 const& pos);
	void DecreaseBlockLight(glm::ivec3 const& pos);
	void IncreaseBlockLight(glm::ivec3 const& pos);
};

class World;
class Chunk
{
public:
	constexpr static glm::ivec2 s_offsets[8] =
	{
		{ -1, -1 },
		{ -1, 0 },
		{ -1, 1 },
		{ 0, -1 },
		{ 0, 1 },
		{ 1, -1 },
		{ 1, 0 },
		{ 1, 1 }
	};
private:
	glm::ivec2 m_pos;
	/**
	 * m_loaded is true if chunk is loaded from file or newly generated and has valid block data.
	 * m_lightSpread is true if initial skylight has been spread.
	 * m_pendingToUnload is true if chunk is waiting to be unloaded.
	 *
	 * For a newly created chunk, m_loaded = false; m_lightSpread = false; m_pendingToUnload = false.
	 * When chunk is generated in slave thread, m_loaded = true; m_lightSpread = false; m_pendingToUnload = false.
	 * Then the initial skylight is spread, m_loaded = true; m_lightSpread = true; m_pendingToUnload = false.
	 * After that the mesh is generated in slave thread, in which case m_builder != -1. And the work is done.
	 *
	 * Light will always be calculated in slave thread.
	 * Mesh can be generated in both slave and main thread.
	 */
	bool m_loaded, m_lightPopulated, m_modified, m_unloaded;
	World* m_world;
	Block m_blocks[Config::ChunkSize * Config::ChunkSize * Config::ChunkHeight]; // 256 KB
	Light m_lightMap[Config::ChunkSize * Config::ChunkSize * Config::ChunkHeight]; // 64 KB
private:
	static constexpr uint32_t GetIndex(glm::ivec3 const& pos) { return (pos.x * Config::ChunkSize + pos.z) * Config::ChunkHeight + pos.y; }
public:
	void Reset(World* world, glm::ivec2 const& pos)
	{
		m_world = world;
		m_pos = pos;
		m_loaded = false;
		m_unloaded = false;
		m_lightPopulated = false;
		m_modified = false;
	}
	void Clear()
	{
		memset(m_blocks, 0, sizeof(m_blocks));
		memset(m_lightMap, 0, sizeof(m_lightMap));
	}
	void Load();
	World* World() const { return m_world; }
	glm::ivec2 Position() const { return m_pos; }
	bool Loaded() const { return m_loaded; }
	bool Unloaded() const { return m_unloaded; }
	void Unloaded(bool value) { m_unloaded = value; }
	bool LightPopulated() const { return m_lightPopulated; }
	bool Modified() const { return m_modified; }
	void Modified(bool value) { m_modified = value; }
	Block& GetBlock(glm::ivec3 const& pos) { return m_blocks[GetIndex(pos)]; }
	Light& GetLight(glm::ivec3 const& pos) { return m_lightMap[GetIndex(pos)]; }
	bool Exposed(glm::ivec3 const& pos);
	ChunkGroup GetNeighborChunks(bool cornersRequired);
	void CalculateInitialSkylight();
	void PopulateLight();
};