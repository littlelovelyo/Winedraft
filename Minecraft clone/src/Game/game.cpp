#include "game.h"
#include "config.h"
#include "slave.h"
#include "render.h"
#include "Gui/gui.h"
#include "Gui/menu.h"
#include "Entity/player.h"
#include <Environment/context.h>
#include <Environment/io.h>
#include <Environment/time.h>
#include <Renderer/renderer.h>
#include <Environment/input.h>
#include <iostream>

void Game::Startup()
{
	// Context
	glex::ContextStartupInfo info;
	info.width = 800;
	info.height = 600;
	info.fullscreenMonitor = 0;
	info.minWidth = 800;
	info.minHeight = 600;
	info.title = "Minecraft clone";
	info.sizeCallback = Renderer::OnResize;
	info.vsync = true;
	glex::Context::Startup(info);
	glex::Renderer::SetScreenResolution(800.0f, 600.0f);
	// other game stuff, load world, maybe later on
	Config::Startup();
	Renderer::Startup();
	ChunkRenderer::Startup();
	Slave::Startup();
	Menu::Startup();
	Gui::Startup();
	ChunkStorage::SetCacheCount(Config::RenderDistance() * 8);
}

void Game::Shutdown()
{
	Slave::Shutdown();
	Gui::Shutdown();
	Renderer::Shutdown();
	Slave::WaitForExiting();
	ChunkRenderer::Shutdown();
	glex::Context::Shutdown();
}

void Game::DoFrame()
{
	glex::Context::HandleEvents();
	s_frameCount++;
	static bool fullScreen = false;
	if (glex::Input::Pressed(glex::Input::F11))
	{
		if (fullScreen)
			glex::Context::Windowed(500, 500, 800, 600);
		else
			glex::Context::FullScreen();
		fullScreen = !fullScreen;
	}
	if (Player::IsInWorld() && !Menu::Paused())
	{
		Player::UpdateWorld();
		bool flip = glex::Input::Pressed(glex::Input::E);
		bool escape = glex::Input::Pressed(glex::Input::Escape);
		if (!Gui::BusyWithContainer())
		{
			if (flip)
				Gui::OpenInventory();
			else if (escape)
				Menu::Pause();
			else
			{
				Player::DoCameraMovement();
				Player::DoPlayerInteraction();
				Player::SubmitMeshUpdate(); // Update player-affected chunks.
			}
		}
		else if (flip || escape)
			Gui::CloseContainer();
		for (World& world : s_worlds)
		{
			world.DoFrame();
			world.SubmitLightChange();
		}
		Renderer::RenderWorld();
		Gui::BusyWithContainer() ? Gui::DrawContainer() : Renderer::RenderHotbar();
	}
	else
		Renderer::RenderNull();
	Menu::DrawMenu();
	Renderer::RenderEnd();
	glex::Context::SwapBuffers();
}

void Game::Start()
{
	Player::Load();
}

void Game::Save()
{
	Player::Save();
}