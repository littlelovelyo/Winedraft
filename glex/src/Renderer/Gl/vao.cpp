#include "gl.h"

using namespace glex;

VertexLayout Gl::CreateVertexLayout(std::initializer_list<VertexAttribute> const& format)
{
	VertexLayout vao;
	glCreateVertexArrays(1, &vao.m_handle);
	uint32_t index = 0, offset = 0;
	for (VertexAttribute const& attribute : format)
	{
		glEnableVertexArrayAttrib(vao.m_handle, index);
		if (attribute.type == VertexLayout::Float)
			glVertexArrayAttribFormat(vao.m_handle, index, attribute.size, static_cast<uint32_t>(attribute.type), GL_FALSE, offset);
		else
			glVertexArrayAttribIFormat(vao.m_handle, index, attribute.size, static_cast<uint32_t>(attribute.type), offset);
		glVertexArrayAttribBinding(vao.m_handle, index, 0);
		index++;
		offset += 4 * attribute.size;
	}
	vao.m_stride = offset;
#ifdef _DEBUG
	s_vaoCount++;
#endif
	return vao;
}

void Gl::DestroyVertexLayout(VertexLayout& vao)
{
	glDeleteVertexArrays(1, &vao.m_handle);
#ifdef _DEBUG
	s_vaoCount--;
#endif
}