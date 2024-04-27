#shader vertex
#version 450 core

layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aTexCoords;
layout(location=2) in uint aFlags;
layout(location=3) in vec4 aMainColor;
layout(location=4) in vec3 aSecondaryColor;		// uMin, vMin, uLength
layout(location=5) in vec2 aOffset;				// texAspect, scrAspect
layout(location=6) in vec2 aBorder;				// texBorder, scrBorder

layout(std140, binding=3) uniform Screen
{
	float width, height;
};

out vec2 vTexCoords;
flat out uint vFlags;
out vec4 vMainColor;
out vec3 vSecondaryColor;
out vec2 vOffset;
out vec2 vBorder;

void main()
{
	vec2 pos = aPos;
	pos.x = pos.x / width * 2.0 - 1.0;
	pos.y = 1.0 - pos.y / height * 2.0;
	gl_Position = vec4(pos, 0.0, 1.0);
	vTexCoords = aTexCoords;
	vFlags = aFlags;
	vMainColor = aMainColor;
	vSecondaryColor = aSecondaryColor;
	vOffset = aOffset;
	vBorder = aBorder;
}

#shader fragment
#version 450 core

in vec2 vTexCoords;
flat in uint vFlags;
in vec4 vMainColor;
in vec3 vSecondaryColor;	// uMin, vMin, uLength
in vec2 vOffset;			// texAspect, scrAspect
in vec2 vBorder;			// texBorder, scrBorder

layout(binding=0) uniform sampler2D textures[32];

out vec4 fColor;

float linearMap(float val, float origMin, float origMax, float newMin, float newMax) {
	return (val - origMin) / (origMax - origMin) * (newMax - newMin) + newMin;
}

float processAxis(float coord, float origMin, float origMax, float texBorder, float wndBorder) {
	if (coord < origMin + wndBorder)
		return linearMap(coord, origMin, origMin + wndBorder, origMin, origMin + texBorder);
	if (coord > origMax - wndBorder)
		return linearMap(coord, origMax - wndBorder, origMax, origMax - texBorder, origMax);
	return linearMap(coord, origMin + wndBorder, origMax - wndBorder, origMin + texBorder, origMax - texBorder);
}

void main()
{
	uint texId = vFlags & 0x1f;
	uint flags = vFlags >> 5;
	if (flags == 0)
	{
		/**
		 * flags = 0: plain quad, with 9-slice support
		 * in vec3 vSecondaryColor;	= uMin, vMin, uLength
		 * in vec2 vOffset;			= vLength, texBorder
		 * in vec2 vBorder;			= scrBorderx, scrBordery
		 * suppose the aspect ratios are the same
		 */
		vec2 u = vec2(vSecondaryColor.x, vSecondaryColor.x + vSecondaryColor.z);
		vec2 v = vec2(vSecondaryColor.y, vSecondaryColor.y + vOffset.x);
		float texBorder = vOffset.y;
		vec2 scrBorder = vec2(vBorder.x * vSecondaryColor.z, vBorder.y * vOffset.x);
		vec2 texCoords;
		texCoords.x = processAxis(vTexCoords.x, u.x, u.y, texBorder, scrBorder.x);
		texCoords.y = processAxis(vTexCoords.y, v.x, v.y, texBorder, scrBorder.y);
		vec4 color = texture(textures[texId], texCoords);
		fColor = color * vMainColor;
	}
	else if (flags == 1)
	{
		// flags = 1: sdf text rendering
		float strokeDist = 1.0 - texture(textures[texId], vTexCoords).a;
		float strokeAlpha = 1.0 - smoothstep(0.5, 0.5 + vBorder.y, strokeDist);
		float borderDist = 1.0 - texture(textures[texId], vTexCoords + vOffset).a;
		float borderAlpha = 1.0 - smoothstep(0.5 + vBorder.x, 0.5 + vBorder.x + vBorder.y, borderDist);
		float alpha = strokeAlpha + (1.0 - strokeAlpha) * borderAlpha;
		vec3 color = mix(vSecondaryColor, vMainColor.rgb, strokeAlpha / alpha);
		fColor = vec4(color, alpha * vMainColor.a);
	}
	else
		fColor = vMainColor;
}