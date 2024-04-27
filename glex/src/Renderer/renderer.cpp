#include "renderer.h"
#include "Environment/io.h"
#include <iostream>
#include "Environment/time.h"

using namespace glex;

void Renderer::Startup()
{
	s_cameraUniform = Gl::CreateUniformBuffer(128);
	Gl::BindUniformBuffer(s_cameraUniform, UniformBuffer::CameraBlock);
	s_transformUniform = Gl::CreateUniformBuffer(64);
	Gl::BindUniformBuffer(s_transformUniform, UniformBuffer::TransformBlock);
	s_screenUniform = Gl::CreateUniformBuffer(8);
	Gl::BindUniformBuffer(s_screenUniform, UniformBuffer::ScreenBlock);

	s_quadVertexLayout = Gl::CreateVertexLayout({ { VertexLayout::Float, 2 }, { VertexLayout::Float, 2 }, { VertexLayout::Uint, 1 }, { VertexLayout::Float, 4 }, { VertexLayout::Float, 3 }, { VertexLayout::Float, 2 }, { VertexLayout::Float, 2 } });
	s_quadVertexBuffer = Gl::CreateVertexBuffer(4 * 64 * quadsPerBatch);
	s_quadIndexBuffer = Gl::CreateIndexBuffer(4 * 6 * quadsPerBatch);
	s_stockQuadShader = Gl::CreateShader(File::ReadAllText(GLEX_SHADER_PATH "Stock/quad.glsl").c_str());
	int samplers[numTextureUnits];
	s_vertexBuffer = s_quadVertexBuffer.Map<f2i>();
	s_indexBuffer = s_quadIndexBuffer.Map<uint32_t>();
}

void Renderer::Shutdown()
{
	s_quadVertexBuffer.Unmap();
	s_quadIndexBuffer.Unmap();
	Gl::DestroyVertexLayout(s_quadVertexLayout);
	Gl::DestroyVertexBuffer(s_quadVertexBuffer);
	Gl::DestroyIndexBuffer(s_quadIndexBuffer);
	Gl::DestroyShader(s_stockQuadShader);
	Gl::DestroyUniformBuffer(s_cameraUniform);
	Gl::DestroyUniformBuffer(s_transformUniform);
	Gl::DestroyUniformBuffer(s_screenUniform);
}

/* void Renderer::Render(Scene& scene)
{
	EnableFaceCulling();
	EnableDepthTest();
	s_currentPass = static_cast<RenderOrder>(0xff);
	static uint32_t SolidBuffers[] = {
		GL_COLOR_ATTACHMENT0
	};
	static uint32_t TransparentBuffers[] = {
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};
	constexpr auto startPass = []() {
		switch (s_currentPass)
		{
			case RenderOrder::Opaque: {
				DisableBlending();
				break;
			}
			case RenderOrder::Transparent: {
				glDrawBuffers(2, TransparentBuffers);
				glDepthMask(GL_FALSE);
				EnableBlending();
				glBlendFunci(0, GL_ONE, GL_ONE);
				glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
				static uint8_t clearColor[4] = { 0, 0, 0, 0 };
				static uint8_t clearRevealage[1] = { 255 };
				glClearTexImage(s_fbo->TextureHandle(1), 0, GL_RGBA, GL_UNSIGNED_BYTE, clearColor);
				glClearTexImage(s_fbo->TextureHandle(2), 0, GL_RED, GL_UNSIGNED_BYTE, clearRevealage);
				break;
			}
		}
	};
	constexpr auto finishPass = []() {
		if (s_currentPass == RenderOrder::Transparent)
		{
			glDrawBuffers(1, SolidBuffers);
			DisableDepthTest();
			glBlendFuncSeparate(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ZERO, GL_ONE);
			s_transparentVertexArray.Bind();
			s_stockTransparentShader.Bind();
			s_transparentVertexBuffer.Bind();
			s_transparentIndexBuffer.Bind();
			s_vao = 0;
			s_shader = 0;
			s_material = nullptr;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, s_fbo->TextureHandle(1));
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, s_fbo->TextureHandle(2));
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
			glDepthMask(GL_TRUE);
		}
	};

	if (scene.m_reorderMeshRenderers)
	{
		scene.m_registry.sort<MeshRenderer>(MeshRendererComparator, entt::insertion_sort()); // insertion sort will be good
		scene.m_reorderMeshRenderers = false;
	}
	auto meshes = scene.m_registry.view<MeshRenderer, Transform>();
	meshes.each([](entt::entity entity, MeshRenderer& meshRenderer, Transform& transform) {
		if (transform.ComponentDisabled<MeshRenderer>())
			return;
		Material const* material = meshRenderer.Material();
		GlMesh const* mesh = meshRenderer.Mesh();
		if (material == nullptr || mesh == nullptr)
			return;

		if (meshRenderer.Order() != s_currentPass)
		{
			finishPass();
			s_currentPass = meshRenderer.Order();
			startPass();
		}

		SetModelMatrix(transform.ModelMat());
		Shader const& shader = material->Shader();
		VertexArray const& vertexArray = shader.VertexArray();
		if (s_material != material)
		{
			if (material->UniformBuffer().Handle() != 0)
				material->UniformBuffer().Bind(Shader::MaterialBlock);
			for (uint32_t i = 0; i < material->TextureCount(); i++)
				material->Textures()[i].Bind(i);
			s_material = material;
			if (s_shader != shader.Handle())
			{
				shader.Bind();
				s_shader = shader.Handle();
				if (s_vao != vertexArray.Handle())
				{
					vertexArray.Bind();
					s_vao = vertexArray.Handle();
				}
			}
		}
		mesh->VertexBuffer().Bind();
		mesh->IndexBuffer().Bind();
		glDrawElements(GL_TRIANGLES, mesh->IndexBuffer().Count(), GL_UNSIGNED_INT, nullptr);
	});
	finishPass();
}

void Renderer::Render(Shader shader, VertexBuffer const& vertexBuffer, IndexBuffer const& indexBuffer, uint32_t indexCount, uint32_t textureCount, Texture const* textures)
{
	s_material = nullptr;
	if (s_shader != shader.Handle())
	{
		shader.Bind();
		s_shader = shader.Handle();
		if (s_vao != shader.VertexArray().Handle())
		{
			shader.VertexArray().Bind();
			s_vao = shader.VertexArray().Handle();
		}
	}
	for (uint32_t i = 0; i < textureCount; i++)
		textures[i].Bind(i);
	vertexBuffer.Bind();
	indexBuffer.Bind();
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}
*/

uint32_t Renderer::BindTexture(Texture const& texture)
{
	uint32_t freeSlot = 0xffffffff;
	for (uint32_t i = 0; i < numTextureUnits; i++)
	{
		if (s_textureUnits[i].textureBound == texture.Handle())
		{
			s_textureUnits[i].inUse = true;
			return i;
		}
		if (freeSlot == 0xffffffff && !s_textureUnits[i].inUse)
			freeSlot = i;
	}
	if (freeSlot != 0xffffffff)
	{
		s_textureUnits[freeSlot].textureBound = texture.Handle();
		s_textureUnits[freeSlot].inUse = true;
		Gl::BindTexture(texture, freeSlot);
		return freeSlot;
	}
	return 0xffffffff;
}

void Renderer::FreeTextures()
{
	for (uint32_t i = 0; i < numTextureUnits; i++)
		s_textureUnits[i].inUse = false;
}

static wchar_t const* GetNextSpace(wchar_t const* s)
{
	if (*s == '~')
	{
		do {
			s++;
		} while (*s != '~');
		s++;
	}
	if (*s > 0xff)
		return s + 1;
	for (; *s != 0 && *s != ' ' && *s != '\n' && *s <= 0xff; s++)
	{
		if (*s == '~')
		{
			do {
				s++;
			} while (*s != '~');
		}
	}
	return s;
}

static float GetCharacterWidth(Font const& font, float size, wchar_t c)
{
	Glyph const* glyph = font.Get(c);
	if (glyph == nullptr)
		return 0.0f;
	return glyph->xadvance * size;
}

static float GetWordWidth(Font const& font, float size, wchar_t const* st, wchar_t const* ed)
{
	float w = 0.0f;
	for (; st != ed; st++)
	{
		if (*st == '~')
		{
			do {
				st++;
			} while (*st != '~');
			continue;
		}
		w += GetCharacterWidth(font, size, *st);
	}
	return w;
}

void Renderer::UIFlush()
{
	s_quadVertexBuffer.Unmap();
	s_quadIndexBuffer.Unmap();
	glDrawElements(GL_TRIANGLES, s_quadCount * 6, GL_UNSIGNED_INT, nullptr);
	s_quadCount = 0;
	FreeTextures();
	s_vertexBuffer = s_quadVertexBuffer.Map<f2i>();
	s_indexBuffer = s_quadIndexBuffer.Map<uint32_t>();
}

void Renderer::UIStart(Shader const& shader)
{
	DisableFaceCulling();
	DisableDepthTest();
	EnableBlending();
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
	Gl::BindVertexLayout(s_quadVertexLayout);
	Gl::BindVertexBuffer(s_quadVertexBuffer);
	Gl::BindIndexBuffer(s_quadIndexBuffer);
	Gl::BindShader(shader);
}

void Renderer::DrawQuad(glm::vec4 const& boundary, Texture const& texture, glm::vec4 const& uv,
	glm::vec4 const& color, glm::vec3 const& secondaryColor, glm::vec2 offset, glm::vec2 border, uint32_t flags)
{
	uint32_t textureSlot = BindTexture(texture);
	if (s_quadCount == quadsPerBatch || textureSlot == 0xffffffff)
	{
		UIFlush();
		textureSlot = BindTexture(texture);
	}
	float u0 = uv.x, v0 = uv.y;
	float u1 = u0 + uv.z, v1 = v0 - uv.w;
	flags = flags << 5 | textureSlot;
	f2i vertData[] = {
		boundary.x, boundary.y, u0, v0, flags, color.r, color.g, color.b, color.a, secondaryColor.r, secondaryColor.g, secondaryColor.b, offset.x, offset.y, border.x, border.y,
		boundary.x, boundary.w, u0, v1, flags, color.r, color.g, color.b, color.a, secondaryColor.r, secondaryColor.g, secondaryColor.b, offset.x, offset.y, border.x, border.y,
		boundary.z, boundary.w, u1, v1, flags, color.r, color.g, color.b, color.a, secondaryColor.r, secondaryColor.g, secondaryColor.b, offset.x, offset.y, border.x, border.y,
		boundary.z, boundary.y, u1, v0, flags, color.r, color.g, color.b, color.a, secondaryColor.r, secondaryColor.g, secondaryColor.b, offset.x, offset.y, border.x, border.y };
	uint32_t indexData[] = { 0, 1, 2, 0, 2, 3 };
	uint32_t iv = 64 * s_quadCount;
	for (uint32_t i = 0; i < 64; i++)
		s_vertexBuffer[iv + i] = vertData[i];
	uint32_t iu = 6 * s_quadCount;
	uint32_t ib = 4 * s_quadCount;
	for (uint32_t i = 0; i < 6; i++)
		s_indexBuffer[iu + i] = indexData[i] + ib;
	s_quadCount++;
}

void Renderer::DrawQuad(glm::vec4 const& border, Texture const& texture, glm::vec4 const& uv, float scrBorder, float texBorder, float alpha)
{
	float width = border.z - border.x;
	float height = border.w - border.y;
	DrawQuad(border, texture, uv, glm::vec4(1.0f, 1.0f, 1.0f, alpha),
		glm::vec3(uv.x, uv.y - uv.w, uv.z), glm::vec2(uv.w, texBorder), glm::vec2(scrBorder / width, scrBorder / height), 0);
}

void Renderer::DrawQuad(glm::vec4 const& border, glm::vec4 const& color)
{
	if (s_quadCount == quadsPerBatch)
		UIFlush();
	constexpr uint32_t flags = 2 << 5;
	f2i vertData[] = {
		border.x, border.y, 0.0f, 0.0f, flags, color.r, color.g, color.b, color.a, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		border.x, border.w, 0.0f, 0.0f, flags, color.r, color.g, color.b, color.a, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		border.z, border.w, 0.0f, 0.0f, flags, color.r, color.g, color.b, color.a, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		border.z, border.y, 0.0f, 0.0f, flags, color.r, color.g, color.b, color.a, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	uint32_t indexData[] = { 0, 1, 2, 0, 2, 3 };
	uint32_t iv = 64 * s_quadCount;
	for (uint32_t i = 0; i < 64; i++)
		s_vertexBuffer[iv + i] = vertData[i];
	uint32_t iu = 6 * s_quadCount;
	uint32_t ib = 4 * s_quadCount;
	for (uint32_t i = 0; i < 6; i++)
		s_indexBuffer[iu + i] = indexData[i] + ib;
	s_quadCount++;
}

void Renderer::DrawText(float x, float y,
	Font const& font, wchar_t const* st, wchar_t const* ed,
	float sizex, float sizey, float spacemult,
	glm::vec4 const& color, glm::vec3 const& borderColor, glm::vec2 border, glm::vec2 offset)
{
	for (; st != ed; st++)
	{
		if (*st == '~')
		{
			do {
				st++;
			} while (*st != '~');
			continue;
		}
		Glyph const* glyph = font.Get(*st);
		if (glyph == nullptr)
			continue;
		float x0 = x + glyph->xoffset * sizex;
		float y0 = y + glyph->yoffset * sizey;
		float x1 = x0 + glyph->w * sizex;
		float y1 = y0 + glyph->h * sizey;
		DrawQuad(glm::vec4(x0, y0, x1, y1), font.Texture(), glyph->uv, color, borderColor, offset, border, 1);
		x += glyph->xadvance * sizex * spacemult;
	}
}

void Renderer::DrawText(glm::vec4 const& boundary, Font const& font, wchar_t const* text,
	float sizex, float sizey, float spacemult, float linemult,
	float border, glm::vec4 const& color, int horizontalAlignment, int verticalAlignment, glm::vec3 borderColor, glm::vec2 offset)
{
	glm::vec2 bd = glm::vec2(border, glm::min(0.3f, 1.0f / glm::min(sizex, sizey)));
	float x = boundary.x, y = boundary.y;
	float spaceWidth = font.Get(' ')->xadvance * sizex * spacemult;
	float lineHeight = font.LineHeight() * sizey * linemult;

	// get number of lines
	wchar_t const* s = text;
	if (verticalAlignment != -1)
	{
		while (*s != 0)
		{
			if (*s == '\n')
			{
				x = boundary.x;
				y += lineHeight;
				s++;
				continue;
			}
			wchar_t const* spc = GetNextSpace(s);
			float w = GetWordWidth(font, sizex * spacemult, s, spc);
			if (x > boundary.x && x + w >= boundary.z)
			{
				x = boundary.x;
				y += lineHeight;
			}
			x += w;
			s = spc;
			if (*spc == ' ')
			{
				x += spaceWidth;
				s++;
			}
		}
		y += lineHeight;
		if (verticalAlignment == 0)
			y = boundary.y - (y - boundary.w) / 2.0f;
		else
			y = boundary.y - y + boundary.w;
	}

	// print again
	x = boundary.x;
	wchar_t const* wst = text;
	wchar_t const* lst = text;
	wchar_t const* led = text;
	while (*lst != 0)
	{
		led = GetNextSpace(wst);
		float w = GetWordWidth(font, sizex * spacemult, wst, led);
		if (*wst == '\n' || *wst == 0 || x > boundary.x && x + w > boundary.z)
		{
			if (horizontalAlignment == 0)
				x = boundary.x + (boundary.z - x) / 2.0f;
			else if (horizontalAlignment == 1)
				x = boundary.x + boundary.z - x;
			else
				x = boundary.x;
			DrawText(x, y, font, lst, wst, sizex, sizey, spacemult, color, borderColor, bd, offset);
			y += lineHeight;
			x = boundary.x;
			lst = wst;
			if (*wst == '\n')
			{
				led++;
				lst++;
			}
		}
		x += w;
		if (*led == ' ')
		{
			x += spaceWidth;
			led++;
		}
		wst = led;
	}
}

void Renderer::UIEnd()
{
	if (s_quadCount != 0)
		UIFlush();
	for (uint32_t i = 0; i < numTextureUnits; i++)
		s_textureUnits[i].textureBound = 0;
}