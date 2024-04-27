#pragma once
#include "comhdr.h"
#include <GLFW/glfw3.h>
#include <Renderer/Gl/fbo.h>

namespace glex
{
	struct ContextStartupInfo
	{
		int width = 640, height = 480, fullscreenMonitor = -1;
		int minWidth = GLFW_DONT_CARE, minHeight = GLFW_DONT_CARE;
		int maxWidth = GLFW_DONT_CARE, maxHeight = GLFW_DONT_CARE;
		short depthBits = 24, stencilBits = 0;
		uint32_t stackAllocatorPageSize = 4096;
		char const* title = "";
		void(*sizeCallback)(int width, int height) = nullptr;
		bool resizable = true, vsync = true, fullScreen = false;
	};

	class Context
	{
		GLEX_STATIC_CLASS(Context)
	private:
		inline static void(*s_sizeCallback)(int width, int height);
		inline static GLFWmonitor* s_monitor;
		inline static GLFWwindow* s_window;
		inline static FrameBuffer s_defaultFrameBuffer;
		inline static void* s_waitableTimer;
		inline static float s_minFrameTime;
	public:
		// fullScreenMonitor: -1 - windowed mode, 0 - primary monitor, 1... - other monitors
		static void Startup(ContextStartupInfo const& info);
		static void Shutdown();
		static void HandleEvents();
		static void SetMinFrameTime(float time);
		static void SwapBuffers();
		static FrameBuffer const& DefaultFrameBuffer() {
			return s_defaultFrameBuffer;
		}
		static bool Closing() {
			return glfwWindowShouldClose(s_window);
		}
		static void Close() {
			glfwSetWindowShouldClose(s_window, 1);
		}
		static void Vsync(bool enabled) {
			glfwSwapInterval(enabled);
		}
		static void CaptureMouse() {
			glfwSetInputMode(s_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		static void FreeMouse() {
			glfwSetInputMode(s_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		static void FullScreen() {
			GLFWvidmode const* mode = glfwGetVideoMode(s_monitor);
			glfwSetWindowMonitor(s_window, s_monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		static void Windowed(int xPos, int yPos, int width, int height) {
			glfwSetWindowMonitor(s_window, nullptr, xPos, yPos, width, height, 0);
		}
	};
}