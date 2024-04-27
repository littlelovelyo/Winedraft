#shader vertex
#version 450 core

layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aTexCoords;
layout(location=2) in uint aFlags;
layout(location=3) in vec3 aMainColor;
layout(location=4) in vec3 aSecondaryColor;
layout(location=5) in vec2 aOffset;
layout(location=6) in vec2 aBorder;

layout(std140) uniform Screen
{
	float width, height;
};

out vec2 vTexCoords;
flat out uint vTexId;

void main()
{
	vec2 pos = aPos;
	pos.x = pos.x / width * 2.0 - 1.0;
	pos.y = 1.0 - pos.y / height * 2.0;
	gl_Position = vec4(pos, 0.0, 1.0);
	vTexCoords = aTexCoords;
	vTexId = aFlags;
}

#shader fragment
#version 450 core

in vec2 vTexCoords;
flat in uint vTexId;

layout(binding=0) uniform sampler2D textures[32];
uniform float radius;

out vec4 fColor;

void main()
{
	vec3 color = vec3(0.0, 0.0, 0.0);
	float dx = dFdx(vTexCoords.x);
	float rr = 2.0 * radius * radius;
	float sum = 0.0;
	for (int i = -int(radius); i <= int(radius); i++)
	{
		float x = clamp(vTexCoords.x + i * dx, 0.0, 1.0);
		float w = exp(-float(i * i) / rr);
		sum += w;
		color += texture(textures[vTexId], vec2(x, vTexCoords.y)).rgb * w;
	}
	fColor = vec4(color / sum, 1.0);
}