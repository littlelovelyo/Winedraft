#pragma once
#include "Game/game.h"
#include "container.h"
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <optional>

extern wchar_t Strbuf[64];

class Gui
{
	GLEX_STATIC_CLASS(Gui)
private:
	inline static uint32_t s_vertexCount = 0, s_indexCount = 0;
	inline static glm::mat4 s_orthoProjMat;
	inline static float* s_vertexBuffer;
	inline static uint32_t* s_indexBuffer;
	inline static std::queue<std::pair<glm::vec2, uint32_t>> s_itemNumbers;
	inline static glm::mat4 MagicRotation = glm::eulerAngleXY(glm::radians(-30.0f), glm::radians(-45.0f));
	inline static std::optional<Inventory> s_inventory;
	inline static Container* s_container = nullptr;
private:
	static void FlushBatch();
public:
	static void Startup();
	static void Shutdown();
	static void OnResize(float width, float height) { s_orthoProjMat = glm::ortho<float>(0.0f, width, height, 0.0f, -100.0f, 100.0f); }
	static void StartItemDrawing() { glex::Renderer::SetProjectionMatrix(s_orthoProjMat); }
	static void DrawItem(glm::vec3 const& psz, Item const& item);
	static void ItemDrawingDone();
	static bool BusyWithContainer() { return s_container != nullptr; }
	static void DrawContainer() { s_container->Draw(); }
	static void CloseContainer() { s_container = nullptr; glex::Context::CaptureMouse(); }
	static void OpenInventory() { s_container = &s_inventory.value(); glex::Context::FreeMouse(); }
};