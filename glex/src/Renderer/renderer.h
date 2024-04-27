#pragma once
#include "Gl/gl.h"
#include "font.h"
#include "Environment/context.h"
#include <glm/gtc/type_ptr.hpp>

namespace glex
{
	class Renderer
	{
		GLEX_STATIC_CLASS(Renderer)
	private:
		struct TextureUnitStatus
		{
			uint32_t textureBound = 0;
			bool inUse = false;
		};
	private:
		inline static UniformBuffer s_cameraUniform, s_transformUniform, s_screenUniform;
		// Quad stuff
		inline static Shader s_stockQuadShader;
		inline static VertexLayout s_quadVertexLayout;
		inline static VertexBuffer s_quadVertexBuffer;
		inline static IndexBuffer s_quadIndexBuffer;
		inline static f2i* s_vertexBuffer;
		inline static uint32_t* s_indexBuffer;
		inline static uint32_t s_quadCount = 0;
		inline static TextureUnitStatus s_textureUnits[numTextureUnits];
	private:
		static uint32_t BindTexture(Texture const& texture);
		static void FreeTextures();
		static void UIFlush();
		static void DrawText(float x, float y,
			Font const& font, wchar_t const* st, wchar_t const* ed,
			float sizex, float sizey, float spacemult,
			glm::vec4 const& color, glm::vec3 const& borderColor, glm::vec2 border, glm::vec2 offset);
		static void DrawQuad(glm::vec4 const& boundary, Texture const& texture, glm::vec4 const& uv,
			glm::vec4 const& color, glm::vec3 const& secondaryColor, glm::vec2 offset, glm::vec2 border, uint32_t flags);
	public:
		static void Startup();
		static void Shutdown();
		static void ViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h) { glViewport(x, y, w, h); }
		static void SetLineWidth(float width) { glLineWidth(width); }
		static void SetClearColor(glm::vec4 const& color) { glClearColor(color.r, color.g, color.b, color.a); }
		static void ClearColor() { glClear(GL_COLOR_BUFFER_BIT); }
		static void ClearColor(glm::vec4 const& color) { glClearBufferfv(GL_COLOR, 0, glm::value_ptr(color)); }
		static void ClearDepth() { glClear(GL_DEPTH_BUFFER_BIT); }
		static void EnableDepthTest() { glEnable(GL_DEPTH_TEST); }
		static void DisableDepthTest() { glDisable(GL_DEPTH_TEST); }
		static void DepthLess() { glDepthFunc(GL_LESS); }
		static void DepthLessOrEqual() { glDepthFunc(GL_LEQUAL); }
		static void EnableBlending() { glEnable(GL_BLEND); }
		static void DisableBlending() { glDisable(GL_BLEND); }
		static void EnableFaceCulling() { glEnable(GL_CULL_FACE); }
		static void DisableFaceCulling() { glDisable(GL_CULL_FACE); }
		static void SetViewMatix(glm::mat4 const& viewMat) { s_cameraUniform.Update(0, glm::value_ptr(viewMat), sizeof(glm::mat4)); }
		static void SetProjectionMatrix(glm::mat4 const& projMat) { s_cameraUniform.Update(64, glm::value_ptr(projMat), sizeof(glm::mat4)); }
		static void SetModelMatrix(glm::mat4 const& modelMat) { s_transformUniform.Update(0, glm::value_ptr(modelMat), sizeof(glm::mat4)); }
		static void SetScreenResolution(float width, float height)
		{
			float screen[] = { width, height };
			s_screenUniform.Update(0, screen, 8);
		}
		// static void Render(Scene& scene);
		// static void Render(Shader shader, VertexBuffer const& vertexBuffer, IndexBuffer const& indexBuffer, uint32_t indexCount, uint32_t textureCount, Texture const* textures);
		static void UIStart(Shader const& shader);
		static void UIStart() { UIStart(s_stockQuadShader); }
		static void UIShader(Shader shader)
		{
			if (s_quadCount != 0)
				UIFlush();
			Gl::BindShader(shader);
		}
		static void UIShaderDefault()
		{
			if (s_quadCount != 0)
				UIFlush();
			Gl::BindShader(s_stockQuadShader);
		}
		static void DrawQuad(glm::vec4 const& border, Texture const& texture,
			glm::vec4 const& uv = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
			float scrBorder = 0.0f, float texBorder = 0.0f, float alpha = 1.0f);
		static void DrawQuad(glm::vec4 const& border, glm::vec4 const& color);
		static void DrawText(glm::vec4 const& boundary, Font const& font, wchar_t const* text,
			float sizex, float sizey, float spacemult, float linemult,
			float border, glm::vec4 const& color, int horizontalAlignment = -1, int verticalAlignment = -1,
			glm::vec3 borderColor = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2 offset = glm::vec2(0.0f, 0.0f));
		static void UIEnd();
	};
}