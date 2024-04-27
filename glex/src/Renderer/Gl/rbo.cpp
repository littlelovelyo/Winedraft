#include "gl.h"

using namespace glex;

RenderBuffer Gl::CreateRenderBuffer(uint32_t format, uint32_t width, uint32_t height)
{
	RenderBuffer rbo;
	glCreateRenderbuffers(1, &rbo.m_handle);
	glNamedRenderbufferStorage(rbo.m_handle, format, width, height);
	rbo.m_format = format;
#ifdef _DEBUG
	s_rboCount++;
#endif
	return rbo;
}

void Gl::DestroyRenderBuffer(RenderBuffer& rbo)
{
	glDeleteRenderbuffers(1, &rbo.m_handle);
#ifdef _DEBUG
	s_rboCount--;
#endif
}

void RenderBuffer::Resize(uint32_t width, uint32_t height)
{
	glNamedRenderbufferStorage(m_handle, m_format, width, height);
}