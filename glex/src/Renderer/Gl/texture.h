#pragma once
#include "obj.h"

namespace glex
{
	class Gl;
	class Texture final : public GlObj
	{
		friend class Gl;
	private:
		uint32_t m_format = 0;
	public:
		void Resize(uint32_t width, uint32_t height);
		enum Type : uint32_t {
			Texture2D = GL_TEXTURE_2D
		};
		enum Wrap : uint32_t {
			Repeat = GL_REPEAT,
			Mirrored = GL_MIRRORED_REPEAT,
			Clamp = GL_CLAMP_TO_EDGE,
			Border = GL_CLAMP_TO_BORDER
		};
		enum Filter : uint32_t {
			Nearest = GL_NEAREST,
			Linear = GL_LINEAR
		};
		enum Format : uint32_t {
			Auto,
			R = GL_RED,
			RGB = GL_RGB,
			RGBA = GL_RGBA,
			R16F = GL_R16F,
			RGBA16F = GL_RGBA16F
		};
	};
}