#pragma once
#include "obj.h"

namespace glex
{
	class Gl;
	class Shader final : public GlObj
	{
		friend class Gl;
	};
}