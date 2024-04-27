#include "panel.h"
#include "Game/config.h"
#include "Game/render.h"

void Panel::AddLabel(glm::vec4 const& border, wchar_t const* text, float size, int horizontalAlignment, int verticalAlignment)
{
	m_labelList.emplace_back(border, text, size, horizontalAlignment, verticalAlignment);
	m_width = glm::max(m_width, border.x + border.z);
	m_height = glm::max(m_height, border.y + border.w);
}

void Panel::AddButton(glm::vec4 const& border, wchar_t const* text, float size, void(*callback)())
{
	m_buttonList.emplace_back(border, text, callback, size, ++s_mid);
	m_width = glm::max(m_width, border.x + border.z);
	m_height = glm::max(m_height, border.y + border.w);
}

void Panel::AddIntegerSlider(glm::vec4 const& border, uint32_t minValue, uint32_t maxValue, uint32_t& value)
{
	m_isliderList.emplace_back(border, &value, minValue, maxValue, ++s_mid);
	m_width = glm::max(m_width, border.x + border.z);
	m_height = glm::max(m_height, border.y + border.w);
}

/* void Panel::AddFloatSlider(glm::vec4 const& border, float minValue, float maxValue, float& value)
{
	m_fsliderList.emplace_back(border, &value, minValue, maxValue, ++s_mid);
	m_width = glm::max(m_width, border.x + border.z);
	m_height = glm::max(m_height, border.y + border.w);
} */

void Panel::Draw(glm::vec4 const& border, int horizontalAlignment, int verticalAlignment)
{
	float scale = Config::GuiScale();
	float cx = glex::Input::MouseX(), cy = glex::Input::MouseY();
	float x, y;
	switch (horizontalAlignment)
	{
		case 0: x = border.x + (border.z - border.x - m_width * scale) / 2.0f; break;
		case 1: x = border.x + border.z - border.x - m_width * scale; break;
		default: x = border.x;
	}
	switch (verticalAlignment)
	{
		case 0: y = border.y + (border.w - border.y - m_height * scale) / 2.0f; break;
		case 1: y = border.y + border.w - border.y - m_height * scale; break;
		default: y = border.y;
	}
	for (Label& label : m_labelList)
	{
		glm::vec4 border = label.border * scale;
		border.x += x;
		border.y += y;
		border.z += border.x;
		border.w += border.y;
		float size = label.size * scale;
		glex::Renderer::DrawText(border, Renderer::s_font, label.text, size, size * Config::Flatness,
			Config::TextSpace, Config::LineSpace, 0.0f, Config::DeepColor, label.horizontalAlignment, label.verticalAlignement);
	}
	bool noButtonHit = true;
	float sfactor = glm::min(1.0f, glex::Time::DeltaTime() * 0.03f);
	float afactor = glm::min(1.0f, glex::Time::DeltaTime() * 0.01f);
	glex::Renderer::DrawQuad(Button::backgroundBorder, Renderer::s_widgets, Renderer::s_button, 20.0f, 0.078125f, Button::alpha);
	for (Button& button : m_buttonList)
	{
		glm::vec4 border = button.border * scale;
		border.x += x;
		border.y += y;
		border.z += border.x;
		border.w += border.y;
		glm::vec4 color;
		if (s_mouseOwner == button.mid)
		{
			color = Config::WhiteColor;
			glm::vec4 bgBorder = border;
			float vOffset = 5.0f * scale;
			float hOffset = 20.0f * scale;
			bgBorder.x += hOffset;
			bgBorder.y += vOffset;
			bgBorder.z -= hOffset;
			bgBorder.w -= vOffset;
			Button::backgroundBorder = (1.0f - sfactor) * Button::backgroundBorder + sfactor * bgBorder;
			Button::alpha = (1.0f - afactor) * Button::alpha + afactor * 1.0f;
			if (glex::Input::Released(glex::Input::LMB))
			{
				s_mouseOwner = 0;
				if (cx >= border.x && cy >= border.y && cx <= border.z && cy <= border.w)
					button.callback();
			}
			else
				noButtonHit = false;
		}
		else
		{
			if (s_mouseOwner == 0 && cx >= border.x && cy >= border.y && cx <= border.z && cy <= border.w)
			{
				noButtonHit = false;
				if (glex::Input::Pressed(glex::Input::LMB))
					s_mouseOwner = button.mid;
				Button::backgroundBorder = (1.0f - sfactor) * Button::backgroundBorder + sfactor * border;
				Button::alpha = (1.0f - afactor) * Button::alpha + afactor * 1.0f;
				color = Config::WhiteColor;
			}
			else
				color = Config::DeepColor;
		}
		float size = button.size * scale;
		glex::Renderer::DrawText(border, Renderer::s_font, button.text, size,
			size * Config::Flatness, Config::TextSpace, Config::LineSpace, 0.0f, color, 0, 0);
	}
	if (noButtonHit)
		Button::alpha = (1.0f - afactor) * Button::alpha + afactor * 0.0f;
	for (IntegerSlider& slider : m_isliderList)
	{
		glm::vec4 border = slider.border * scale;
		border.x += x;
		border.y += y;
		float mid = border.x + border.z / (slider.maxValue - slider.minValue) * (*slider.value - slider.minValue);
		float height = border.w;
		float width = height / 24.0f * 14.0f;
		float left = mid - width / 2.0f, right = left + width;
		border.w += border.y;
		if (glex::Input::Pressed(glex::Input::LMB) && cx >= left && cx <= right && cy >= border.y && border.w)
		{
			s_mouseOwner = slider.mid;
			IntegerSlider::offset = cx - left;
		}
		if (s_mouseOwner == slider.mid)
		{
			if (glex::Input::Released(glex::Input::LMB))
				s_mouseOwner = 0;
			else
				*slider.value = static_cast<uint32_t>(glm::clamp(cx - IntegerSlider::offset - border.x, 0.0f, border.z - width) / (border.z - width) * (slider.maxValue - slider.minValue) + 0.5f)+ slider.minValue;
		}
		border.z += border.x;
		glex::Renderer::DrawQuad(border,
			Renderer::s_widgets, Renderer::s_slider, height / 2.0f, 0.046875f);
		border.z = mid;
		glex::Renderer::DrawQuad(border, Renderer::s_widgets, Renderer::s_sliderFill, height / 2.0f, 0.046875f);
		border.x = left, border.z = right;
		glex::Renderer::DrawQuad(border, Renderer::s_widgets, Renderer::s_thumb);
	}
	/* for (FloatSlider& slider : m_fsliderList)
	{
		glm::vec4 border = slider.border * scale;
		border.x += x;
		border.y += y;
		float mid = border.x + border.z / (slider.maxValue - slider.minValue) * (*slider.value - slider.minValue);
		float height = border.w;
		float width = height / 24.0f * 14.0f;
		float left = mid - width / 2.0f, right = left + width;
		border.w += border.y;
		if (glex::Input::Pressed(glex::Input::LMB) && cx >= left && cx <= right && cy >= border.y && border.w)
		{
			s_mouseOwner = slider.mid;
			IntegerSlider::offset = cx - left;
		}
		if (s_mouseOwner == slider.mid)
		{
			if (glex::Input::Released(glex::Input::LMB))
				s_mouseOwner = 0;
			else
				*slider.value = glm::clamp(cx - IntegerSlider::offset - border.x, 0.0f, border.z - width) / (border.z - width) * (slider.maxValue - slider.minValue) + slider.minValue;
		}
		border.z += border.x;
		glex::Renderer::DrawQuad(border, Game::s_widgets, Game::s_slider, height / 2.0f, 0.046875f);
		border.z = mid;
		glex::Renderer::DrawQuad(border, Game::s_widgets, Game::s_sliderFill, height / 2.0f, 0.046875f);
		border.x = left, border.z = right;
		glex::Renderer::DrawQuad(border, Game::s_widgets, Game::s_thumb);
	} */
}