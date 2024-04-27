#shader vertex
#version 450 core

layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aTexCoords;

out vec2 vTexCoords;

void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
	vTexCoords = aTexCoords;
}

#shader fragment
#version 450 core

in vec2 vTexCoords;

layout(binding=0) uniform sampler2D accumTex;
layout(binding=1) uniform sampler2D revealageTex;

out vec4 fColor;

void main()
{
	vec4 accum = texture(accumTex, vTexCoords);
	float revealage = texture(revealageTex, vTexCoords).r;
	fColor = vec4(accum.rgb / accum.a, revealage);
}