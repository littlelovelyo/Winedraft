#include <GL/glew.h>
#include "context.h"
#include "memory.h"
#include "Renderer/renderer.h"
#include "time.h"
#include "input.h"
#include "stb_image.h"
#ifdef _DEBUG
#include <iostream>
#endif
#include <Windows.h>

using namespace glex;

void Context::Startup(ContextStartupInfo const& info)
{
	/**
	 * init stack allocator so that we can use it right away
	 */
	StackAllocator::Init(info.stackAllocatorPageSize);

	/**
	 * alloc debug console
	 */
#ifdef _DEBUG
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	// See remarks: https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hOut, &mode);
	mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
	SetConsoleMode(hOut, mode);
	uint32_t size = GetCurrentDirectoryW(0, NULL);
	wchar_t* dir = reinterpret_cast<wchar_t*>(StackAllocator::Allocate(size * 2));
	GLEX_ASSERT(dir != nullptr, "Failed to allocate memory.") {}
	GetCurrentDirectoryW(size, dir);
	std::cout << GLEX_LOG_INFO "Working at: ";
	std::wcout << dir;
	std::cout << ".\n";
	StackAllocator::Deallocate();
#endif

	/**
	 * init GLFW
	 */
	if (!glfwInit())
		throw Exception("GLFW error: failed to init.");
#ifdef _DEBUG
	glfwSetErrorCallback([](int error, char const* desc) {
		std::cout << GLEX_LOG_ERROR "Error from GLFW: " << desc << std::endl;
	});
#endif
	if (info.fullscreenMonitor == -1)
		s_monitor = nullptr;
	else if (info.fullscreenMonitor == 0)
		s_monitor = glfwGetPrimaryMonitor();
	else
	{
		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);
		if (info.fullscreenMonitor >= count)
			throw Exception("GLFW error: monitor not found.");
		s_monitor = monitors[info.fullscreenMonitor];
	}
	glfwWindowHint(GLFW_RESIZABLE, info.resizable);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DEPTH_BITS, info.depthBits);
	glfwWindowHint(GLFW_STENCIL_BITS, info.stencilBits);
	s_window = glfwCreateWindow(info.width, info.height, info.title, info.fullScreen ? s_monitor : nullptr, nullptr);
	if (s_window == nullptr)
		throw Exception("GLFW error: failed to create window.");
	glfwSetWindowSizeLimits(s_window, info.minWidth, info.minHeight, info.maxWidth, info.maxHeight);
	glfwMakeContextCurrent(s_window);
	glfwSwapInterval(info.vsync);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(s_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	/**
	 * init GLEW
	 */
	if (glewInit() != GLEW_OK)
		throw Exception("GLEW error: failed to init.");

	/**
	 * window callbacks
	 */
	glfwSetFramebufferSizeCallback(s_window, [](GLFWwindow* window, int width, int height) {
		if (width == 0 && height == 0)
			return;
		s_defaultFrameBuffer.Resize(width, height);
		if (s_sizeCallback != nullptr)
			s_sizeCallback(width, height);
	});
	glfwSetKeyCallback(s_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key < 0)
			return;
		if (action == GLFW_PRESS)
			Input::Press(key);
		else if (action == GLFW_RELEASE)
			Input::Release(key);
	});
	glfwSetMouseButtonCallback(s_window, [](GLFWwindow* window, int button, int action, int mods) {
		if (action == GLFW_PRESS)
			Input::Press(button);
		else
			Input::Release(button);
	});
	glfwSetCursorPosCallback(s_window, [](GLFWwindow* window, double xpos, double ypos) {
		Input::Move(xpos, ypos);
	});
	glfwSetScrollCallback(s_window, [](GLFWwindow* window, double xoffset, double yoffset) {
		Input::Scroll(yoffset);
	});

	/**
	 * print friendly prompts
	 */
#ifdef _DEBUG
	std::cout << GLEX_LOG_INFO << glGetString(GL_RENDERER) << std::endl;
	std::cout << GLEX_LOG_INFO "Version: " << glGetString(GL_VERSION) << std::endl;
	glDebugMessageCallback([](uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int length, char const* message, void const* userParam) {
		if (severity == GL_DEBUG_SEVERITY_HIGH)
		{
			std::cout << GLEX_LOG_ERROR "OpenGL error: " << message << std::endl;
			__debugbreak();
		}
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
			std::cout << GLEX_LOG_WARNING "OpenGL warning: " << message << std::endl;
	}, nullptr);
#endif

	/**
	 * other stuff
	 */
	stbi_set_flip_vertically_on_load(1);
	s_sizeCallback = info.sizeCallback;
	s_defaultFrameBuffer.m_type = GL_COLOR_BUFFER_BIT;
	if (info.depthBits != 0)
		s_defaultFrameBuffer.m_type |= GL_DEPTH_BUFFER_BIT;
	if (info.stencilBits != 0)
		s_defaultFrameBuffer.m_type |= GL_STENCIL_BUFFER_BIT;
	int width, height;
	glfwGetFramebufferSize(s_window, &width, &height);
	s_defaultFrameBuffer.m_width = width;
	s_defaultFrameBuffer.m_height = height;
	Renderer::Startup();
	Time::Startup();
}

void Context::Shutdown()
{
	CloseHandle(s_waitableTimer);
	Renderer::Shutdown();
	glfwDestroyWindow(s_window);
	glfwTerminate();
#ifdef _DEBUG
	Gl::Shutdown();
#endif
	StackAllocator::Shutdown();
#ifdef _DEBUG
	std::cout << GLEX_LOG_NORMAL "Application terminated.\n";
	system("pause");
#endif
}

void Context::HandleEvents()
{
	Input::Update();
	glfwPollEvents();
	Time::Update();
}

void Context::SetMinFrameTime(float time)
{
	s_minFrameTime = time;
	if (s_waitableTimer == NULL)
	{
		s_waitableTimer = CreateWaitableTimerW(NULL, TRUE, NULL);
		if (s_waitableTimer == NULL)
		{
#ifdef _DEBUG
			std::cout << GLEX_LOG_WARNING "Warning form context: failed to create timer. FPS cap is disabled.\n";
#endif
			s_minFrameTime = 0.0f;
		}
	}
}

void Context::SwapBuffers()
{
	float frameTime = Time::DeltaTime();
	if (frameTime < s_minFrameTime)
	{
		LARGE_INTEGER li;
		li.QuadPart = -static_cast<int64_t>((s_minFrameTime - frameTime) * 10000.0f);
		SetWaitableTimer(s_waitableTimer, &li, 0, NULL, NULL, FALSE);
		WaitForSingleObject(s_waitableTimer, 2 * static_cast<uint64_t>(s_minFrameTime - frameTime));
	}
	glfwSwapBuffers(s_window);
}