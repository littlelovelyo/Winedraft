#pragma once
#include <glex.h>
#include "Entity/player.h"

class Renderer
{
	GLEX_STATIC_CLASS(Renderer)
public:
	// Frame buffer stuff
	inline static glex::Texture s_frameTexture;
	inline static glex::RenderBuffer s_frameRenderBuffer;
	inline static glex::FrameBuffer s_frameBuffer;
	// Chunk stuff
	inline static glex::VertexLayout s_chunkVertexLayout;
	inline static glex::Shader s_chunkShader;
	inline static glex::Texture s_chunkTexture;
	inline static glex::UniformBuffer s_chunkMaterial;
	// Block outline & skybox stuff
	inline static glex::VertexLayout s_wireVertexLayout;
	inline static glex::VertexBuffer s_wireVertexBuffer;
	inline static glex::IndexBuffer s_wireIndexBuffer;
	inline static glex::Texture s_starTexture;
	inline static glex::Shader s_wireShader, s_skyboxShader;
	// Block stuff
	inline static glex::VertexLayout s_blockVertexLayout;
	inline static glex::VertexBuffer s_blockVertexBuffer;
	inline static glex::IndexBuffer s_blockIndexBuffer;
	inline static glex::Shader s_blockShader;
	// GUI stuff
	inline static glex::Texture s_fontTexture;
	inline static glex::Font s_font;
	inline static glex::Texture s_widgets, s_title, s_background;
	constexpr static glm::vec4 s_button = glm::vec4(0.0f, 1.0f, 0.25f, 0.25f);
	constexpr static glm::vec4 s_slider = glm::vec4(0.25390625f, 1.0f, 0.24609375f, 0.1015625f);
	constexpr static glm::vec4 s_sliderFill = glm::vec4(0.25390625f, 0.9140625f, 0.125f, 0.1015625f);
	constexpr static glm::vec4 s_thumb = glm::vec4(0.4140625f, 0.8984375f, 0.0546875f, 0.09375f);
	constexpr static glm::vec4 s_crosshair = glm::vec4(0.50390625f, 0.99609375f, 0.0703125f, 0.0703125f);
private:
	inline static glex::Camera s_camera { Player::Transform() };
	inline static wchar_t const* s_itemName;
	inline static float s_fadeTime;
private:
	static void RenderSky();
	static void RenderVisibleChunks();
	static void RenderBlockOutline();
public:
	static void Startup();
	static void Shutdown();
	static void OnResize(int width, int height);
	static void OnSetRenderDistance();
	static void OnSelectItem(wchar_t const* name) { s_itemName = name; s_fadeTime = 3000.0f; }
	static void SetFog(glm::vec4 const& data) { s_chunkMaterial.Update(0, glm::value_ptr(data), sizeof(data)); }
	static void SetSeed(uint32_t seed) { s_chunkMaterial.Update(20, &seed, 4); }
	static void SetTime(float time) { s_chunkMaterial.Update(16, &time, 4); }
	static void RenderWorld();
	static void RenderHotbar();
	static void RenderNull() { glex::Renderer::ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); glex::Renderer::UIStart(); }
	static void RenderEnd() { glex::Renderer::UIEnd(); }
};