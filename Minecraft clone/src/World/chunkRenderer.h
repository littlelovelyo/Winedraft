#pragma once
#include <glex.h>
#include "Game/config.h"
#include "World/chunk.h"

class ChunkRenderer
{
private:
	struct VertexBuilder
	{
		std::vector<glex::f2i> vertexBuffer;
		std::vector<uint32_t> indexBuffer;
		std::vector<glex::f2i> transparentVertexBuffer;
		std::vector<uint32_t> transparentIndexBuffer;
		uint32_t submissionTime;
		bool inUse;
		void Clear() { vertexBuffer.clear(); indexBuffer.clear(); transparentVertexBuffer.clear(); transparentIndexBuffer.clear(); submissionTime = 0; }
		void Release() { Clear(); inUse = false; }
	};
	inline static glm::mat4 s_modelMat { 1.0f };
	inline static glex::Lock s_lock { 64 };
	inline static VertexBuilder s_builders[Config::MaxVertexBuilders];
	glex::VertexBuffer m_solidVertexBuffer;
	glex::IndexBuffer m_solidIndexBuffer;
	glex::VertexBuffer m_transparentVertexBuffer;
	glex::IndexBuffer m_transparentIndexBuffer;
	VertexBuilder* m_builder = nullptr;
	Chunk* m_chunk = nullptr;
private:
	static VertexBuilder* GetFreeVertexBuilder();
	static bool ShouldRenderSideFaces(ChunkGroup& chunks, glm::ivec3 const& pos, uint8_t facing, uint16_t id, Face const* face);
	static void AddModel(VertexBuilder* builder, ChunkGroup& chunks, glm::ivec3 const& pos, TexturedModel const& model, uint16_t id, bool transparent, glm::mat4 const* rotation);
	static void AddQuads(VertexBuilder* builder, ChunkGroup& chunks, glm::ivec3 const& pos, Face const* face, uint16_t tex, bool transparent, glm::mat4 const* rotation);
public:
	ChunkRenderer();
	~ChunkRenderer();
	static void Startup();
	static void Shutdown();
	static void OnSetRenderDistance();
	static ChunkRenderer* GetRendererFor(Chunk* chunk);
	void Release();
	// Chunk const* Chunk() const { return m_chunk; }
	bool Build();
	void Upload();
	void DrawSolid();
	void DrawTransparent();
};