#include "overworld.h"
#ifdef _DEBUG
#include <iostream>
#endif

void OverworldGenerator::Startup()
{
	s_noiser.SetSeed(0xabac54);
	s_noiser.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
}

static float ShapeTerrian(float x)
{
	if (x < 0.0f)
		return x + 0.95f;
	if (x < 0.675f)
		return 8.0f * x + 0.95f;
	else
		return 2.0f * x + 5.0f;
}

void OverworldGenerator::GenerateChunk(Chunk* chunk)
{
	chunk->Clear();
	/* for (int x = 0; x < Config::ChunkSize; x++)
	{
		for (int z = 0; z < Config::ChunkSize; z++)
		{
			chunk->GetBlock(glm::ivec3(x, 0, z)) = { Block::Stone, 0 };
			chunk->GetBlock(glm::ivec3(x, Config::ChunkHeight - 1, z)) = { Block::GrassBlock, 0 };
		}
	} */
	glm::ivec2 pos = chunk->Position() * static_cast<int>(Config::ChunkSize);
	for (int x = 0; x < Config::ChunkSize; x++)
	{
		float gx = pos.x + x;
		for (int z = 0; z < Config::ChunkSize; z++)
		{
			float gz = pos.y + z;

			s_noiser.SetFrequency(0.001f);
			float erosion = s_noiser.GetNoise<float>(gx, gz);
			s_noiser.SetFrequency(0.005f);
			float continentalness = s_noiser.GetNoise<float>(gx, gz);

			float erosionHeight = glm::smoothstep(-0.95f, 0.7f, erosion);
			float continentalnessHeight = ShapeTerrian(continentalness);
			uint32_t height = 10.0f * continentalnessHeight * erosionHeight + 10;

			for (uint32_t i = 0; i < height - 3; i++)
				chunk->GetBlock(glm::ivec3(x, i, z)).id = Block::Stone;
			for (uint32_t i = height - 3; i < height + 3; i++)
				chunk->GetBlock(glm::ivec3(x, i, z)).id = Block::Dirt;
			chunk->GetBlock(glm::ivec3(x, height + 3, z)).id = Block::GrassBlock;
		}
	}
}