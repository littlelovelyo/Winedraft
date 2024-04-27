#pragma once
#include "obj.h"
#include "texture.h"
#include "rbo.h"

namespace glex
{
	class Gl;
	class Context;
	class FrameBuffer final : GlObj
	{
		friend class Gl;
		friend class Context;
	private:
		uint32_t m_type = 0;
		float m_width = 0.0, m_height = 0.0;
	public:
		void AttachColor(uint32_t slot, Texture const& texture);
		void AttachDepth(RenderBuffer const& rbo);
		uint32_t Type() const {
			return m_type;
		}
		void Resize(float width, float height);
		float Width() const {
			return m_width;
		}
		float Height() const {
			return m_height;
		}
	};
}