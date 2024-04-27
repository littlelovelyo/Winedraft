#shader vertex
#version 450 core

layout(location=0) in vec3 pos;
layout(location=1) in vec2 uv;

layout(std140, binding=0) uniform Camera
{
	mat4 viewMat;
	mat4 projMat;
};

layout(std140, binding=1) uniform Transform
{
	mat4 modelMat;
};

out vec2 texCoords;
out vec4 eyeSpacePosition;

void main()
{
	eyeSpacePosition = viewMat * modelMat * vec4(pos, 1.0);
	gl_Position = projMat * eyeSpacePosition;
	texCoords = uv;
}

#shader fragment
#version 450 core

in vec2 texCoords;
in vec4 eyeSpacePosition;

layout(std140, binding=2) uniform Material
{
	vec3 fogColor;
	float fogDist;
};

layout(binding=0) uniform sampler2D blockTex;

layout(location=0) out vec4 accum;
layout(location=1) out vec4 revealage;

void main()
{
	float dist = abs(eyeSpacePosition.z / eyeSpacePosition.w) / fogDist;
	float fogFactor = exp(-20.0 * pow(dist - 1.0, 2));
	vec4 texColor = texture(blockTex, texCoords);
	vec3 color = mix(texColor.rgb, fogColor, fogFactor);

	float distWeight = exp(-8.0 * dist);
	float intensity = length(texColor.rgb);
	float colorCorrection = exp(-0.8 * intensity) * (1.0 - texColor.a);
	float alphaCorrection = texColor.a;
	float weight = 0.1 * distWeight * colorCorrection * alphaCorrection;
	accum = vec4(color, texColor.a) * weight;
	revealage = texColor.aaaa;
}