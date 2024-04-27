#pragma once
#include "helper.h"
#include "Data/item.h"
#include "Game/config.h"
#include "Gui/container.h"
#include "World/chunkRenderer.h"
#include "World/world.h"
#include <unordered_set>
#include <unordered_map>

class Player
{
	friend class Inventory;
private:
	// Inventory
	inline static Item s_crafting[4], s_craftingOutput;
	inline static Item s_inventory[Config::InventoryRows][Config::HotbarSlots];
	inline static Item s_hotbar[Config::HotbarSlots];
	inline static uint32_t s_hotbarIndex;
	// Controller
	inline static glex::Lock s_lock { 79 };
	inline static glex::Transform s_cameraTransform { static_cast<entt::entity>(entt::null) };
	inline static std::unordered_map<glm::ivec2, ChunkRenderer*> s_chunksInSight;
	inline static std::unordered_set<ChunkRenderer*> s_dirtyChunks;
	inline static glm::ivec2 s_chunkIn;
	inline static World* s_worldIn;
	inline static float s_timeOfDay;
private:
	static void RefreshVisibleChunks();
	static void DoRender();
	static void FlushVertexData();
public:
	// Inventory
	static void Load();
	static void Save();
	static void DrawHotbar();
	static uint32_t ItemIndex() { return s_hotbarIndex; }
	static Item const& GetItem(uint32_t index) { return s_hotbar[index]; }
	static Item& CurrentItem() { return s_hotbar[s_hotbarIndex]; }
	// Controller
	static glex::Transform const& Transform() { return s_cameraTransform; }
	static bool IsInWorld() { return s_worldIn != nullptr; }
	static World& WorldIn() { return *s_worldIn; }
	static void DoCameraMovement();
	static void DoPlayerInteraction();
	// Chunk render
	static std::unordered_map<glm::ivec2, ChunkRenderer*> const& VisibleChunks() { return s_chunksInSight; }
	static uint32_t ChunksInSight() { return s_chunksInSight.size(); }
	static void GenerateMeshIfVisible(glm::ivec2 pos);
	static void UpdateMesh(glm::ivec2 pos);
	static void UpdateMeshLater(glm::ivec2 pos);
	static void SubmitMeshUpdate();
	static void UpdateWorld();
};