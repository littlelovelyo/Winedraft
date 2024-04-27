#pragma once
#include <glex.h>
#include "panel.h"

class Menu
{
	GLEX_STATIC_CLASS(Menu)
public:
	enum : uint32_t
	{
		IngameMenu,
		MainMenu,
		OptionsMenu,
		PauseMenu,
		PendingToPause,
	};
private:
	inline static Panel s_mainMenuPanel, s_pauseMenuPanel, s_settingsPanel, s_optionsPanel;
	inline static uint32_t s_currentFramerateSelection, s_currentGuiScaleSelection, s_mouseSensitivitySelection;
	inline static float s_originalGuiScale;
	inline static uint32_t s_current = MainMenu, s_previous;
	inline static wchar_t s_fpsStr[10];
	inline static wchar_t s_scaleStr[4];
	inline static wchar_t s_senseStr[7];
	static void DrawBackground();
	static void DrawMainMenu();
	static void LoadOptionsMenu();
	static void DrawOptionsMenu();
	static void DrawPauseMenu();
public:
	static void Startup();
	static uint32_t Current() { return s_current; }
	static bool Paused() { return s_current == PauseMenu || s_previous == PauseMenu; }
	static void Pause() { s_current = PendingToPause; }
	static void DrawMenu();
};