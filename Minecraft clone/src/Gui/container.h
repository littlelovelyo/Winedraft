#pragma once
#include "Data/item.h"
#include "glex.h"

class Container
{
protected:
	inline static Item s_itemHolding;
	glex::Texture m_texture;
	float m_width, m_height;
	uint32_t m_numAeras;
	struct Aera
	{
		float x, y;
		uint32_t rows, cols;
		Item* items;
		bool(*filter)(uint32_t idx, Item item);
		void(*take)();
	};
protected:
	Container(char const* texture, float width, float height, uint32_t numAeras);
public:
	bool HoldingItem() { return s_itemHolding.id != Block::Air; }
	void Draw();
	~Container();
};

class Inventory : public Container
{
private:
	Aera m_aeras[4];
public:
	Inventory();
};