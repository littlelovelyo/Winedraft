#include "config.h"
#include "game.h"
#include "World/chunkRenderer.h"
#include "Game/render.h"

void Config::Startup()
{
}

void Config::SetRenderDistance(uint32_t renderDistance)
{
	s_renderDistance = renderDistance;
	Slave::OnSetRenderDistance();
	ChunkRenderer::OnSetRenderDistance();
	Renderer::OnSetRenderDistance();
	ChunkStorage::SetCacheCount(Config::RenderDistance() * 8);
}

void Config::MaxFramerate(uint32_t frameRate)
{
	if (s_maxFramerate == frameRate)
		return;
	if (frameRate == 3)
	{
		glex::Context::Vsync(true);
		glex::Context::SetMinFrameTime(0.0f);
		s_maxFramerate = frameRate;
		return;
	}
	else if (s_maxFramerate == 3)
		glex::Context::Vsync(false);
	s_maxFramerate = frameRate;
	constexpr static uint32_t actualFramerate[] = { 10, 30, 60 };
	float frameTime = frameRate == 4 ? 0.0f : 1000.0f / actualFramerate[frameRate];
	glex::Context::SetMinFrameTime(frameTime);
}