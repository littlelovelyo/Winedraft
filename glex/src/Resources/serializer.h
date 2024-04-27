#pragma once
#include "Renderer/GL//gl.h"
#include <memory>

namespace glex
{
	class Serializer
	{
		GLEX_STATIC_CLASS(Serializer)
	public:
		static std::shared_ptr<GlMesh> LoadGlMesh(char const* file);
	};
}