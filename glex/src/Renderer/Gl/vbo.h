#pragma once
#include "obj.h"

namespace glex
{
	class Gl;
	class VertexBuffer final : public GlBuffer
	{
		friend class Gl;
	};
}