#include "gl.h"

using namespace glex;

#ifdef _DEBUG
void Gl::Shutdown()
{
	if (s_vaoCount != 0)
		std::cout << GLEX_LOG_WARNING << Gl::s_vaoCount << " VAOs leaked.\n";
	if (s_vboCount != 0)
		std::cout << GLEX_LOG_WARNING << Gl::s_vboCount << " VBOs leaked.\n";
	if (s_eboCount != 0)
		std::cout << GLEX_LOG_WARNING << Gl::s_eboCount << " EBOs leaked.\n";
	if (s_uboCount != 0)
		std::cout << GLEX_LOG_WARNING << Gl::s_uboCount << " UBOs leaked.\n";
	if (s_fboCount != 0)
		std::cout << GLEX_LOG_WARNING << Gl::s_fboCount << " FBOs leaked.\n";
	if (s_rboCount != 0)
		std::cout << GLEX_LOG_WARNING << Gl::s_rboCount << " RBOs leaked.\n";
	if (s_textureCount != 0)
		std::cout << GLEX_LOG_WARNING << Gl::s_textureCount << " textures leaked.\n";
	if (s_programCount != 0)
		std::cout << GLEX_LOG_WARNING << Gl::s_programCount << " programs leaked.\n";
	/* if (s_pipelineCount != 0)
		std::cout << GLEX_LOG_WARNING << Gl::s_programCount << " pipelines leaked.\n"; */
}
#endif