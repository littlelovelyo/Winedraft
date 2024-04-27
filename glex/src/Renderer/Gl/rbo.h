#pragma once
#include "obj.h"

namespace glex
{
	class Gl;
	class RenderBuffer final : public GlObj
	{
		friend class Gl;
	private:
		uint32_t m_format = 0;
	public:
		void Resize(uint32_t width, uint32_t height);
		enum Format : uint32_t {
			Depth16 = GL_DEPTH_COMPONENT16,
			Depth24 = GL_DEPTH_COMPONENT24,
			Depth32 = GL_DEPTH_COMPONENT32,
			Depth32f = GL_DEPTH_COMPONENT32F,
			Depth24Stencil8 = GL_DEPTH24_STENCIL8,
			Depth32fStencil8 = GL_DEPTH32F_STENCIL8,
			Stencil8 = GL_STENCIL_INDEX8
		};
	};
}