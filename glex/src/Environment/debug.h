#pragma once
#ifdef _DEBUG
#include "comhdr.h"

namespace glex
{
	class Debug
	{
		GLEX_STATIC_CLASS(Debug)
	public:
		static void Init();
	};
}
#endif