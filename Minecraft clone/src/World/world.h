#pragma once
#include "generator.h"
#include "helper.h"
#include "chunkStorage.h"
#include <stack>
#include <vector>
#include <tuple>

class World
{
public:
	enum Dimension : uint32_t
	{
		Overworld
	};
	struct Ray
	{
		glm::ivec3 blockPos;
		glm::vec3 hitPos;
		BoundingBox const* bbox;
		Block block;
		uint8_t facing;
		bool blockHit;
	};
private:
	inline static Ray s_raycastResult;
	ChunkStorage m_chunks;
	std::vector<std::tuple<glm::ivec3, bool, bool>> m_lightChange;
	VirtualStorage<WorldGenerator> m_generator;
	glm::vec4 m_skyColor;
	int m_skylightLevel;
private:
	bool CanReceiveRedstonePower(glm::ivec3 const& pos);
	std::pair<uint16_t, uint16_t> CheckRedstonePowerAndConnection(glm::ivec3 const& pos, bool canConnectUpwards);
	bool UpdateBlock(glm::ivec3 const& pos);
public:
	static Ray const& RaycastResult() { return s_raycastResult; }
	World(WorldGenerator const& generator, glm::vec4 const& skyColor, int lightLevel) { m_generator = generator, m_skyColor = skyColor, m_skylightLevel = lightLevel; }
	void Save() { m_chunks.Save(); }
	glm::vec4 const& SkyColor() const { return m_skyColor; }
	int SkylightLevel() const { return m_skylightLevel; }
	Chunk* LoadChunk(glm::ivec2 const& pos) { return m_chunks.LoadChunk(this, pos); }
	void UnloadChunk(glm::ivec2 const& pos) { m_chunks.UnloadChunk(pos); }
	void FreeChunk(Chunk* chunk) { m_chunks.FreeChunk(chunk); }
	void DoFrame() {}
	Block GetBlock(glm::ivec3 const& pos);
	void SetBlockState(glm::ivec3 const& pos, Block block, bool updateLight, bool updateMesh);
	void PropagateBlockUpdate(std::initializer_list<glm::ivec3> positions);
	void CastRay(glm::vec3 origin, glm::vec3 direction);
	void LoadChunk(Chunk* chunk) { m_generator->GenerateChunk(chunk); chunk->CalculateInitialSkylight(); }
	Chunk* GetChunk(glm::ivec2 const& pos) { return m_chunks.Get(pos); }
	void SubmitLightChange() { Slave::CalculateLightLater(this, m_lightChange); }
	bool EvaluateLight(glm::ivec3 const& pos, bool sky, bool block);
};