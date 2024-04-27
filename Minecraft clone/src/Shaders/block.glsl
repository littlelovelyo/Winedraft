#shader vertex
#version 450 core

layout(location=0) in vec3 pos;
layout(location=1) in vec2 uv;
layout(location=2) in vec3 normal;

layout(std140, binding=0) uniform Camera
{
	mat4 viewMat;
	mat4 projMat;
};

out vec2 texCoords;
flat out vec3 norm;

void main()
{
	gl_Position = projMat * vec4(pos, 1.0);
	texCoords = uv;
	norm = normal;
}

#shader fragment
#version 450 core

in vec2 texCoords;
flat in vec3 norm;

const vec3 MagicLight = vec3(0.6, 1.0, 0.8);

layout(binding=0) uniform sampler2D blockTex;

out vec4 color;

void main()
{
	float diffuse = max(0.0, dot(MagicLight, norm));
	vec4 base = texture(blockTex, texCoords);
	color = vec4(base.rgb * diffuse, base.a);
}