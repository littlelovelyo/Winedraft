#include "item.h"
#include "Game/config.h"

Block Item::GetCorrespondingBlock() const
{
	static uint8_t itemBlockLookupTable[] = {
		Block::Redstone
	};
	if (id < Block::Unknown)
		return { id, 0 };
	if (id <= Item::Redstone)
		return { itemBlockLookupTable[id - Item::Redstone], 0 };
	return { Block::Unknown, 0 };
}

glm::vec4 Item::GetSprite() const
{
	static uint16_t itemSpriteLookupTable[] = {
		TextureId::Redstone
	};
	constexpr float multex = 1.0f / Config::TextureSize;
	uint16_t texId = itemSpriteLookupTable[id - Redstone];
	float offtex = texId % Config::TextureSize * multex;
	float offtey = 1.0f - texId / Config::TextureSize * multex;
	return glm::vec4(offtex, offtey, multex, multex);
}

wchar_t const* Item::GetName() const
{
	static wchar_t const* itemNameLookupTable[] = {
		L"Air",
		L"Stone",
		L"Dirt",
		L"Grass Block",
		L"Acacia Planks",
		L"Birch Planks",
		L"Oak Planks",
		L"Spruce Planks",
		L"Iron Block",
		L"Gold Block",
		L"Diamond Block",
		L"Cobblestone",
		L"Redstone",
		L"Acacia Log",
		L"Birch Log",
		L"Oak Log",
		L"Spruce Log",
		L"Glass",
		L"Black Stained Glass",
		L"Blue Stained Glass",
		L"Brown Stained Glass",
		L"Cyan Stained Glass",
		L"Gray Stained Glass",
		L"Green Stained Glass",
		L"Light Blue Stained Glass",
		L"Light Gray Stained Glass",
		L"Lime Stained Glass",
		L"Magenta Stained Glass",
		L"Orange Stained Glass",
		L"Pink Stained Glass",
		L"Purple Stained Glass",
		L"Red Stained Glass",
		L"White Stained Glass",
		L"Yellow Stained Glass",
		L"Redstone Lamp",
		L"Cobblestone Slab",
		L"Stone Slab",
		L"Acacia Slab",
		L"Birch Slab",
		L"Oak Slab",
		L"Spruce Slab",
		L"Redstone",
		L"Glowstone",
		L"Unknown",
		L"Redstone"
	};
	return itemNameLookupTable[id];
}