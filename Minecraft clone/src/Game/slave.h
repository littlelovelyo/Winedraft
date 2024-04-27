#pragma once
#include "config.h"
#include <glex.h>
#include <queue>
#include <stack>
#include <unordered_map>
#include <tuple>

enum class ThreadIdentifier : uint32_t
{
	Main,
	Slave
};

class Chunk;
class World;
class ChunkRenderer;
class Slave
{
	GLEX_STATIC_CLASS(Slave)
private:
	inline thread_local static ThreadIdentifier t_threadIdentifier;
	// thread synchronization stuff
	inline static uint32_t s_goOn = 2;
	inline static HANDLE s_hThread, s_hasWorkEvent;
	// inline static Chunk* s_requiringChunk;
	inline static glex::Lock s_lock { 137 };
	// work queues
	inline static std::queue<Chunk*> s_chunksToLoad;
	inline static std::queue<Chunk*> s_chunksToUnload;
	inline static std::queue<std::tuple<World*, glm::ivec3, bool, bool>> s_posesToRecalculateLight;
	inline static std::deque<ChunkRenderer*> s_chunksToGenerateMesh;
	inline static std::unordered_map<ChunkRenderer*, uint32_t> s_chunkIndices;
	inline static ChunkRenderer* s_chunkGeneratingMesh;
	inline static std::stack<Chunk*> s_chunksToDelete;
	// chunk pools
	inline static std::stack<Chunk*> s_freeChunks;
	inline static uint32_t s_freeChunksCap;
private:
	static DWORD __stdcall DoWork(LPVOID param);
public:
	static void Startup();
	static void Shutdown();
	static void OnSetRenderDistance();
	static void WaitForExiting();
	static ThreadIdentifier ThreadIdentifier() { return t_threadIdentifier; }
	static void LoadChunkLater(Chunk* chunk);
	static void UnloadChunkLater(Chunk* chunk);
	static void CalculateLightLater(World* world, std::vector<std::tuple<glm::ivec3, bool, bool>>& poses);
	static void GenerateMeshLater(ChunkRenderer* chunk);
	static void StopGeneratingMesh(ChunkRenderer* chunk);
	static bool MeshGenerationCanceled() { return s_chunkGeneratingMesh == nullptr; }
	static Chunk* NewChunk();
};