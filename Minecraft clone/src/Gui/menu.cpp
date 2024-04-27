#include "menu.h"
#include "gui.h"
#include "Game/game.h"
#include "Game/render.h"
#include <iostream>

void Menu::Startup()
{
	// Load main menu
	s_mainMenuPanel.AddLabel(glm::vec4(0.0f, 0.0f, 650.0f, 120.0f), L"Winedraft", 120.0f, 0, -1);
	s_mainMenuPanel.AddLabel(glm::vec4(0.0f, 120.0f, 650.0f, 200.0f), L"C++ edition", 64.0f, 0, -1);
	s_mainMenuPanel.AddButton(glm::vec4(0.0f, 250.0f, 650.0f, 80.0f), L"Start game", 48.0f, []() {
		s_current = IngameMenu;
		Button::alpha = 0.0f;
		glex::Context::CaptureMouse();
		Game::Start();
	});
	s_mainMenuPanel.AddButton(glm::vec4(0.0f, 330.0f, 650.0f, 80.0f), L"Options", 48.0f, []() {
		s_current = OptionsMenu;
		s_previous = MainMenu;
		Button::alpha = 0.0f;
		LoadOptionsMenu();
	});
	s_mainMenuPanel.AddButton(glm::vec4(0.0f, 410.0f, 650.0f, 80.0f), L"Quit game", 48.0f, []() {
		Game::Quit();
	});
	// Load pause menu
	s_pauseMenuPanel.AddButton(glm::vec4(0.0f, 0.0f, 500.0f, 80.0f), L"Resume", 48.0f, []() {
		s_current = IngameMenu;
		s_previous = MainMenu;
		Button::alpha = 0.0f;
		glex::Context::CaptureMouse();
	});
	s_pauseMenuPanel.AddButton(glm::vec4(0.0f, 80.0f, 500.0f, 80.0f), L"Achievements", 48.0f, []() {
	});
	s_pauseMenuPanel.AddButton(glm::vec4(0.0f, 160.0f, 500.0f, 80.0f), L"Options", 48.0f, []() {
		s_current = OptionsMenu;
		s_previous = PauseMenu;
		Button::alpha = 0.0f;
		LoadOptionsMenu();
	});
	s_pauseMenuPanel.AddButton(glm::vec4(0.0f, 240.0f, 500.0f, 80.0f), L"Quit to title", 48.0f, []() {
		s_current = MainMenu;
		Button::alpha = 0.0f;
		Game::Save();
	});
	// Load options menu
	s_optionsPanel.AddButton(glm::vec4(0.0f, 0.0f, 300.0f, 80.0f), L"Done", 48.0f, []() {
		s_current = s_previous;
		Button::alpha = 0.0f;
		Config::MaxFramerate(s_currentFramerateSelection);
		Config::Sensitivity(s_mouseSensitivitySelection / 10.0f * 0.001f);
	});
	s_optionsPanel.AddButton(glm::vec4(300.0f, 0.0f, 300.0f, 80.0f), L"Cancel", 48.0f, []() {
		s_current = s_previous;
		Button::alpha = 0.0f;
		Config::GuiScale(s_originalGuiScale);
	});
	s_settingsPanel.AddLabel(glm::vec4(0.0f, 0.0f, 300.0f, 60.0f), L"FPS limit", 36.0f, 1, 0);
	s_settingsPanel.AddIntegerSlider(glm::vec4(360.0f, 0.0f, 400.0f, 60.0f), 0, 4, s_currentFramerateSelection);
	s_settingsPanel.AddLabel(glm::vec4(820.0f, 0.0f, 200.0f, 60.0f), s_fpsStr, 36.0f, -1, 0);
	s_settingsPanel.AddLabel(glm::vec4(0.0f, 60.0f, 300.0f, 60.0f), L"GUI scale", 36.0f, 1, 0);
	s_settingsPanel.AddIntegerSlider(glm::vec4(360.0f, 60.0f, 400.0f, 60.0f), 5, 15, s_currentGuiScaleSelection);
	s_settingsPanel.AddLabel(glm::vec4(820.0f, 60.0f, 200.0f, 60.0f), s_scaleStr, 36.0f, -1, 0);
	s_settingsPanel.AddLabel(glm::vec4(0.0f, 120.0f, 300.0f, 60.0f), L"Mouse sensitivity", 36.0f, 1, 0);
	s_settingsPanel.AddIntegerSlider(glm::vec4(360.0f, 120.0f, 400.0f, 60.0f), 1, 60, s_mouseSensitivitySelection);
	s_settingsPanel.AddLabel(glm::vec4(820.0f, 120.0f, 200.0f, 60.0f), s_senseStr, 36.0f, -1, 0);
}

void Menu::DrawBackground()
{
	float width = Renderer::s_frameBuffer.Width(), height = Renderer::s_frameBuffer.Height();
	glex::Renderer::DrawQuad(glm::vec4(0, 0, width, height), Renderer::s_background);
}

void Menu::DrawMenu()
{
	static wchar_t buffer[32] = L"Frame time: 0ms\nFPS: 0";
	static bool showDebugMenu = false;
	static float frameTime = 0.0f;
	static float waitInterval = 0.0f;
	static uint32_t frameCount = 0;
	static Button btnStart;
	if (glex::Input::Pressed(glex::Input::F3))
		showDebugMenu = !showDebugMenu;

	switch (s_current)
	{
		case MainMenu: DrawMainMenu(); break;
		case OptionsMenu: DrawOptionsMenu(); break;
		case PauseMenu: DrawPauseMenu(); break;
		case PendingToPause: s_current = PauseMenu; glex::Context::FreeMouse(); break;
		default: break;
	}

	if (showDebugMenu)
	{
		frameTime += glex::Time::DeltaTime();
		frameCount++;
		if ((waitInterval += glex::Time::DeltaTime()) >= 1000.0f)
		{
			float average = frameTime / frameCount;
			float fps = 1000.0f / average;
			swprintf(buffer, 32, L"Frame time: %.1fms\nFPS: %.1f", average, fps);
			frameTime = 0.0f;
			waitInterval = 0.0f;
			frameCount = 0;
		}
		glex::Renderer::DrawText(glm::vec4(20.0f, 20.0f, 800.0f, 100.0f), Renderer::s_font, buffer, 32.0f, 32.0f, 0.7f, 0.7f, 0.1f,
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), -1, -1, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(-0.01f, 0.01f));
	}
}

void Menu::DrawMainMenu()
{
	float width = glex::Context::DefaultFrameBuffer().Width(), height = glex::Context::DefaultFrameBuffer().Height();
	DrawBackground();
	s_mainMenuPanel.Draw(glm::vec4(0.0f, 0.0f, width, height), 0, 0);
	glex::Renderer::DrawText(glm::vec4(20.0f, 0.0f, width, height), Renderer::s_font, L"Winedraft Version 0.0",
		48.0f * Config::GuiScale(), 48.0f * Config::Flatness * Config::GuiScale(), Config::TextSpace, Config::LineSpace,
		0.0f, Config::DeepColor, -1, 1);
}

void Menu::LoadOptionsMenu()
{
	s_currentFramerateSelection = Config::MaxFramerate();
	s_currentGuiScaleSelection = (s_originalGuiScale = Config::GuiScale()) * 10.0f;
	s_mouseSensitivitySelection = Config::Sensitivity() * 10.0f / 0.001f;
}

void Menu::DrawOptionsMenu()
{
	static wchar_t const* fpsDescriptions[] = {
		L"10", L"30", L"60", L"VSync", L"Unlimited"
	};
	wcscpy_s(s_fpsStr, fpsDescriptions[s_currentFramerateSelection]);
	float guiScale = s_currentGuiScaleSelection / 10.0f, mouseSensitivity = s_mouseSensitivitySelection / 10.0f;
	swprintf_s(s_scaleStr, L"%.1f", guiScale);
	swprintf_s(s_senseStr, L"%.1f", mouseSensitivity);
	Config::GuiScale(guiScale);

	float width = glex::Context::DefaultFrameBuffer().Width(), height = glex::Context::DefaultFrameBuffer().Height();
	float offset = 20.0f * Config::GuiScale();
	DrawBackground();
	s_settingsPanel.Draw(glm::vec4(0.0f, 0.0f, width, height), 0, 0);
	s_optionsPanel.Draw(glm::vec4(offset, 0.0f, width, height - offset), -1, 1);
	glex::Renderer::DrawText(glm::vec4(0.0f, 0.0f, width - 20.0f, height), Renderer::s_font, L"Options",
		80.0f * Config::GuiScale(), 80.0f * Config::Flatness * Config::GuiScale(),
		Config::TextSpace, Config::LineSpace, 0.0f, Config::DeepColor, 1, 1);
}

void Menu::DrawPauseMenu()
{
	if (glex::Input::Pressed(glex::Input::Escape))
	{
		s_current = IngameMenu;
		Button::alpha = 0.0f;
		glex::Context::CaptureMouse();
	}
	float width = glex::Context::DefaultFrameBuffer().Width(), height = glex::Context::DefaultFrameBuffer().Height();
	DrawBackground();
	s_pauseMenuPanel.Draw(glm::vec4(0.0f, 0.0f, width, height), 0, 0);
	glex::Renderer::DrawText(glm::vec4(0.0f, 0.0f, width - 20.0f, height), Renderer::s_font, L"Pause menu",
		80.0f * Config::GuiScale(), 80.0f * Config::Flatness * Config::GuiScale(), 
		Config::TextSpace, Config::LineSpace, 0.0f, Config::DeepColor, 1, 1);
}