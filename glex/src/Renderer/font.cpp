#include "font.h"
#include "Environment/io.h"
#include "Gl/gl.h"

using namespace glex;

Font Gl::CreateFont(::Texture const& texture, char const* descFile)
{
	Font font;
	font.m_texture = &texture;
	File file(descFile, File::ReadOnly);
	char buf[13];
	buf[12] = 0;
	file.DiscardUntil('=');
	file.DiscardUntil('=');
	file.ReadUntil(reinterpret_cast<uint8_t*>(buf), 12, ' ');
	int size = atoi(buf);
	file.DiscardUntil('\n');
	file.DiscardUntil('=');
	file.ReadUntil(reinterpret_cast<uint8_t*>(buf), 12, ' ');
	int lineHeight = atoi(buf);
	file.DiscardUntil('=');
	file.DiscardUntil('=');
	file.ReadUntil(reinterpret_cast<uint8_t*>(buf), 12, ' ');
	int width = atoi(buf);
	file.DiscardUntil('=');
	file.ReadUntil(reinterpret_cast<uint8_t*>(buf), 12, ' ');
	int height = atoi(buf);
	file.DiscardUntil('\n');
	file.DiscardUntil('\n');
	font.m_lineHeight = lineHeight / static_cast<float>(size);
	file.DiscardUntil('=');
	file.ReadUntil(reinterpret_cast<uint8_t*>(buf), 12, '\n');
	int count = atoi(buf);
	font.m_table.reserve(count);
	for (int i = 0; i < count; i++)
	{
		file.DiscardUntil('=');
		file.ReadUntil(reinterpret_cast<uint8_t*>(buf), 12, ' ');
		int id = atoi(buf);
		file.DiscardUntil('=');
		file.ReadUntil(reinterpret_cast<uint8_t*>(buf), 12, ' ');
		int x = atoi(buf);
		file.DiscardUntil('=');
		file.ReadUntil(reinterpret_cast<uint8_t*>(buf), 12, ' ');
		int y = atoi(buf);
		file.DiscardUntil('=');
		file.ReadUntil(reinterpret_cast<uint8_t*>(buf), 12, ' ');
		int w = atoi(buf);
		file.DiscardUntil('=');
		file.ReadUntil(reinterpret_cast<uint8_t*>(buf), 12, ' ');
		int h = atoi(buf);
		file.DiscardUntil('=');
		file.ReadUntil(reinterpret_cast<uint8_t*>(buf), 12, ' ');
		int xoffset = atoi(buf);
		file.DiscardUntil('=');
		file.ReadUntil(reinterpret_cast<uint8_t*>(buf), 12, ' ');
		int yoffset = atoi(buf);
		file.DiscardUntil('=');
		file.ReadUntil(reinterpret_cast<uint8_t*>(buf), 12, ' ');
		int xadvance = atoi(buf);
		file.DiscardUntil('\n');

		Glyph& glyph = font.m_table[id];
		glyph.uv.x = static_cast<float>(x) / width;
		glyph.uv.y = 1.0f - static_cast<float>(y) / height;
		glyph.uv.z = static_cast<float>(w) / width;
		glyph.uv.w = static_cast<float>(h) / height;
		glyph.w = w / static_cast<float>(size);
		glyph.h = h / static_cast<float>(size);
		glyph.xoffset = xoffset / static_cast<float>(size);
		glyph.yoffset = yoffset / static_cast<float>(size);
		glyph.xadvance = xadvance / static_cast<float>(size);
	}
	return font;
}

Glyph const* Font::Get(wchar_t c) const
{
	auto iter = m_table.find(c);
	if (iter == m_table.end())
		return nullptr;
	return &(*iter).second;
}