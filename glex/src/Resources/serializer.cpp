#include <gl/glew.h>
#include "serializer.h"
#include "Environment/io.h"
#include <vector>
#ifdef _DEBUG
#include <iostream>
#endif

using namespace glex;

std::shared_ptr<GlMesh> Serializer::LoadGlMesh(char const* file)
{
	File f(file, File::ReadOnly);
	char sig[9];
	f.Read(reinterpret_cast<uint8_t*>(sig), 8);
	sig[8] = 0;
	if (!strcmp(sig, "glexmesh"))
		return nullptr;
	if (f.Read<uint32_t>() != fileVersion)
		return nullptr;
	std::vector<float> vboData;
	uint32_t stride = f.Read<uint32_t>();
	uint32_t size = f.Read<uint32_t>();
	vboData.reserve(size);
	for (uint32_t i = 0; i < size; i++)
		vboData.push_back(f.Read<float>());
	VertexBuffer vbo = Gl::CreateVertexBuffer(vboData.data(), size, stride);

	std::vector<uint32_t> eboData;
	size = f.Read<uint32_t>();
	eboData.reserve(size);
	for (uint32_t i = 0; i < size; i++)
		eboData.push_back(f.Read<uint32_t>());
	IndexBuffer ebo = Gl::CreateIndexBuffer(eboData.data(), size);

	return std::make_shared<GlMesh>(vbo, std::move(ebo));
}