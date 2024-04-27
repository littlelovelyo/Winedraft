#pragma once
#include "obj.h"

namespace glex
{
	class Gl;
	class VertexLayout final : public GlObj
	{
		friend class Gl;
	private:
		uint32_t m_stride = 0; // we save stride here
	public:
		uint32_t Stride() const {
			return m_stride;
		}
		enum Type : uint32_t {
			Float = GL_FLOAT,
			Uint = GL_UNSIGNED_INT
		};
	};
}