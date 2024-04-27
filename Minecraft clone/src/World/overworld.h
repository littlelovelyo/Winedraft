#pragma once
#include "generator.h"
#include <FastNoiseLite.h>

class OverworldGenerator final : public WorldGenerator
{
private:
	inline static FastNoiseLite s_noiser;
public:
	virtual void Startup() override;
	virtual void GenerateChunk(Chunk* chunk) override;
};