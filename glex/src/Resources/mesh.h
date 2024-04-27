#pragma once

class Serializer;
class GlMesh
{
	friend class Serializer;
private:
	VertexBuffer m_vbo;
	IndexBuffer m_ebo;
public:
	GlMesh(VertexBuffer vbo, IndexBuffer&& ebo) : m_vbo(vbo), m_ebo(std::move(ebo)) {}
	~GlMesh();
	VertexBuffer const& VertexBuffer() const { return m_vbo; }
	IndexBuffer& IndexBuffer() { return m_ebo; }
	glex::VertexBuffer& VertexBuffer() { return m_vbo; }
	glex::IndexBuffer const& IndexBuffer() const { return m_ebo; }
};