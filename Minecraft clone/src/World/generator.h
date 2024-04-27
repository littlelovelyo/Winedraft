#pragma once
#include "chunk.h"

class WorldGenerator
{
public:
	virtual void Startup() = 0;
	virtual void GenerateChunk(Chunk* chunk) = 0;
};