#pragma once
#include "obj.h"

namespace glex
{
	class Gl;
	class UniformBuffer final: public GlBuffer
	{
		friend class Gl;
	public:
		enum UniformBlock : uint32_t {
			CameraBlock = 0,
			TransformBlock = 1,
			MaterialBlock = 2,
			ScreenBlock = 3,
		};
	};
}