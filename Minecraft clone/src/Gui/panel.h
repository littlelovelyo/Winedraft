#pragma once
#include "control.h"
#include <vector>

class Panel
{
private:
	inline static uint32_t s_mid = 0;
	inline static uint32_t s_mouseOwner = 0;
	float m_width = 0.0f, m_height = 0.0f;
	std::vector<Label> m_labelList;
	std::vector<Button> m_buttonList;
	std::vector<IntegerSlider> m_isliderList;
	// std::vector<FloatSlider> m_fsliderList;
public:
	void AddLabel(glm::vec4 const& border, wchar_t const* text, float size, int horizontalAlignment, int verticalAlignment);
	void AddButton(glm::vec4 const& border, wchar_t const* text, float size, void(*callback)());
	void AddIntegerSlider(glm::vec4 const& border, uint32_t minValue, uint32_t maxValue, uint32_t& value);
	// void AddFloatSlider(glm::vec4 const& border, float minValue, float maxValue, float& value);
	void Draw(glm::vec4 const& border, int horizontalAlignment, int verticalAlignment);
};