#pragma once
#include "Gl/gl.h"

namespace glex
{
	class Material
	{
	private:
		Shader m_shader;
		UniformBuffer m_ubo;
		uint32_t m_numTextures;
	public:
		Material(Shader shader, uint32_t dataSize, uint32_t numTextures);
		~Material();
		Shader const& Shader() const { return m_shader; }
		UniformBuffer const& UniformBuffer() const { return m_ubo; }
		uint32_t TextureCount() const { return m_numTextures; }
		Texture const* Textures() const { return reinterpret_cast<Texture const*>(reinterpret_cast<uint8_t const*>(this) + sizeof(Material)); }
		void Update(uint32_t offset, void const* data, uint32_t size) { m_ubo.Update(offset, data, size); }
		template <typename T> T* Map() { return m_ubo.Map<T>(); }
		bool Unmap() { return m_ubo.Unmap(); }
	};
}