#pragma once
#include "obj.h"

namespace glex
{
	class Gl;
	class IndexBuffer final : public GlBuffer
	{
		friend class Gl;
	};
}