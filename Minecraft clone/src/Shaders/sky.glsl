#shader vertex
#version 450 core

layout(location=0) in vec3 pos;

layout(std140, binding=0) uniform Camera
{
	mat4 viewMat;
	mat4 projMat;
};

out vec3 vPos;

void main()
{
	vPos = pos;
	gl_Position = (projMat * viewMat * vec4(pos, 1.0)).xyww;
}

#shader fragment
#version 450 core

layout(std140, binding=2) uniform Material
{
	vec3 fogColor;
	float fogDist;
	float time;
	uint seed;
};

in vec3 vPos;

out vec4 color;

layout(binding=0) uniform sampler2D starTex;

const float pi = 3.14159265359;

vec2 rand(vec2 pos)
{
    return fract(sin(vec2(dot(pos, vec2(127.1, 311.7)), dot(pos, vec2(269.5, 183.3)))) * 43758.5453123) * 2.0 - 1.0;
}

float grad(vec2 pos)
{
	vec2 i = floor(pos), f = fract(pos);
    vec2 u = f * f * (3.0 - 2.0 * f);
    vec2 a = rand(i), b = rand(i + vec2(0.0, 1.0)), c = rand(i + vec2(1.0, 0.0)), d = rand(i + vec2(1.0, 1.0));
    vec2 x = f, y = f - vec2(0.0, 1.0), z = f - vec2(1.0, 0.0), w = f - vec2(1.0, 1.0);
    float o = dot(a, x), p = dot(b, y), q = dot(c, z), r = dot(d, w);
    return mix(mix(o, p, u.y), mix(q, r, u.y), u.x);
}

float linearstep(float t0, float t1, float x)
{
	return clamp((x - t0) / (t1 - t0), 0.0, 1.0);
}

const vec3 asahi = vec3(1.0, 0.8, 0.2);
const vec3 hiru = vec3(1.0, 0.95, 0.9);
const vec3 night = vec3(0.1, 0.1, 0.1);
const vec3 morning = vec3(1.0, 0.9, 0.7);
const vec3 day = vec3(0.5, 0.8, 1.0);

void main()
{
	// Draw the clouds or the stars.
	float t = time / 80000.0;
	float darkness = mix(mix(0.0, 1.0, linearstep(0.25, 0.3, t)), 0.0, linearstep(0.7, 0.75, t));
	vec3 nDir = normalize(vPos);
	float cloudiness = 0.0, starness = 0.0;
	if (t > 0.25 && t < 0.75)
	{
		vec2 pos = (nDir / nDir.y).xz;
		float fogFactor = 1.0 - min(32.0 * length(pos) / fogDist, 1.0);
		pos.x += time * 0.001;
		float value = grad(pos * 2.0) + 0.15 * grad(pos * 10.0);
		cloudiness = (value + 1.15) / 2.3;
		cloudiness = step(0.0, vPos.y) * linearstep(0.4, 0.65, cloudiness) * fogFactor * darkness;
	}
	else
	{
		vec2 uv = acos(nDir.xz);
		starness = linearstep(0.03, 0.35, vPos.y) * texture(starTex, 4.0 * uv).r * (1.0 - darkness) * 0.7;
	}
	// Draw the sun.
	float theta = (t * 2.0 - 0.5) * pi;
	vec3 sunDir = vec3(0.0, sin(theta), cos(theta));
	float horizon = linearstep(-0.1, 0.0, vPos.y);
	float cosAngle = dot(nDir, sunDir);
	float sunness = horizon * smoothstep(0.990, 0.998, cosAngle);
	vec3 sunColor = mix(mix(asahi, hiru, linearstep(0.24, 0.26, t)), asahi, linearstep(0.7, 0.72, t));
	// Draw the moon.
	theta = (t * 2.0 + 0.5) * pi;
	vec3 moonDir = vec3(0.0, sin(theta), cos(theta));
	cosAngle = dot(nDir, moonDir);
	float moonness = horizon * linearstep(0.999, 0.9991, cosAngle);
	// Mix them together.
	vec3 overallColor = mix(mix(mix(mix(night, morning, linearstep(0.20, 0.26, t)), day, linearstep(0.27, 0.3, t)), morning, linearstep(0.70, 0.78, t)), night, linearstep(0.78, 0.83, t));
	overallColor = overallColor * (1.0 - starness) + vec3(starness);
	overallColor = overallColor * (1.0 - sunness) + sunColor * sunness;
	overallColor = overallColor * (1.0 - cloudiness) + vec3(cloudiness);
	overallColor = overallColor * (1.0 - moonness) + vec3(1.0, 0.9, 0.5) * moonness;
	color = vec4(overallColor, 1.0);
}