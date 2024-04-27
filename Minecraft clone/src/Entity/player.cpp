#include "player.h"
#include "Data/item.h"
#include "Game/game.h"
#include "Game/render.h"
#include "World/chunkRenderer.h"
#include "Gui/gui.h"
#include <glex.h>
#include <iostream>

void Player::Load()
{
	// Load player inventory.
	s_hotbar[0] = { Block::Stone, Config::ItemStackSize };
	s_hotbar[1] = { Block::Dirt, Config::ItemStackSize };
	s_hotbar[2] = { Block::GrassBlock, Config::ItemStackSize };
	s_hotbar[3] = { Block::AcaciaPlanks, Config::ItemStackSize };
	s_hotbar[4] = { Block::BirchPlanks, Config::ItemStackSize };
	s_hotbar[5] = { Block::OakPlanks, Config::ItemStackSize };
	s_hotbar[6] = { Block::SprucePlanks, Config::ItemStackSize };
	s_hotbar[7] = { Block::IronBlock, Config::ItemStackSize };
	s_hotbar[8] = { Block::GoldBlock, Config::ItemStackSize };
	s_hotbar[9] = { Block::DiamondBlock, Config::ItemStackSize };
	s_hotbar[10] = { Block::Cobblestone, Config::ItemStackSize };
	s_hotbar[11] = { Block::CobblestoneSlab, Config::ItemStackSize };
	s_hotbar[12] = { Block::SmoothstoneSlab, Config::ItemStackSize };
	s_inventory[0][0] = { Block::AcaciaSlab, Config::ItemStackSize };
	s_inventory[0][1] = { Block::BirchSlab, Config::ItemStackSize };
	s_inventory[0][2] = { Block::OakSlab, Config::ItemStackSize };
	s_inventory[0][3] = { Block::SpruceSlab, Config::ItemStackSize };
	s_inventory[0][4] = { Block::AcaciaLog, Config::ItemStackSize };
	s_inventory[0][5] = { Block::BirchLog, Config::ItemStackSize };
	s_inventory[0][6] = { Block::OakLog, Config::ItemStackSize };
	s_inventory[0][7] = { Block::SpruceLog, Config::ItemStackSize };
	s_inventory[0][8] = { Block::Glass, Config::ItemStackSize };
	s_inventory[0][9] = { Block::BlackGlass, Config::ItemStackSize };
	s_inventory[0][10] = { Block::BlueGlass, Config::ItemStackSize };
	s_inventory[0][11] = { Block::BrownGlass, Config::ItemStackSize };
	s_inventory[0][12] = { Block::CyanGlass, Config::ItemStackSize };
	s_inventory[1][0] = { Block::GrayGlass, Config::ItemStackSize };
	s_inventory[1][1] = { Block::GreenGlass, Config::ItemStackSize };
	s_inventory[1][2] = { Block::LightBlueGlass, Config::ItemStackSize };
	s_inventory[1][3] = { Block::LightGrayGlass, Config::ItemStackSize };
	s_inventory[1][4] = { Block::LimeGlass, Config::ItemStackSize };
	s_inventory[1][5] = { Block::MagentaGlass, Config::ItemStackSize };
	s_inventory[1][6] = { Block::OrangeGlass, Config::ItemStackSize };
	s_inventory[1][7] = { Block::PinkGlass, Config::ItemStackSize };
	s_inventory[1][8] = { Block::PurpleGlass, Config::ItemStackSize };
	s_inventory[1][9] = { Block::RedGlass, Config::ItemStackSize };
	s_inventory[1][10] = { Block::WhiteGlass, Config::ItemStackSize };
	s_inventory[1][11] = { Block::YellowGlass, Config::ItemStackSize };
	s_inventory[1][12] = { Block::RedstoneBlock, Config::ItemStackSize };
	s_inventory[2][0] = { Block::RedstoneLamp, Config::ItemStackSize };
	s_inventory[2][1] = { Item::Redstone, Config::ItemStackSize };
	s_inventory[2][2] = { Block::Glowstone, Config::ItemStackSize };
	// Set player position.
	s_worldIn = &Game::s_worlds[World::Overworld]; // Player is in overworld.
	s_timeOfDay = 0.0f;
	Renderer::SetSeed(0xdeadbeef);
	Renderer::SetTime(s_timeOfDay);
	Renderer::SetFog(glm::vec4(glm::vec3(s_worldIn->SkyColor()), Config::RenderDistance() * Config::ChunkSize));
	int renderDistance = Config::RenderDistance();
	s_chunksInSight.clear();
	// Load a ring outer.
	for (int ox = -renderDistance; ox <= renderDistance; ox++)
	{
		for (int oz = -renderDistance; oz <= renderDistance; oz++)
		{
			glm::ivec2 pos = glm::ivec2(ox, oz);
			Chunk* chunk = s_worldIn->LoadChunk(pos);
			if (glm::abs(ox) != renderDistance && glm::abs(oz) != renderDistance)
				s_chunksInSight[pos] = ChunkRenderer::GetRendererFor(chunk);
		}
	}
}

void Player::Save()
{
	for (auto& [pos, chunk] : s_chunksInSight)
		chunk->Release();
	s_chunksInSight.clear();
	for (World& world : Game::s_worlds)
		world.Save();
}

void Player::DoCameraMovement()
{
	// Move the camera.
	constexpr float pi = glm::pi<float>();
	static float verticalSpeed = 0.0f, horizontalSpeed = 0.0f;
	static float verticalDirection = 0.0f, forwardDirection = 0.0f;
	int forward = glex::Input::Pressing(glex::Input::W) - glex::Input::Pressing(glex::Input::S);
	int sideways = glex::Input::Pressing(glex::Input::D) - glex::Input::Pressing(glex::Input::A);
	int vertical = glex::Input::Pressing(glex::Input::Space) - glex::Input::Pressing(glex::Input::LeftShift);
	if (vertical == 0)
		verticalSpeed = glm::max(0.0f, verticalSpeed - 0.0001f * glex::Time::DeltaTime());
	else
	{
		verticalSpeed = glm::min(0.03f, verticalSpeed + 0.00003f * glex::Time::DeltaTime());
		verticalDirection = vertical;
	}
	if (forward == 0 && sideways == 0)
		horizontalSpeed = glm::max(0.0f, horizontalSpeed - 0.0001f * glex::Time::DeltaTime());
	else
	{
		float targetDirection = glm::atan(static_cast<float>(forward), static_cast<float>(sideways));
		if (horizontalSpeed < 0.003f)
			forwardDirection = targetDirection;
		else if (forwardDirection < targetDirection)
		{
			if (targetDirection - forwardDirection > pi)
				forwardDirection = glm::max(targetDirection, forwardDirection + 2.0f * pi - 0.01f * glex::Time::DeltaTime());
			else
				forwardDirection = glm::min(targetDirection, forwardDirection + 0.01f * glex::Time::DeltaTime());
		}
		else
		{
			if (forwardDirection - targetDirection > pi)
				forwardDirection = glm::min(targetDirection, forwardDirection - 2.0f * pi + 0.01f * glex::Time::DeltaTime());
			else
				forwardDirection = glm::max(targetDirection, forwardDirection - 0.01f * glex::Time::DeltaTime());
		}
		horizontalSpeed = glm::min(0.03f, horizontalSpeed + 0.00003f * glex::Time::DeltaTime());
	}
	// check if we need to render new chunks.
	if (verticalSpeed > 0.0f || horizontalSpeed > 0.0f)
	{
		glm::vec3 forward = s_cameraTransform.Forward();
		forward.y = 0.0f;
		forward = glm::normalize(forward);
		glm::vec3 velocity = glm::vec3(0.0f, verticalSpeed * verticalDirection, 0.0f) -
			s_cameraTransform.Left() * glm::cos(forwardDirection) * horizontalSpeed + forward * glm::sin(forwardDirection) * horizontalSpeed;
		s_cameraTransform.Move(velocity * glex::Time::DeltaTime());
		glm::vec3 pos = s_cameraTransform.Position();
		int ncx = glm::floor((pos.x + 0.5f) / Config::ChunkSize);
		int ncz = glm::floor((pos.z + 0.5f) / Config::ChunkSize);
		if (ncx != s_chunkIn.x || ncz != s_chunkIn.y)
		{
			s_chunkIn = glm::ivec2(ncx, ncz);
			RefreshVisibleChunks();
		}
	}
	// Rotate the camera.
	static float rotY = 0.0f, rotX = 0.0f;
	float dx = -glex::Input::DeltaX() * Config::Sensitivity(), dy = glex::Input::DeltaY() * Config::Sensitivity();
	rotY += dx;
	rotX = glm::clamp(rotX + dy, -glm::radians(89.9f), glm::radians(89.9f));
	s_cameraTransform.Rotation(glm::vec3(rotX, rotY, 0.0f));
}

void Player::DoPlayerInteraction()
{
	int scroll = glex::Input::Scroll();
	if (scroll != 0)
	{
		s_hotbarIndex = (static_cast<int>(s_hotbarIndex) - scroll + static_cast<int>(Config::HotbarSlots)) % Config::HotbarSlots;
		Item& item = s_hotbar[s_hotbarIndex];
		if (item.id != Block::Air)
			Renderer::OnSelectItem(item.GetName());
	}
	s_worldIn->CastRay(s_cameraTransform.Position(), s_cameraTransform.Forward() * Config::PlayerReach);
	World::Ray const& ray = World::RaycastResult();
	if (glex::Input::Pressed(glex::Input::LMB))
	{
		if (ray.blockHit)
		{
			s_worldIn->SetBlockState(ray.blockPos, { Block::Air, 0 }, true, true);
			s_worldIn->PropagateBlockUpdate({ ray.blockPos + blockNormals[0], ray.blockPos + blockNormals[1], ray.blockPos + blockNormals[2],
				ray.blockPos + blockNormals[3], ray.blockPos + blockNormals[4], ray.blockPos + blockNormals[5] });
		}
	}
	else if (glex::Input::Pressed(glex::Input::RMB))
	{
		if (ray.blockHit)
		{
			Item& item = Player::CurrentItem();
			Block block = item.GetCorrespondingBlock();
			if (block.id != Block::Air && block.id != Block::Unknown)
			{
				// finally, we place a block
				item.Consume();
				glm::ivec3 targetPos = ray.blockPos + blockNormals[static_cast<uint32_t>(ray.facing)];
				switch (block.GetDescriptor().type)
				{
					case BlockType::Slab:
					{
						if (ray.block.id == block.id && (ray.block.metadata == SlabMetadata::Upper && ray.facing == Facing::Down ||
							ray.block.metadata == SlabMetadata::Lower && ray.facing == Facing::Up))
						{
							// put double slab in the same block
							s_worldIn->SetBlockState(ray.blockPos, { block.id, SlabMetadata::Double }, true, true);
							s_worldIn->PropagateBlockUpdate({ ray.blockPos });
							return;
						}
						Block target = s_worldIn->GetBlock(targetPos);
						block.metadata = ray.facing == Facing::Up ? SlabMetadata::Lower :
							ray.facing == Facing::Down ? SlabMetadata::Upper :
							glm::fract(ray.hitPos.y) <= 0.5f ? SlabMetadata::Upper : SlabMetadata::Lower;
						if (target.id == block.id && (target.metadata == SlabMetadata::Upper && block.metadata == SlabMetadata::Lower ||
							target.metadata == SlabMetadata::Lower && block.metadata == SlabMetadata::Upper))
						{
							// put double slab in the next block
							s_worldIn->SetBlockState(targetPos, { block.id, SlabMetadata::Double }, true, true);
							s_worldIn->PropagateBlockUpdate({ targetPos });
							return;
						}
						break;
					}
					case BlockType::ThreeDirectional:
					{
						switch (ray.facing)
						{
							case Facing::Up: case Facing::Down: block.metadata = DirectionalMetadata::YAxis; break;
							case Facing::Front: case Facing::Back: block.metadata = DirectionalMetadata::ZAxis; break;
							default: block.metadata = DirectionalMetadata::XAxis; break;
						}
						break;
					}
				}
				s_worldIn->SetBlockState(targetPos, block, true, true);
				s_worldIn->PropagateBlockUpdate({ targetPos });
			}
		}
	}
}

/* ChunkRenderer* Player::GetChunkRenderer(glm::ivec2 pos)
{
	ChunkRenderer* ret;
	s_lock.Aquire(); // Aquires s_lock,
	auto iter = s_chunksInSight.find(pos);
	ret = iter == s_chunksInSight.end() ? nullptr : iter->second;
	s_lock.Release();
	return ret;
} */

void Player::GenerateMeshIfVisible(glm::ivec2 pos)
{
	ChunkRenderer* ret;
	s_lock.Aquire();
	auto iter = s_chunksInSight.find(pos);
	if (iter != s_chunksInSight.end())
		Slave::GenerateMeshLater(iter->second);
	s_lock.Release();
}

void Player::RefreshVisibleChunks()
{
	int renderDistance = Config::RenderDistance();
	s_lock.Aquire();
	for (auto iter = s_chunksInSight.begin(); iter != s_chunksInSight.end();)
	{
		auto& [pos, chunk] = *iter;
		glm::ivec2 dist = glm::abs(s_chunkIn - pos);
		if (glm::max(dist.x, dist.y) >= renderDistance)
		{
			chunk->Release(); // Aquires Slave::s_lock.
			s_worldIn->UnloadChunk(pos); // Aquires Slave::s_lock.
			for (uint32_t i = 0; i < 8; i++)
			{
				glm::ivec2 np = pos + Chunk::s_offsets[i];
				dist = glm::abs(s_chunkIn - np);
				if (glm::max(dist.x, dist.y) > renderDistance)
					s_worldIn->UnloadChunk(np);
			}
			s_chunksInSight.erase(iter++);
		}
		else
			++iter;
	}
	for (int ox = -renderDistance + 1; ox < renderDistance; ox++)
	{
		for (int oz = -renderDistance + 1; oz < renderDistance; oz++)
		{
			glm::ivec2 pos = s_chunkIn + glm::ivec2(ox, oz);
			ChunkRenderer*& chunk = s_chunksInSight[pos];
			if (chunk == nullptr)
			{
				chunk = ChunkRenderer::GetRendererFor(s_worldIn->LoadChunk(pos));
				Slave::GenerateMeshLater(chunk);
				for (uint32_t i = 0; i < 8; i++)
					s_worldIn->LoadChunk(pos + Chunk::s_offsets[i]);
			}
		}
	}
	s_lock.Release();
}

void Player::UpdateMesh(glm::ivec2 pos)
{
	auto iter = s_chunksInSight.find(pos);
	if (iter != s_chunksInSight.end())
	{
		Slave::StopGeneratingMesh(iter->second);
		s_dirtyChunks.insert(iter->second);
	}
}

void Player::UpdateMeshLater(glm::ivec2 pos)
{
	auto iter = s_chunksInSight.find(pos);
	if (iter != s_chunksInSight.end())
		Slave::GenerateMeshLater(iter->second);
}

void Player::SubmitMeshUpdate()
{
	for (ChunkRenderer* chunk : s_dirtyChunks)
	{
		chunk->Build();
		chunk->Upload();
	}
	s_dirtyChunks.clear();
}

void Player::UpdateWorld()
{
	s_timeOfDay += (glex::Input::Pressing(glex::Input::G) ? 4.0f : 0.1f) * glex::Time::DeltaTime();
	if (s_timeOfDay > 80000.0f)
		s_timeOfDay -= 80000.0f;
	Renderer::SetTime(s_timeOfDay);
}