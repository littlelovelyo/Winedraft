#include <glm/glm.hpp>

struct Label
{
	glm::vec4 border;
	wchar_t const* text;
	float size;
	int8_t horizontalAlignment, verticalAlignement;
};

struct Button
{
	glm::vec4 border;
	wchar_t const* text;
	void(*callback)();
	float size;
	uint32_t mid;
	inline static float alpha;
	inline static glm::vec4 backgroundBorder;
};

struct IntegerSlider
{
	glm::vec4 border;
	uint32_t* value;
	uint32_t minValue, maxValue;
	uint32_t mid;
	inline static float offset;
};

/* struct FloatSlider
{
	glm::vec4 border;
	float* value;
	float minValue, maxValue;
	uint32_t mid;
}; */