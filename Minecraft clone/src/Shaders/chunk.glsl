#shader vertex
#version 450 core

layout(location=0) in vec3 pos;
layout(location=1) in vec2 uv;
layout(location=2) in uint light;

layout(std140, binding=0) uniform Camera
{
	mat4 viewMat;
	mat4 projMat;
};

layout(std140, binding=1) uniform Transform
{
	mat4 modelMat;
};

layout(std140, binding=2) uniform Material
{
	vec3 fogColor;
	float fogDist;
	float time;
	uint seed;
};

out vec2 texCoords;
out vec4 eyeSpacePosition;

float linearstep(float t0, float t1, float x)
{
	return clamp((x - t0) / (t1 - t0), 0.0, 1.0);
}

float getLight(uint level)
{
	return log(mix(1.1, 2.718281828, float(level) / 65535.0));
}

void main()
{
	eyeSpacePosition = viewMat * modelMat * vec4(pos, 1.0);
	gl_Position = projMat * eyeSpacePosition;
	texCoords = uv;
	float t = time / 80000.0;
	float sunlight = 0.5 * (linearstep(0.2, 0.3, t) + linearstep(0.83, 0.78, t));
	float skylight = sunlight * getLight(light >> 16), blockLight = getLight(light & 0xffff);
	eyeSpacePosition.y = max(skylight, blockLight);
}

#shader fragment
#version 450 core

in vec2 texCoords;
in vec4 eyeSpacePosition;

layout(std140, binding=2) uniform Material
{
	vec3 fogColor;
	float fogDist;
	float time;
	uint seed;
};

layout(binding=0) uniform sampler2D blockTex;

out vec4 color;

const vec3 night = vec3(0.1, 0.1, 0.1);
const vec3 morning = vec3(1.0, 0.9, 0.7);
const vec3 day = vec3(0.5, 0.8, 1.0);

float linearstep(float t0, float t1, float x)
{
	return clamp((x - t0) / (t1 - t0), 0.0, 1.0);
}

void main()
{
	vec4 texColor = texture(blockTex, texCoords);
	if (texColor.a < 0.5)
		discard;
	vec3 baseColor = texColor.rgb * eyeSpacePosition.y;
	float dist = abs(eyeSpacePosition.z / eyeSpacePosition.w) / fogDist;
	float fogFactor = exp(-20.0 * pow(dist - 1.0, 2));
	float t = time / 80000.0;
	vec3 skyColor = mix(mix(mix(mix(night, morning, linearstep(0.20, 0.26, t)), day, linearstep(0.27, 0.3, t)), morning, linearstep(0.70, 0.78, t)), night, linearstep(0.78, 0.83, t));
	color = vec4(mix(baseColor, skyColor, fogFactor), 1.0);
}