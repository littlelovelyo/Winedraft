#pragma once
#include "block.h"

/**
 * 0 ~ Block::Unknown represents blocks
 * Block::Unknown ~ 65535 represents items
 * if item is a tool, data represents its durability
 * else its stack number
 */
struct Item
{
	uint16_t id;
	uint16_t data;
	enum : uint16_t {
		Redstone = Block::Unknown + 1
	};
	Block GetCorrespondingBlock() const;
	glm::vec4 GetSprite() const;
	wchar_t const* GetName() const;
	bool IsBlock() const {
		return id <= Block::Unknown;
	}
	bool IsTool() const {
		return false;
	}
	Block& AsBlock() {
		return *reinterpret_cast<Block*>(this);
	}
	Block const& AsBlock() const {
		return *reinterpret_cast<Block const*>(this);
	}
	void Consume() {
		if (--data == 0)
			id = Block::Air;
	}
};