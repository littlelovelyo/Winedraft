#pragma once
#include "comhdr.h"
#include "Gl/texture.h"
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

namespace glex
{
	using String = std::basic_string<uint16_t>;

	struct Glyph
	{
		glm::vec4 uv;
		float w, h;
		float xoffset, yoffset, xadvance;
	};

	class Gl;
	class Font
	{
		friend class Gl;
	private:
		std::unordered_map<wchar_t, Glyph> m_table;
		Texture const* m_texture;
		float m_lineHeight;
	public:
		Texture const& Texture() const {
			return *m_texture;
		}
		float LineHeight() const {
			return m_lineHeight;
		}
		Glyph const* Get(wchar_t c) const;
	};
}