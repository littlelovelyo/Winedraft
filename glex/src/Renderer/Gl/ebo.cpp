#include "gl.h"

using namespace glex;

IndexBuffer Gl::CreateIndexBuffer(uint32_t const* data, uint32_t size)
{
	IndexBuffer ebo;
	ebo.Create(data, size);
#ifdef _DEBUG
	s_eboCount++;
#endif
	return ebo;
}

IndexBuffer Gl::CreateIndexBuffer(uint32_t size)
{
	IndexBuffer ebo;
	ebo.Create(size);
#ifdef _DEBUG
	s_eboCount++;
#endif
	return ebo;
}

IndexBuffer Gl::CreateIndexBuffer()
{
	IndexBuffer ebo;
	ebo.Create();
#ifdef _DEBUG
	s_eboCount++;
#endif
	return ebo;
}

void Gl::DestroyIndexBuffer(IndexBuffer& ebo)
{
	ebo.Delete();
#ifdef _DEBUG
	s_eboCount--;
#endif
}