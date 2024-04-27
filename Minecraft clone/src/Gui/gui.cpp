#include "gui.h"
#include "Data/item.h"
#include "Game/render.h"
#include <glex.h>

wchar_t Strbuf[64];

void Gui::Startup()
{
	OnResize(Renderer::s_frameBuffer.Width(), Renderer::s_frameBuffer.Height());
	s_vertexBuffer = Renderer::s_blockVertexBuffer.Map<float>();
	s_indexBuffer = Renderer::s_blockIndexBuffer.Map<uint32_t>();
	s_inventory.emplace();
}

void Gui::Shutdown()
{
	s_inventory.reset();
	Renderer::s_blockVertexBuffer.Unmap();
	Renderer::s_blockIndexBuffer.Unmap();
}

void Gui::FlushBatch()
{
	glex::Renderer::UIEnd();
	Renderer::s_blockVertexBuffer.Unmap();
	Renderer::s_blockIndexBuffer.Unmap();
	glex::Renderer::EnableFaceCulling();
	glex::Renderer::EnableBlending();
	glex::Renderer::DisableDepthTest();
	glex::Gl::BindVertexLayout(Renderer::s_blockVertexLayout);
	glex::Gl::BindVertexBuffer(Renderer::s_blockVertexBuffer);
	glex::Gl::BindIndexBuffer(Renderer::s_blockIndexBuffer);
	glex::Gl::BindShader(Renderer::s_blockShader);
	glex::Gl::BindTexture(Renderer::s_chunkTexture, 0);
	glex::Gl::Draw(s_indexCount);
	s_vertexBuffer = Renderer::s_blockVertexBuffer.Map<float>();
	s_indexBuffer = Renderer::s_blockIndexBuffer.Map<uint32_t>();
	s_vertexCount = 0;
	s_indexCount = 0;
	glex::Renderer::UIStart();
}

void Gui::ItemDrawingDone()
{
	if (s_vertexCount != 0)
		FlushBatch();
	while (!s_itemNumbers.empty())
	{
		auto [pos, num] = s_itemNumbers.front();
		s_itemNumbers.pop();
		_itow_s(num, Strbuf, 10);
		glex::Renderer::DrawText(glm::vec4(0.0f, 0.0f, pos.x, pos.y), Renderer::s_font, Strbuf,
			32.0f * Config::GuiScale(), 32.0f * Config::Flatness * Config::GuiScale(),
			Config::TextSpace, Config::LineSpace, 0.0f, Config::WhiteColor, 1, 1);
	}
}

void Gui::DrawItem(glm::vec3 const& psz, Item const& item)
{
	if (item.IsBlock())
	{
		// black magic here
		TexturedModel const& model = *Block { item.id, 0 }.GetDescriptor().model;
		Face const* faces[4] = {
			model.model->faces[static_cast<uint32_t>(Facing::Up)],
			model.model->faces[static_cast<uint32_t>(Facing::Front)],
			model.model->faces[static_cast<uint32_t>(Facing::Right)],
			model.model->faces[static_cast<uint32_t>(Facing::Nowhere)]
		};
		uint16_t textureIds[4] = {
			model.textureId[static_cast<uint32_t>(Facing::Up)],
			model.textureId[static_cast<uint32_t>(Facing::Front)],
			model.textureId[static_cast<uint32_t>(Facing::Right)],
			model.textureId[static_cast<uint32_t>(Facing::Nowhere)]
		};
		for (uint32_t i = 0; i < 4; i++)
		{
			Face const* face = faces[i];
			if (face == nullptr)
				continue;
			if (s_vertexCount + face->numVertices > Config::BlockVertexBatch || s_indexCount + face->numIndices > Config::BlockIndexBatch)
				FlushBatch();
			constexpr float multex = 1.0f / Config::TextureSize;
			float offtex = textureIds[i] % Config::TextureSize * multex;
			float offtey = 1.0f - textureIds[i] / Config::TextureSize * multex - multex;
			float* vert = s_vertexBuffer + s_vertexCount * 8;
			uint32_t* indx = s_indexBuffer + s_indexCount;
			uint32_t vi = 0;
			float scale = 0.87f * 0.707f * psz.z;
			for (uint8_t i = 0; i < face->numVertices; i++)
			{
				// inverse y because the positive directions are different
				glm::vec3 pos = glm::vec3(face->vertices[vi], -face->vertices[vi + 1], face->vertices[vi + 2]);
				// quaternion is said to be slower than rotation matrix
				pos = MagicRotation * glm::vec4(scale * pos, 1.0f);
				*vert++ = pos.x + psz.x + psz.z / 2.0f;
				*vert++ = pos.y + psz.y + psz.z / 2.0f;
				*vert++ = pos.z;
				*vert++ = face->vertices[vi + 3] * multex + offtex;
				*vert++ = face->vertices[vi + 4] * multex + offtey;
				*vert++ = face->vertices[vi + 5];
				*vert++ = face->vertices[vi + 6];
				*vert++ = face->vertices[vi + 7];
				vi += 8;
			}
			for (uint8_t i = 0; i < face->numIndices; i++)
				*indx++ = face->indices[i] + s_vertexCount;
			s_vertexCount += face->numVertices;
			s_indexCount += face->numIndices;
		}
		s_itemNumbers.push({ glm::vec2(psz.x + psz.z, psz.y + psz.z), item.data });
	}
	else
	{
		glm::vec4 border = glm::vec4(psz.x, psz.y, psz.x + psz.z, psz.y + psz.z);
		glex::Renderer::DrawQuad(border, Renderer::s_chunkTexture, item.GetSprite());
		if (item.IsTool())
		{
			// draw durability
		}
		else
		{
			_itow_s(item.data, Strbuf, 10); // should be faster than swprintf_s
			glex::Renderer::DrawText(border, Renderer::s_font, Strbuf, 32.0f * Config::GuiScale(),
				32.0f * Config::Flatness * Config::GuiScale(),
				Config::TextSpace, Config::LineSpace, 0.0f, Config::WhiteColor, 1, 1);
		}
	}
}