#pragma once
#include <glex.h>
#include "World/world.h"
#include "World/overworld.h"
#include "helper.h"

class Game
{
	GLEX_STATIC_CLASS(Game)
public:
	// Worlds
	inline static World s_worlds[1]
	{
		{ OverworldGenerator(), FromHex(0x66e8ffff), 15 }
	};
private:
	inline static uint32_t s_frameCount = 0;
public:
	static void Startup();
	static void Shutdown();
	static void DoFrame();
	static uint32_t FrameCount() { return s_frameCount; }
	static void Start();
	static void Save();
	static void Quit() { glex::Context::Close(); }
};