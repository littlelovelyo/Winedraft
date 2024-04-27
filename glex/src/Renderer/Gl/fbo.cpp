#include "gl.h"
#include "Renderer/renderer.h"

using namespace glex;

FrameBuffer Gl::CreateFrameBuffer(float width, float height)
{
	FrameBuffer fbo;
	glCreateFramebuffers(1, &fbo.m_handle);
	fbo.m_width = width;
	fbo.m_height = height;
#ifdef _DEBUG
	s_fboCount++;
#endif
	return fbo;
}

void Gl::DestroyFrameBuffer(FrameBuffer& fbo)
{
	glDeleteFramebuffers(1, &fbo.m_handle);
#ifdef _DEBUG
	s_fboCount--;
#endif
}

void FrameBuffer::AttachColor(uint32_t slot, Texture const& texture)
{
	glNamedFramebufferTexture(m_handle, GL_COLOR_ATTACHMENT0 + slot, texture.Handle(), 0);
	m_type |= GL_COLOR_BUFFER_BIT;
}

void FrameBuffer::AttachDepth(RenderBuffer const& rbo)
{
	glNamedFramebufferRenderbuffer(m_handle, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo.Handle());
	m_type |= GL_DEPTH_BUFFER_BIT;
}

void FrameBuffer::Resize(float width, float height)
{
	m_width = width;
	m_height = height;
}