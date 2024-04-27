#include "world.h"
#include "Game/game.h"
#include "Game/config.h"
#include "Game/slave.h"
#include "Gui/menu.h"
#include "Gui/gui.h"
#include "Entity/player.h"
#include <glm/gtx/extended_min_max.hpp>
#include <queue>

bool World::CanReceiveRedstonePower(glm::ivec3 const& pos)
{
	for (uint8_t i = 2; i < 6; i++)
	{
		Block side = GetBlock(pos + blockNormals[i]);
		if (side.IsPowered() || side.id == Block::Redstone && side.Metadata().As<RedstoneMetadata>().IsPoweringSide(i ^ 1))
			return true;
	}
	return GetBlock(pos + blockNormals[Facing::Down]).IsPowered() || GetBlock(pos + blockNormals[Facing::Up]).IsPowered();
}

std::pair<uint16_t, uint16_t> World::CheckRedstonePowerAndConnection(glm::ivec3 const& pos, bool canConnectUpwards)
{
	uint16_t power = 0, connection;
	Block block = GetBlock(pos);
	if (block.IsStronglyPowered())
	{
		power = 15;
		connection = RedstoneMetadata::Unconnected;
	}
	else if (block.id == Block::Redstone)
	{
		power = std::max(static_cast<int>(power), block.Metadata().As<RedstoneMetadata>().Power() - 1);
		connection = RedstoneMetadata::Connected;
	}
	else if (!block.IsSolid() && (block = GetBlock(pos + blockNormals[Facing::Down])).id == Block::Redstone)
	{
		power = std::max(static_cast<int>(power), block.Metadata().As<RedstoneMetadata>().Power() - 1);
		connection = RedstoneMetadata::ConnectedDownwards;
	}
	else if (canConnectUpwards && (block = GetBlock(pos + blockNormals[Facing::Up])).id == Block::Redstone)
	{
		power = std::max(static_cast<int>(power), block.Metadata().As<RedstoneMetadata>().Power() - 1);
		connection = RedstoneMetadata::ConnectedUpwards;
	}
	else
		connection = RedstoneMetadata::Unconnected;
	return  { power, connection };
}

bool World::UpdateBlock(glm::ivec3 const& pos)
{
	Block block = GetBlock(pos);
	switch (block.id)
	{
		case Block::RedstoneBlock:
		{
			if (!block.IsPowered())
			{
				block.Metadata().As<PowerableMetadata>().PowerStrongly();
				SetBlockState(pos, block, false, false);
				return true;
			}
			break;
		};
		case Block::RedstoneLamp:
		{
			uint16_t state = CanReceiveRedstonePower(pos) ? MultitexturedMetadata::Lit : MultitexturedMetadata::Unlit;
			if (state != block.Metadata().As<MultitexturedMetadata>().TextureIndex())
			{
				block.Metadata().As<MultitexturedMetadata>().TextureIndex(state);
				SetBlockState(pos, block, true, true);
			}
			break;
		}
		case Block::Redstone:
		{
			Block bottom = GetBlock(pos + blockNormals[Facing::Down]);
			if (!bottom.IsFull())
			{
				SetBlockState(pos, { Block::Air, 0 }, false, true);
				return true;
			}
			RedstoneMetadata metadata = Metadata(block.metadata).As<RedstoneMetadata>();
			uint16_t power = bottom.IsStronglyPowered() ? 15 : 0;
			Block up = GetBlock(pos + blockNormals[Facing::Up]);
			bool canConnectUpwards = !up.IsSolid();
			power = up.IsStronglyPowered() ? 15 : power;
			auto [frontPower, frontConnection] = CheckRedstonePowerAndConnection(pos + blockNormals[Facing::Front], canConnectUpwards);
			auto [backPower, backConnection] = CheckRedstonePowerAndConnection(pos + blockNormals[Facing::Back], canConnectUpwards);
			auto [leftPower, leftConnection] = CheckRedstonePowerAndConnection(pos + blockNormals[Facing::Left], canConnectUpwards);
			auto [rightPower, rightConnection] = CheckRedstonePowerAndConnection(pos + blockNormals[Facing::Right], canConnectUpwards);
			uint16_t connectionMask = frontConnection << 6 | backConnection << 4 | leftConnection << 2 | rightConnection;
			if (connectionMask != 0)
			{
				if (frontConnection << 6 == connectionMask)
					backConnection = RedstoneMetadata::Connected;
				else if (backConnection << 4 == connectionMask)
					frontConnection = RedstoneMetadata::Connected;
				else if (leftConnection << 2 == connectionMask)
					rightConnection = RedstoneMetadata::Connected;
				else if (rightConnection == connectionMask)
					leftConnection = RedstoneMetadata::Connected;
			}
			power = glm::max(power, glm::max(glm::max(frontPower, backPower), glm::max(leftPower, rightPower)));
			metadata.Power(power);
			metadata.FrontConnection(frontConnection);
			metadata.BackConnection(backConnection);
			metadata.LeftConnection(leftConnection);
			metadata.RightConnection(rightConnection);
			if (metadata.Value() != block.metadata)
			{
				block.metadata = metadata.Value();
				SetBlockState(pos, block, false, true);
				return true;
			}
			break;
		}
	}
	return false;
}

void World::PropagateBlockUpdate(std::initializer_list<glm::ivec3> positions)
{
	static std::queue<glm::ivec3> queue;
	for (glm::ivec3 const& pos : positions)
		queue.push(pos);
	while (!queue.empty())
	{
		glm::ivec3 pos = queue.front();
		queue.pop();
		if (UpdateBlock(pos))
		{
			for (uint32_t i = 0; i < 6; i++)
			{
				glm::ivec3 adj = pos + blockNormals[i];
				queue.push(adj);
			}
			for (uint32_t i = 0; i < 12; i++)
			{
				glm::ivec3 adj = pos + blockEdges[i];
				queue.push(adj);
			}
		}
	}
}

Block World::GetBlock(glm::ivec3 const& pos)
{
	if (pos.y < 0 || pos.y >= Config::ChunkHeight)
		return { Block::Air, 0 };
	auto [cx, lx] = floor_divmod(pos.x, Config::ChunkSize);
	auto [cz, lz] = floor_divmod(pos.z, Config::ChunkSize);
	Chunk* chunk = m_chunks.Get(glm::ivec2(cx, cz));
	if (chunk == nullptr || !chunk->Loaded() || chunk->Unloaded())
		return { Block::Unknown, 0 };
	return chunk->GetBlock(glm::ivec3(lx, pos.y, lz));
}

void World::SetBlockState(glm::ivec3 const& pos, Block block, bool updateLight, bool updateMesh)
{
	if (pos.y < 0 || pos.y >= Config::ChunkHeight)
		return;
	auto [cx, lx] = floor_divmod(pos.x, Config::ChunkSize);
	auto [cz, lz] = floor_divmod(pos.z, Config::ChunkSize);
	Chunk* chunk = m_chunks.Get(glm::ivec2(cx, cz));
	if (chunk == nullptr || !chunk->Loaded() || chunk->Unloaded())
		return;
	glm::ivec3 lpos = glm::ivec3(lx, pos.y, lz);
	Block& rb = chunk->GetBlock(lpos);
	Block original = rb;
	rb = block;
	if (updateLight)
	{
		auto [cx, lx] = floor_divmod(pos.x, Config::ChunkSize);
		auto [cz, lz] = floor_divmod(pos.z, Config::ChunkSize);
		glm::ivec3 lpos = glm::ivec3(lx, pos.y, lz);
		int lightOpacity = block.GetDescriptor().lightOpacity;
		ChunkGroup chunks = chunk->GetNeighborChunks(true);
		int targetBlockLight = 0, targetSkylight = 0;
		if (lpos.y < Config::ChunkHeight - 1)
		{
			glm::ivec3 up = lpos + glm::ivec3(0, 1, 0);
			Light light = chunks.GetLight(up);
			targetBlockLight = light.BlockLight();
			targetSkylight = light.Skylight() + chunks.Exposed(up);
		}
		else
			targetSkylight = m_skylightLevel + 1;
		if (lpos.y > 0)
		{
			glm::ivec3 down = lpos + glm::ivec3(0, -1, 0);
			Light light = chunks.GetLight(down);
			targetBlockLight = glm::max(targetBlockLight, light.BlockLight());
			targetSkylight = glm::max(targetSkylight, light.Skylight());
		}
		for (uint32_t i = 2; i < 6; i++)
		{
			glm::ivec3 nextPos = lpos + blockNormals[i];
			Light light = chunks.GetLight(nextPos);
			targetBlockLight = glm::max(targetBlockLight, light.BlockLight());
			targetSkylight = glm::max(targetSkylight, light.Skylight());
		}
		targetBlockLight = glm::max(static_cast<int>(block.DefaultBlockLight()), targetBlockLight - 1 - lightOpacity);
		targetSkylight = glm::max(0, targetSkylight - 1 - lightOpacity);
		Light& light = chunks.GetLight(lpos);
		if (targetBlockLight > light.BlockLight())
		{
			light.BlockLight(targetBlockLight);
			chunks.IncreaseBlockLight(lpos);
		}
		else if (targetBlockLight < light.BlockLight())
			chunks.DecreaseBlockLight(lpos);
		if (targetSkylight > light.Skylight())
		{
			light.Skylight(targetSkylight);
			chunks.IncreaseSkylight(lpos);
		}
		else if (targetSkylight < light.Skylight())
			chunks.DecreaseSkylight(lpos);
		Player::UpdateMeshLater(chunk->Position() + glm::ivec2(-1, -1));
		Player::UpdateMeshLater(chunk->Position() + glm::ivec2(-1, 1));
		Player::UpdateMeshLater(chunk->Position() + glm::ivec2(1, -1));
		Player::UpdateMeshLater(chunk->Position() + glm::ivec2(1, 1));
		Player::UpdateMesh(chunk->Position());
		if (original.id != block.id)
		{
			if (lx == 0)
			{
				Player::UpdateMesh(chunk->Position() + glm::ivec2(-1, 0));
				Player::UpdateMeshLater(chunk->Position() + glm::ivec2(1, 0));
			}
			else if (lx == Config::ChunkSize - 1)
			{
				Player::UpdateMesh(chunk->Position() + glm::ivec2(1, 0));
				Player::UpdateMeshLater(chunk->Position() + glm::ivec2(-1, 0));
			}
			else
			{
				Player::UpdateMeshLater(chunk->Position() + glm::ivec2(-1, 0));
				Player::UpdateMeshLater(chunk->Position() + glm::ivec2(1, 0));
			}
			if (lz == 0)
			{
				Player::UpdateMesh(chunk->Position() + glm::ivec2(0, -1));
				Player::UpdateMeshLater(chunk->Position() + glm::ivec2(0, 1));
			}
			else if (lz == Config::ChunkSize - 1)
			{
				Player::UpdateMesh(chunk->Position() + glm::ivec2(0, 1));
				Player::UpdateMeshLater(chunk->Position() + glm::ivec2(0, -1));
			}
			else
			{
				Player::UpdateMeshLater(chunk->Position() + glm::ivec2(0, -1));
				Player::UpdateMeshLater(chunk->Position() + glm::ivec2(0, 1));
			}
		}
		else
		{
			Player::UpdateMeshLater(chunk->Position() + glm::ivec2(-1, 0));
			Player::UpdateMeshLater(chunk->Position() + glm::ivec2(1, 0));
			Player::UpdateMeshLater(chunk->Position() + glm::ivec2(0, -1));
			Player::UpdateMeshLater(chunk->Position() + glm::ivec2(0, 1));
		}
	}
	else if (updateMesh)
	{
		Player::UpdateMesh(chunk->Position());
		if (original.id != block.id)
		{
			if (lx == 0)
				Player::UpdateMesh(chunk->Position() + glm::ivec2(-1, 0));
			else if (lx == Config::ChunkSize - 1)
				Player::UpdateMesh(chunk->Position() + glm::ivec2(1, 0));
			if (lz == 0)
				Player::UpdateMesh(chunk->Position() + glm::ivec2(0, -1));
			else if (lz == Config::ChunkSize - 1)
				Player::UpdateMesh(chunk->Position() + glm::ivec2(0, 1));
		}
	}
}

bool World::EvaluateLight(glm::ivec3 const& pos, bool sky, bool block)
{
	auto [cx, lx] = floor_divmod(pos.x, Config::ChunkSize);
	auto [cz, lz] = floor_divmod(pos.z, Config::ChunkSize);
	ChunkGroup chunks = m_chunks.Get(glm::ivec2(cx, cz))->GetNeighborChunks(true);
	if (!chunks.Loaded())
		return false;

	return true;
}

void World::CastRay(glm::vec3 origin, glm::vec3 direction)
{
	s_raycastResult.blockHit = false;
	Block block = { Block::Air, 0 };
	uint32_t axis = 0;
	glm::vec3 start = origin + 0.5f;
	float distance = glm::length(direction);
	glm::vec3 dir = direction / distance;
	glm::vec3 end = glm::abs(direction / dir);
	glm::ivec3 pos = glm::floor(start);
	glm::ivec3 step;
	step.x = dir.x >= 0.0f ? 1 : -1;
	step.y = dir.y >= 0.0f ? 1 : -1;
	step.z = dir.z >= 0.0f ? 1 : -1;
	glm::vec3 walked;
	walked.x = ((dir.x >= 0.0f ? glm::floor(start.x + 1.0f) : glm::ceil(start.x - 1.0f)) - start.x) / dir.x;
	walked.y = ((dir.y >= 0.0f ? glm::floor(start.y + 1.0f) : glm::ceil(start.y - 1.0f)) - start.y) / dir.y;
	walked.z = ((dir.z >= 0.0f ? glm::floor(start.z + 1.0f) : glm::ceil(start.z - 1.0f)) - start.z) / dir.z;
	glm::vec3 delta = glm::abs(1.0f / dir);
	for (;;)
	{
		if (walked.x <= walked.y && walked.x <= walked.z) {
			if (walked.x > end.x)
				break;
			pos.x += step.x;
			axis = 0;
			walked.x += delta.x;
		}
		else if (walked.y <= walked.z) {
			if (walked.y > end.y)
				break;
			pos.y += step.y;
			axis = 1;
			walked.y += delta.y;
		}
		else {
			if (walked.z > end.z)
				break;
			pos.z += step.z;
			axis = 2;
			walked.z += delta.z;
		}
		block = GetBlock(pos);
		if (block.id == Block::Air)
			continue;
		if (block.IsFull()) // or stairs
		{
			s_raycastResult.blockHit = true;
			break;
		}
		if (block.id == Block::Unknown)
			break;
		BoundingBox const& bbox = *(s_raycastResult.bbox = &block.GetBoundingBox());
		glm::vec3 offset = pos;
		glm::vec3 t0 = (bbox.lowerLeftBack + offset - origin) / dir;
		glm::vec3 t1 = (bbox.upperRightFront + offset - origin) / dir;
		glm::vec3 tlower = glm::min(t0, t1);
		glm::vec3 tupper = glm::max(t0, t1);
		float lower = glm::fmax(tlower.x, tlower.y, tlower.z, 0.0f);
		float upper = glm::fmin(tupper.x, tupper.y, tupper.z, distance);
		if (lower > upper)
			continue;
		if (lower == tlower.x) {
			axis = 0;
			walked.x = lower;
		}
		else if (lower == tlower.y) {
			axis = 1;
			walked.y = lower;
		}
		else {
			axis = 2;
			walked.z = lower;
		}
		s_raycastResult.blockHit = true;
		break;
	}

	s_raycastResult.blockPos = pos;
	if (axis == 0) {
		s_raycastResult.hitPos = origin + (walked.x - delta.x) * dir;
		s_raycastResult.facing = dir.x >= 0.0f ? Facing::Left : Facing::Right;
	}
	else if (axis == 1) {
		s_raycastResult.hitPos = origin + (walked.y - delta.y) * dir;
		s_raycastResult.facing = dir.y >= 0.0f ? Facing::Down : Facing::Up;
	}
	else {
		s_raycastResult.hitPos = origin + (walked.z - delta.z) * dir;
		s_raycastResult.facing = dir.z >= 0.0f ? Facing::Back : Facing::Front;
	}
	s_raycastResult.block = block;
}