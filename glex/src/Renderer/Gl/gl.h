#pragma once
#include "comhdr.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "ubo.h"
#include "fbo.h"
#include "rbo.h"
#include "shader.h"
#include "texture.h"
#include "Renderer/font.h"
#include <initializer_list>
#include  <utility>

namespace glex
{
	struct VertexAttribute
	{
		uint32_t type;
		uint32_t size;
	};

	struct TextureInfo
	{
		uint32_t wrapS = GL_REPEAT, wrapT = GL_REPEAT;
		uint32_t filterMin = GL_NEAREST, filterMag = GL_LINEAR;
		float border[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		uint32_t width = 0, height = 0;
		uint32_t format = 0; // Auto
		bool generateMipmap = false;
	};

	class Gl
	{
		GLEX_STATIC_CLASS(Gl)
	private:
		inline static uint32_t s_stride, s_indexCount, s_renderTarget; // backdoors
#ifdef _DEBUG
	private:
		inline static int s_vaoCount, s_vboCount, s_eboCount, s_uboCount, s_fboCount, s_rboCount, s_textureCount, s_programCount/*, s_pipelineCount*/;
	public:
		static void Shutdown();
#endif
	public:
		// Vertex array
		static VertexLayout CreateVertexLayout(std::initializer_list<VertexAttribute> const& format);
		static void DestroyVertexLayout(VertexLayout& vao);
		static void BindVertexLayout(VertexLayout const& vao)
		{
			glBindVertexArray(vao.m_handle);
			s_stride = vao.m_stride;
		}
		// Vertex buffer
		static VertexBuffer CreateVertexBuffer(float const* data, uint32_t size);
		static VertexBuffer CreateVertexBuffer(uint32_t size);
		static VertexBuffer CreateVertexBuffer();
		static void DestroyVertexBuffer(VertexBuffer& vbo);
		static void BindVertexBuffer(VertexBuffer const& vbo)
		{
			glBindVertexBuffer(0, vbo.m_handle, 0, s_stride);
		}
		// Index buffer
		static IndexBuffer CreateIndexBuffer(uint32_t const* data, uint32_t size);
		static IndexBuffer CreateIndexBuffer(uint32_t size);
		static IndexBuffer CreateIndexBuffer();
		static void DestroyIndexBuffer(IndexBuffer& ebo);
		static void BindIndexBuffer(IndexBuffer const& ebo)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.m_handle);
			s_indexCount = ebo.Size() / 4;
		}
		// Frame buffer
		static FrameBuffer CreateFrameBuffer(float width, float height);
		static void DestroyFrameBuffer(FrameBuffer& fbo);
		static void BindFrameBuffer(FrameBuffer const& fbo)
		{
			if (s_renderTarget != fbo.m_handle)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, fbo.m_handle);
				glViewport(0, 0, fbo.m_width, fbo.m_height);
				s_renderTarget = fbo.m_handle;
			}
		}
		// Render buffer
		static RenderBuffer CreateRenderBuffer(uint32_t format, uint32_t width, uint32_t height);
		static void DestroyRenderBuffer(RenderBuffer& rbo);
		static void BindRenderBuffer(RenderBuffer const& rbo) { glBindRenderbuffer(GL_RENDERBUFFER, rbo.m_handle); }
		// Uniform buffer
		static UniformBuffer CreateUniformBuffer(uint32_t size);
		static void DestroyUniformBuffer(UniformBuffer& ubo);
		static void BindUniformBuffer(UniformBuffer const& ubo, uint32_t slot) { glBindBufferBase(GL_UNIFORM_BUFFER, slot, ubo.m_handle); }
		// Shader
		static Shader CreateShader(char const* source);
		static void DestroyShader(Shader& shader);
		static void BindShader(Shader const& shader) { glUseProgram(shader.m_handle); }
		// Texture
		static Texture CreateTexture(char const* file, TextureInfo const& format);
		static void DestroyTexture(Texture& texture);
		static void BindTexture(Texture const& texture, uint32_t slot) { glBindTextureUnit(slot, texture.m_handle); }
		// My stuff
		static Font CreateFont(Texture const& texture, char const* descFile);
		static void Draw() { glDrawElements(GL_TRIANGLES, s_indexCount, GL_UNSIGNED_INT, nullptr); }
		static void Draw(uint32_t size) { glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr); }
	};
}