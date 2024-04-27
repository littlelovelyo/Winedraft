#include "Game/game.h"
#include <glex.h>
#include <Windows.h>

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Game::Startup();
	while (!glex::Context::Closing())
		Game::DoFrame();
	Game::Shutdown();
	return 0;
}