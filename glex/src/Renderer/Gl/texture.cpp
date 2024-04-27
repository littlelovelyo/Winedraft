#include "gl.h"
#include <stb_image.h>
#ifdef _DEBUG
#include <iostream>
#endif

using namespace glex;

inline uint32_t GetFormat(uint32_t channels)
{
	switch (channels)
	{
		case 4: return GL_RGBA;
		case 1: return GL_RED;
		default: return GL_RGB;
	}
};

void Gl::DestroyTexture(Texture& texture)
{
	glDeleteTextures(1, &texture.m_handle);
#ifdef _DEBUG
	s_textureCount--;
#endif
}

Texture Gl::CreateTexture(char const* file, TextureInfo const& format)
{
	Texture texture;
	glGenTextures(1, &texture.m_handle);
	glBindTexture(GL_TEXTURE_2D, texture.m_handle);

	static uint8_t defaultTexture[] = { 255, 0, 255, 255 };
	glTextureParameteri(texture.m_handle, GL_TEXTURE_WRAP_S, static_cast<uint32_t>(format.wrapS));
	glTextureParameteri(texture.m_handle, GL_TEXTURE_WRAP_T, static_cast<uint32_t>(format.wrapT));
	if (format.wrapS == Texture::Border || format.wrapT == Texture::Border)
		glTextureParameterfv(texture.m_handle, GL_TEXTURE_BORDER_COLOR, format.border);
	glTextureParameteri(texture.m_handle, GL_TEXTURE_MIN_FILTER, static_cast<uint32_t>(format.filterMin));
	glTextureParameteri(texture.m_handle, GL_TEXTURE_MAG_FILTER, static_cast<uint32_t>(format.filterMag));
	int width, height, channels;
	uint8_t* image = nullptr, *data;
	if (file != nullptr)
	{
		image = stbi_load(file, &width, &height, &channels, 0);
		if (image == nullptr)
		{
#ifdef _DEBUG
			std::cout << GLEX_LOG_ERROR "Texture error: failed to load " << file << ".\n";
#endif
			data = defaultTexture;
			width = 1;
			height = 1;
			channels = 4;
		}
		else
			data = image;
	}
	else
	{
		data = nullptr;
		width = format.width;
		height = format.height;
		channels = 4; // whatever
	}
	uint32_t dataFormat = GetFormat(channels);
	uint32_t internalFormat = format.format == Texture::Auto ? dataFormat : static_cast<uint32_t>(format.format);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
	if (format.generateMipmap)
		glGenerateMipmap(GL_TEXTURE_2D);
	if (image != nullptr)
		stbi_image_free(image);
	texture.m_format = internalFormat;
#ifdef _DEBUG
	s_textureCount++;
#endif
	return texture;
}

void Texture::Resize(uint32_t width, uint32_t height)
{
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glTexImage2D(GL_TEXTURE_2D, 0, m_format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
}