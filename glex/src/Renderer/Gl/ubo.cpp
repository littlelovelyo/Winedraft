#include "gl.h"

using namespace glex;

UniformBuffer Gl::CreateUniformBuffer(uint32_t size)
{
	UniformBuffer ubo;
	ubo.Create(size);
#ifdef _DEBUG
	s_uboCount++;
#endif
	return ubo;
}

void Gl::DestroyUniformBuffer(UniformBuffer& ubo)
{
	glDeleteBuffers(1, &ubo.m_handle);
#ifdef _DEBUG
	s_uboCount--;
#endif
}