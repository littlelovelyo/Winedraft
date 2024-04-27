#include "gl.h"

using namespace glex;

VertexBuffer Gl::CreateVertexBuffer(float const* data, uint32_t size)
{
	VertexBuffer vbo;
	vbo.Create(data, size);
#ifdef _DEBUG
	s_vboCount++;
#endif
	return vbo;
}

VertexBuffer Gl::CreateVertexBuffer(uint32_t size)
{
	VertexBuffer vbo;
	vbo.Create(size);
#ifdef _DEBUG
	s_vboCount++;
#endif
	return vbo;
}

VertexBuffer Gl::CreateVertexBuffer()
{
	VertexBuffer vbo;
	vbo.Create();
#ifdef _DEBUG
	s_vboCount++;
#endif
	return vbo;
}

void Gl::DestroyVertexBuffer(VertexBuffer& vbo)
{
	vbo.Delete();
#ifdef _DEBUG
	s_vboCount--;
#endif
}