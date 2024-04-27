#pragma once
#include <glex.h>

constexpr glm::vec4 FromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
};

constexpr glm::vec4 FromHex(uint32_t hex)
{
	uint8_t r = hex >> 24;
	uint8_t g = hex >> 16;
	uint8_t b = hex >> 8;
	uint8_t a = hex;
	return FromRGBA(r, g, b, a);
}

class Config
{
	GLEX_STATIC_CLASS(Config)
public:
	constexpr static uint32_t TextureSize = 16;
	constexpr static uint32_t ChunkSize = 16;
	constexpr static uint32_t ChunkHeight = 256;
	constexpr static uint32_t MaxVertexBuilders = 6;
	constexpr static uint32_t HotbarSlots = 13;
	constexpr static uint32_t InventoryRows = 4;
	constexpr static uint32_t ItemStackSize = 999;
	constexpr static uint32_t BlockVertexBatch = 1404;
	constexpr static uint32_t BlockIndexBatch = 2106;
	constexpr static float PlayerReach = 16.0f;
	constexpr static float TextSpace = 0.5f;
	constexpr static float Flatness = 0.9f;
	constexpr static float LineSpace = 0.8f;
	constexpr static glm::vec4 DeepColor = FromHex(0xd0637cff);
	constexpr static glm::vec4 LightColor = FromHex(0xf5dde090);
	constexpr static glm::vec4 WhiteColor = FromHex(0xffffffff);
	constexpr static glm::vec4 BlackColor = FromHex(0x000000ff);
private:
	inline static uint32_t s_renderDistance = 8;
	inline static float s_mouseSensitivity = 0.005f;
	inline static float s_guiScale = 1.0f;
	inline static uint32_t s_maxFramerate = 3; // 10, 30, 60, vsync, unlimited
public:
	static void Startup();
	static void SetRenderDistance(uint32_t renderDistance);
	static uint32_t RenderDistance() {
		return s_renderDistance;
	}
	static float Sensitivity() {
		return s_mouseSensitivity;
	}
	static void Sensitivity(float value) {
		s_mouseSensitivity = value;
	}
	static float GuiScale() {
		return s_guiScale;
	}
	static void GuiScale(float value) {
		s_guiScale = value;
	}
	static uint32_t MaxFramerate() {
		return s_maxFramerate;
	}
	static void MaxFramerate(uint32_t framerate);
};