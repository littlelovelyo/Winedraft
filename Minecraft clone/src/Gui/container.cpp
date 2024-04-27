#include "container.h"
#include "Game/config.h"
#include "Entity/player.h"
#include "Gui/gui.h"

Container::Container(char const* texture, float width, float height, uint32_t numAeras) : m_width(width), m_height(height), m_numAeras(numAeras)
{
	glex::TextureInfo tf;
	tf.filterMag = glex::Texture::Nearest;
	m_texture = glex::Gl::CreateTexture("./res/inventory.png", tf);
}

Container::~Container()
{
	glex::Gl::DestroyTexture(m_texture);
}

void Container::Draw()
{
	// mouse interaction
	constexpr float texScale = 72.0f / 16.0f;
	float guiScale = Config::GuiScale();
	float bgScale = texScale * guiScale;
	float innerSize = 72.0f * guiScale;
	float outerSize = 81.0f * guiScale;
	float scrWidth = glex::Context::DefaultFrameBuffer().Width();
	float scrHeight = glex::Context::DefaultFrameBuffer().Height();
	float width = m_width * bgScale, height = m_height * bgScale;
	float x = (scrWidth - width) / 2.0f, y = (scrHeight - height) / 2.0f;
	float cx = glex::Input::MouseX(), cy = glex::Input::MouseY();
	int px = (cx - x) / bgScale, py = (cy - y) / bgScale;
	glex::Renderer::DrawQuad(glm::vec4(x, y, x + width, y + height), m_texture,
		glm::vec4(0.0f, 1.0f, m_width / 256.0f, m_height / 256.0f));
	Aera* aeras = reinterpret_cast<Aera*>(&m_numAeras + 2);
	for (uint32_t i = 0; i < m_numAeras; i++)
	{
		Aera& aera = aeras[i];
		int ax = aera.x, ay = aera.y;
		if (px >= ax && py >= ay && px < ax + aera.cols * 18 && py < ay + aera.rows * 18)
		{
			uint32_t rres = (py - ay) % 18, cres = (px - ax) % 18;
			if (rres < 16 && cres < 16)
			{
				uint32_t row = (py - aera.y) / 18, col = (px - aera.x) / 18;
				float qx = x + (aera.x + col * 18) * bgScale;
				float qy = y + (aera.y + row * 18) * bgScale;
				glex::Renderer::DrawQuad(glm::vec4(qx, qy, qx + innerSize, qy + innerSize), glm::vec4(1.0f, 1.0f, 1.0f, 0.45f));
				uint32_t idx = row * aera.cols + col;
				Item& slot = *(aera.items + idx);
				if (glex::Input::Pressed(glex::Input::LMB))
				{
					if (s_itemHolding.id == Block::Air)
					{
						if (slot.id != Block::Air)
						{
							s_itemHolding = slot;
							slot = { Block::Air, 0 };
							if (aera.take != nullptr)
								aera.take();
						}
					}
					else if (slot.id == Block::Air)
					{
						if (aera.filter == nullptr || aera.filter(idx, s_itemHolding))
						{
							slot = s_itemHolding;
							s_itemHolding = { Block::Air, 0 };
						}
					}
					else
					{
						if (aera.filter == nullptr || aera.filter(idx, s_itemHolding))
						{
							std::swap(s_itemHolding, slot);
							if (aera.take)
								aera.take();
						}
					}
				}
			}
			break;
		}
	}
	// draw items in container
	Gui::StartItemDrawing();
	for (uint32_t i = 0; i < m_numAeras; i++)
	{
		Aera& aera = aeras[i];
		Item* item = aera.items;
		float x0 = x + aera.x * bgScale, ix = x0, iy = y + aera.y * bgScale;
		for (uint32_t row = 0; row < aera.rows; row++)
		{
			for (uint32_t col = 0; col < aera.cols; col++)
			{
				if (item->id != Block::Air)
					Gui::DrawItem(glm::vec3(ix, iy, innerSize), *item);
				item++;
				ix += outerSize;
			}
			ix = x0;
			iy += outerSize;
		}
	}
	// draw item holding
	if (s_itemHolding.id != Block::Air)
		Gui::DrawItem(glm::vec3(cx - innerSize / 2.0f, cy - innerSize / 2.0f, innerSize), s_itemHolding);
	Gui::ItemDrawingDone();
}

static bool RejectAll(uint32_t idx, Item item)
{
	return false;
}

static bool FindRecipe(uint32_t idx, Item item)
{

	return true;
}

static void Craft()
{

}

Inventory::Inventory() : Container("./res/inventory.png", 248.0f, 184.0f, 4), m_aeras {
	{ 134.0f, 18.0f, 2, 2, Player::s_crafting, FindRecipe, nullptr },
	{ 207.0f, 28.0f, 1, 1, &Player::s_craftingOutput, RejectAll, Craft },
	{ 8.0f, 84.0f, Config::InventoryRows, Config::HotbarSlots, reinterpret_cast<Item*>(Player::s_inventory), nullptr, nullptr },
	{ 8.0f, 160.0f, 1, Config::HotbarSlots, Player::s_hotbar, nullptr, nullptr } } {}