#pragma once
#include "comhdr.h"
#include <GLFW/glfw3.h>

namespace glex
{
	class Input
	{
		GLEX_STATIC_CLASS(Input)
	private:
		static constexpr uint32_t KeyCount = 349;
		inline static uint8_t s_state[KeyCount], s_lastFrame[KeyCount];
		inline static float s_dx, s_dy, s_x, s_y, s_scroll;
	public:
		static void Press(int key) {
			s_state[key] = 1;
		}
		static void Release(int key) {
			s_state[key] = 0;
		}
		static void Move(float x, float y) {
			s_dx = x - s_x;
			s_dy = y - s_y;
			s_x = x;
			s_y = y;
		}
		static void Scroll(float scroll) {
			s_scroll = scroll;
		}
		static void Update() {
			memcpy(s_lastFrame, s_state, KeyCount);
			s_dx = 0.0f;
			s_dy = 0.0f;
			s_scroll = 0.0f;
		}
		static bool Pressing(int key) {
			return s_state[key];
		}
		static bool Pressed(int key) {
			return s_state[key] && !s_lastFrame[key];
		}
		static bool Released(int key) {
			return !s_state[key] && s_lastFrame[key];
		}
		static float DeltaX() {
			return s_dx;
		}
		static float DeltaY() {
			return s_dy;
		}
		static float MouseX() {
			return s_x;
		}
		static float MouseY() {
			return s_y;
		}
		static float Scroll() {
			return s_scroll;
		}
		enum : int
		{
			LMB = 0, // magic number
			RMB = 1,
			MMB = 2,
			Escape = GLFW_KEY_ESCAPE,
			F1 = GLFW_KEY_F1,
			F2 = GLFW_KEY_F2,
			F3 = GLFW_KEY_F3,
			F4 = GLFW_KEY_F4,
			F5 = GLFW_KEY_F5,
			F6 = GLFW_KEY_F6,
			F7 = GLFW_KEY_F7,
			F8 = GLFW_KEY_F8,
			F9 = GLFW_KEY_F9,
			F10 = GLFW_KEY_F10,
			F11 = GLFW_KEY_F11,
			F12 = GLFW_KEY_F12,
			Q = GLFW_KEY_Q,
			W = GLFW_KEY_W,
			E = GLFW_KEY_E,
			R = GLFW_KEY_R,
			T = GLFW_KEY_T,
			Y = GLFW_KEY_Y,
			U = GLFW_KEY_U,
			I = GLFW_KEY_I,
			O = GLFW_KEY_O,
			P = GLFW_KEY_P,
			A = GLFW_KEY_A,
			S = GLFW_KEY_S,
			D = GLFW_KEY_D,
			F = GLFW_KEY_F,
			G = GLFW_KEY_G,
			H = GLFW_KEY_H,
			J = GLFW_KEY_J,
			K = GLFW_KEY_K,
			L = GLFW_KEY_L,
			Z = GLFW_KEY_Z,
			X = GLFW_KEY_X,
			C = GLFW_KEY_C,
			V = GLFW_KEY_V,
			B = GLFW_KEY_B,
			N = GLFW_KEY_N,
			M = GLFW_KEY_M,
			M0 = GLFW_KEY_0,
			M1 = GLFW_KEY_1,
			M2 = GLFW_KEY_2,
			M3 = GLFW_KEY_3,
			M4 = GLFW_KEY_4,
			M5 = GLFW_KEY_5,
			M6 = GLFW_KEY_6,
			M7 = GLFW_KEY_7,
			M8 = GLFW_KEY_8,
			M9 = GLFW_KEY_9,
			GraveAccent = GLFW_KEY_GRAVE_ACCENT,
			Minus = GLFW_KEY_MINUS,
			Equal = GLFW_KEY_EQUAL,
			Backspace = GLFW_KEY_BACKSPACE,
			LeftBracket = GLFW_KEY_LEFT_BRACKET,
			RightBracket = GLFW_KEY_RIGHT_BRACKET,
			Backslash = GLFW_KEY_BACKSLASH,
			Semicolon = GLFW_KEY_SEMICOLON,
			Apostrophe = GLFW_KEY_APOSTROPHE,
			Comma = GLFW_KEY_COMMA,
			Period = GLFW_KEY_PERIOD,
			Slash = GLFW_KEY_SLASH,
			Tab = GLFW_KEY_TAB,
			CapsLock = GLFW_KEY_CAPS_LOCK,
			LeftShift = GLFW_KEY_LEFT_SHIFT,
			LeftControl = GLFW_KEY_LEFT_CONTROL,
			LeftWindows = GLFW_KEY_LEFT_SUPER,
			LeftAlt = GLFW_KEY_LEFT_ALT,
			RightShift = GLFW_KEY_RIGHT_SHIFT,
			RightControl = GLFW_KEY_RIGHT_CONTROL,
			Menu = GLFW_KEY_MENU,
			RightWindows = GLFW_KEY_RIGHT_SUPER,
			RightAlt = GLFW_KEY_RIGHT_ALT,
			Space = GLFW_KEY_SPACE,
			Return = GLFW_KEY_ENTER,
			PrintScreen = GLFW_KEY_PRINT_SCREEN,
			ScrollLock = GLFW_KEY_SCROLL_LOCK,
			PauseBreak = GLFW_KEY_PAUSE,
			Insert = GLFW_KEY_INSERT,
			Home = GLFW_KEY_HOME,
			PageUp = GLFW_KEY_PAGE_UP,
			Delete = GLFW_KEY_DELETE,
			End = GLFW_KEY_END,
			PageDown = GLFW_KEY_PAGE_DOWN,
			Up = GLFW_KEY_UP,
			Down = GLFW_KEY_DOWN,
			Left = GLFW_KEY_LEFT,
			Right = GLFW_KEY_RIGHT,
			NumLock = GLFW_KEY_NUM_LOCK,
			NumSlash = GLFW_KEY_KP_DIVIDE,
			NumStar = GLFW_KEY_KP_MULTIPLY,
			NumMinus = GLFW_KEY_KP_SUBTRACT,
			NumAdd = GLFW_KEY_KP_ADD,
			NumReturn = GLFW_KEY_KP_ENTER,
			NumDot = GLFW_KEY_KP_DECIMAL,
			Num0 = GLFW_KEY_KP_0,
			Num1 = GLFW_KEY_KP_1,
			Num2 = GLFW_KEY_KP_2,
			Num3 = GLFW_KEY_KP_3,
			Num4 = GLFW_KEY_KP_4,
			Num5 = GLFW_KEY_KP_5,
			Num6 = GLFW_KEY_KP_6,
			Num7 = GLFW_KEY_KP_7,
			Num8 = GLFW_KEY_KP_8,
			Num9 = GLFW_KEY_KP_9
		};
	};
}