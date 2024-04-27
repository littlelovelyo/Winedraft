#pragma once
#include <stdint.h>
#include <iostream>

namespace glex
{
#define GLEX_STATIC_CLASS(cls) cls() = delete;
#define GLEX_NON_COPYABLE(cls) public: cls() = default; cls(cls const&) = delete; cls& operator=(cls const&) = delete; cls(cls&&) = default; cls& operator=(cls&&) = default;
#define GLEX_STABLE_POINTER public: static constexpr bool in_place_delete = true;
#define GLEX_LOG_NORMAL "\x1b[1;37m"
#define GLEX_LOG_ERROR "\x1b[1;31m"
#define GLEX_LOG_INFO "\x1b[1;32m"
#define GLEX_LOG_WARNING "\x1b[1;33m"
#define GLEX_SHADER_PATH "F:/Programs/glex/src/"

#ifdef _DEBUG
#define GLEX_ASSERT(condition, errorMessage) if (!(condition)) { std::cout << GLEX_LOG_ERROR << "Assertion failure: " << errorMessage << std::endl; __debugbreak(); }
#else
#define GLEX_ASSERT(condition, errorMessage)
#endif

	union f2i
	{
		float f;
		uint32_t i;
		f2i(float v) : f(v) {}
		f2i(uint32_t v) : i(v) {}
	};

	constexpr uint32_t fileVersion = 0;
	constexpr uint32_t quadsPerBatch = 1024;
	constexpr uint32_t numTextureUnits = 32;

	class Exception
	{
	private:
		char const* m_desc;
	public:
		Exception(char const* desc) : m_desc(desc) {}
		char const* Desc() { return m_desc; }
	};
}
