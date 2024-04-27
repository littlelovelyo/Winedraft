#include "material.h"

using namespace glex;

Material::Material(glex::Shader shader, uint32_t dataSize, uint32_t numTextures) : m_shader(shader), m_numTextures(numTextures)
{
	if (dataSize > 0)
		m_ubo = Gl::CreateUniformBuffer(dataSize);
}

Material::~Material()
{
	if (m_ubo.Handle() != 0)
		Gl::DestroyUniformBuffer(m_ubo);
}