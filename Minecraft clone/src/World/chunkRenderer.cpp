#include "chunkRenderer.h"
#include "Entity/player.h"
#include "Game/slave.h"
#include "Game/game.h"
#include <utility>
#include <deque>

static std::deque<ChunkRenderer> s_chunkRenderers;

void ChunkRenderer::Startup()
{
	uint32_t size = Config::RenderDistance() * 2 - 1;
	size = size * size;
	s_chunkRenderers.resize(size * size);
}

void ChunkRenderer::Shutdown()
{
	s_chunkRenderers.clear();
}

void ChunkRenderer::OnSetRenderDistance()
{
	uint32_t size = Config::RenderDistance() * 2 - 1;
	size = size * size;
	if (size > s_chunkRenderers.size())
		s_chunkRenderers.resize(size * size);
	else
	{
		while (s_chunkRenderers.size() > size)
		{
			// We can't actually destroy it because of pointer stability.
			if (s_chunkRenderers.back().m_chunk != nullptr)
				break;
			s_chunkRenderers.pop_back();
		}
	}
}

ChunkRenderer* ChunkRenderer::GetRendererFor(Chunk* chunk)
{
	for (ChunkRenderer& chunkRenderer : s_chunkRenderers)
	{
		if (chunkRenderer.m_chunk == nullptr)
		{
			chunkRenderer.m_chunk = chunk;
			return &chunkRenderer;
		}
	}
	std::unreachable();
}

void ChunkRenderer::Release()
{
	Slave::StopGeneratingMesh(this);
	s_lock.Aquire();
	m_chunk = nullptr;
	if (m_builder != nullptr)
	{
		m_builder->Release();
		m_builder = nullptr;
	}
	s_lock.Release();
	m_solidVertexBuffer.Reload(nullptr, 0);
	m_solidIndexBuffer.Reload(nullptr, 0);
	m_transparentVertexBuffer.Reload(nullptr, 0);
	m_transparentIndexBuffer.Reload(nullptr, 0);
	// Try again.
	uint32_t size = Config::RenderDistance() * 2 - 1;
	size = size * size;
	while (s_chunkRenderers.size() > size)
	{
		if (s_chunkRenderers.back().m_chunk != nullptr)
			break;
		s_chunkRenderers.pop_back();
	}
}

ChunkRenderer::VertexBuilder* ChunkRenderer::GetFreeVertexBuilder()
{
	// 0 is reserved for main thread.
	for (uint32_t i = 1; i < Config::MaxVertexBuilders; i++)
	{
		if (_InterlockedCompareExchange8(reinterpret_cast<char*>(&s_builders[i].inUse), 1, 0) == 0)
			return &s_builders[i];
	}
	return nullptr;
}

bool ChunkRenderer::ShouldRenderSideFaces(ChunkGroup& chunks, glm::ivec3 const& pos, uint8_t facing, uint16_t id, Face const* face)
{
	glm::ivec3 next = pos + blockNormals[facing];
	if (next.y >= Config::ChunkHeight || next.y < 0)
		return true;
	Block block = chunks.GetBlock(next);
	int8_t fst = block.GetFaceState(facing ^ 1);
	if (fst != -1)
		return fst;
	switch (face->cullMode)
	{
		case CullMode::UpperSlabOrUpsideDownStair:
			return !(block.GetDescriptor().type == BlockType::Slab && block.metadata == SlabMetadata::Upper);
		case CullMode::LowerSlabOrRightSideUpStair:
			return !(block.GetDescriptor().type == BlockType::Slab && block.metadata == SlabMetadata::Lower);
		case CullMode::SameBlock:
			return block.id != id;
		default: return true;
	}
}

void ChunkRenderer::AddModel(VertexBuilder* builder, ChunkGroup& chunks, glm::ivec3 const& pos, TexturedModel const& model, uint16_t id, bool transparent, glm::mat4 const* rotation)
{
	if (model.model == nullptr)
		return;
	for (uint32_t d = 0; d < 6; d++)
	{
		Face const* face = model.model->faces[d];
		if (face == nullptr)
			continue;
		else if (ShouldRenderSideFaces(chunks, pos, d, id, face))
			AddQuads(builder, chunks, pos, face, model.textureId[d], transparent, rotation);
	}
	Face const* face = model.model->faces[6];
	if (face != nullptr)
		AddQuads(builder, chunks, pos, face, model.textureId[6], transparent, rotation);
}

inline static float Interpolate(float a0, float a1, float a2, float a3, float x, float y)
{
	return glm::mix(glm::mix(a0, a1, x), glm::mix(a2, a3, x), y);
}

void ChunkRenderer::AddQuads(VertexBuilder* builder, ChunkGroup& chunks, glm::ivec3 const& pos, Face const* face, uint16_t tex, bool transparent, glm::mat4 const* rotation)
{
	glm::vec3 offset = pos;
	constexpr float multex = 1.0f / Config::TextureSize;
	float offtex = tex % Config::TextureSize * multex;
	float offtey = 1.0f - tex / Config::TextureSize * multex - multex;
	std::vector<glex::f2i>& vertBuilder = transparent ? builder->transparentVertexBuffer : builder->vertexBuffer;
	std::vector<uint32_t>& indexBuilder = transparent ? builder->transparentIndexBuffer : builder->indexBuffer;

	int iv = 0;
	uint32_t ni = vertBuilder.size() / 6;
	for (int i = 0; i < face->numVertices; i++)
	{
		glm::vec3 vp = glm::vec3(face->vertices[iv], face->vertices[iv + 1], face->vertices[iv + 2]);
		glm::vec3 normal = glm::vec3(face->vertices[iv + 5], face->vertices[iv + 6], face->vertices[iv + 7]);
		if (rotation != nullptr)
		{
			vp = *rotation * glm::vec4(vp, 1.0f);
			normal = *rotation * glm::vec4(normal, 1.0f);
		}
		glm::vec3 position = vp + offset;
		vertBuilder.push_back(position.x);
		vertBuilder.push_back(position.y);
		vertBuilder.push_back(position.z);
		vertBuilder.push_back(face->vertices[iv + 3] * multex + offtex);
		vertBuilder.push_back(face->vertices[iv + 4] * multex + offtey);

		float skylight, blockLight;
		glm::vec3 amp = glm::abs(normal);
		glm::ivec3 dir = glm::sign(vp);
		if (amp.x > amp.y && amp.x > amp.z)
		{
			int dx = glm::sign(normal.x);
			Light l0 = chunks.GetLight(pos + glm::ivec3(dx, 0, 0));
			Light l1 = chunks.GetLight(pos + glm::ivec3(dx, 0, dir.z));
			Light l2 = chunks.GetLightValue(pos + glm::ivec3(dx, dir.y, 0));
			Light l3 = chunks.GetLightValue(pos + glm::ivec3(dx, dir.y, dir.z));
			float x = glm::abs(vp.y), y = glm::abs(vp.z);
			skylight = Interpolate(l0.Skylight(), l1.Skylight(), l2.Skylight(), l3.Skylight(), y, x);
			blockLight = Interpolate(l0.BlockLight(), l1.BlockLight(), l2.BlockLight(), l3.BlockLight(), y, x);
		}
		else if (amp.y > amp.x && amp.y > amp.z)
		{
			int dy = glm::sign(normal.y);
			Light l0 = chunks.GetLightValue(pos + glm::ivec3(0, dy, 0));
			Light l1 = chunks.GetLightValue(pos + glm::ivec3(0, dy, dir.z));
			Light l2 = chunks.GetLightValue(pos + glm::ivec3(dir.x, dy, 0));
			Light l3 = chunks.GetLightValue(pos + glm::ivec3(dir.x, dy, dir.z));
			float x = glm::abs(vp.x), y = glm::abs(vp.z);
			skylight = Interpolate(l0.Skylight(), l1.Skylight(), l2.Skylight(), l3.Skylight(), y, x);
			blockLight = Interpolate(l0.BlockLight(), l1.BlockLight(), l2.BlockLight(), l3.BlockLight(), y, x);
		}
		else
		{
			int dz = glm::sign(normal.z);
			Light l0 = chunks.GetLight(pos + glm::ivec3(0, 0, dz));
			Light l1 = chunks.GetLightValue(pos + glm::ivec3(0, dir.y, dz));
			Light l2 = chunks.GetLight(pos + glm::ivec3(dir.x, 0, dz));
			Light l3 = chunks.GetLightValue(pos + glm::ivec3(dir.x, dir.y, dz));
			float x = glm::abs(vp.x), y = glm::abs(vp.y);
			skylight = Interpolate(l0.Skylight(), l1.Skylight(), l2.Skylight(), l3.Skylight(), y, x);
			blockLight = Interpolate(l0.BlockLight(), l1.BlockLight(), l2.BlockLight(), l3.BlockLight(), y, x);
		}
		vertBuilder.push_back(static_cast<uint32_t>(skylight / 15.0f * 65535.0f) << 16 | static_cast<uint32_t>(blockLight / 15.0f * 65535.0f));
		iv += 8;
	}
	for (int i = 0; i < face->numIndices; i++)
		indexBuilder.push_back(face->indices[i] + ni);
}

ChunkRenderer::ChunkRenderer() :
	m_solidVertexBuffer(glex::Gl::CreateVertexBuffer()),
	m_solidIndexBuffer(glex::Gl::CreateIndexBuffer()),
	m_transparentVertexBuffer(glex::Gl::CreateVertexBuffer()),
	m_transparentIndexBuffer(glex::Gl::CreateIndexBuffer()) {}

ChunkRenderer::~ChunkRenderer()
{
	glex::Gl::DestroyVertexBuffer(m_solidVertexBuffer);
	glex::Gl::DestroyIndexBuffer(m_solidIndexBuffer);
	glex::Gl::DestroyVertexBuffer(m_transparentVertexBuffer);
	glex::Gl::DestroyIndexBuffer(m_transparentIndexBuffer);
}

bool ChunkRenderer::Build()
{
	Chunk* chunk = m_chunk;
	if (chunk == nullptr)
		return true;
	ChunkGroup chunks = chunk->GetNeighborChunks(true);
	if (!chunks.Loaded())
		return false;
	VertexBuilder* builder;
	bool mainThread = Slave::ThreadIdentifier() == ThreadIdentifier::Main;
	if (mainThread)
		builder = &s_builders[0];
	else
	{
		builder = GetFreeVertexBuilder();
		if (builder == nullptr)
			return false;
		chunks.ClearDirtyFlag();
	}

	builder->submissionTime = Game::FrameCount();
	for (int x = 0; x < Config::ChunkSize; x++)
	{
		for (int z = 0; z < Config::ChunkSize; z++)
		{
			for (int y = 0; y < Config::ChunkHeight; y++)
			{
				// Chunk is modified in main thread, operation fails.
				if (!mainThread && (Slave::MeshGenerationCanceled() || chunks.Modified()))
				{
					builder->Release();
					return false;
				}
				Block block = chunks.GetBlock(glm::ivec3(x, y, z));
				BlockDesctiptor const& desc = block.GetDescriptor();
				switch (desc.type)
				{
					case BlockType::Connectable:
					{
						ConnectableMetadata& metadata = block.Metadata().As<ConnectableMetadata>();
						TexturedModel const& central = desc.model[0];
						AddModel(builder, chunks, glm::ivec3(x, y, z), central, block.id, false, nullptr);
						if (metadata.FrontConnection())
						{
							TexturedModel const& front = desc.model[metadata.FrontConnection()];
							AddModel(builder, chunks, glm::ivec3(x, y, z), front, block.id, false, nullptr);
						}
						if (metadata.BackConnection())
						{
							TexturedModel const& back = desc.model[metadata.BackConnection()];
							AddModel(builder, chunks, glm::ivec3(x, y, z), back, block.id, false, &backRotation);
						}
						if (metadata.LeftConnection())
						{
							TexturedModel const& left = desc.model[metadata.LeftConnection()];
							AddModel(builder, chunks, glm::ivec3(x, y, z), left, block.id, false, &leftRotation);
						}
						if (metadata.RightConnection())
						{
							TexturedModel const& right = desc.model[metadata.RightConnection()];
							AddModel(builder, chunks, glm::ivec3(x, y, z), right, block.id, false, &rightRotation);
						}
						break;
					}
					case BlockType::Null: break;
					case BlockType::MultiTextured: AddModel(builder, chunks, glm::ivec3(x, y, z), desc.model[block.metadata], block.id, false, nullptr); break;
					default: AddModel(builder, chunks, glm::ivec3(x, y, z), *desc.model, block.id, desc.type == BlockType::Transparent, nullptr);
				}
			}
		}
	}

	s_lock.Aquire();
	if (Slave::MeshGenerationCanceled())
		builder->Release();
	else
	{
		if (m_builder == nullptr)
			m_builder = builder;
		else if (builder->submissionTime > m_builder->submissionTime)
		{
			m_builder->Release();
			m_builder = builder;
		}
		else
			builder->Release();
	}
	s_lock.Release();
	return true;
}

void ChunkRenderer::Upload()
{
	s_lock.Aquire();
	VertexBuilder* builder = m_builder;
	m_builder = nullptr;
	s_lock.Release();
	if (builder != nullptr)
	{
		std::vector<glex::f2i>& vertBuilder = builder->vertexBuffer;
		std::vector<uint32_t>& indexBuilder = builder->indexBuffer;
		std::vector<glex::f2i>& transparentVertBuilder = builder->transparentVertexBuffer;
		std::vector<uint32_t>& transparentIndexBuilder = builder->transparentIndexBuffer;
		m_solidVertexBuffer.Reload(vertBuilder.data(), vertBuilder.size() * 4);
		m_solidIndexBuffer.Reload(indexBuilder.data(), indexBuilder.size() * 4);
		m_transparentVertexBuffer.Reload(transparentVertBuilder.data(), transparentVertBuilder.size() * 4);
		m_transparentIndexBuffer.Reload(transparentIndexBuilder.data(), transparentIndexBuilder.size() * 4);
		builder->Release();
	}
}

void ChunkRenderer::DrawSolid()
{
	if (m_solidIndexBuffer.Size() != 0)
	{
		glm::ivec2 pos = m_chunk->Position();
		s_modelMat[3] = glm::vec4(pos.x * static_cast<int>(Config::ChunkSize), 0.0f, pos.y * static_cast<int>(Config::ChunkSize), 1.0f);
		glex::Renderer::SetModelMatrix(s_modelMat);
		glex::Gl::BindVertexBuffer(m_solidVertexBuffer);
		glex::Gl::BindIndexBuffer(m_solidIndexBuffer);
		glex::Gl::Draw();
	}
}

void ChunkRenderer::DrawTransparent()
{
	if (m_transparentIndexBuffer.Size() != 0)
	{
		glm::ivec2 pos = m_chunk->Position();
		s_modelMat[3] = glm::vec4(pos.x * static_cast<int>(Config::ChunkSize), 0.0f, pos.y * static_cast<int>(Config::ChunkSize), 1.0f);
		glex::Renderer::SetModelMatrix(s_modelMat);
		glex::Gl::BindVertexBuffer(m_transparentVertexBuffer);
		glex::Gl::BindIndexBuffer(m_transparentIndexBuffer);
		glex::Gl::Draw();
	}
}