#shader vertex
#version 450 core

layout(location=0) in vec3 pos;

layout(std140, binding=0) uniform Camera
{
	mat4 viewMat;
	mat4 projMat;
};

layout(std140, binding=1) uniform Transform
{
	mat4 modelMat;
};

void main()
{
	vec4 position = projMat * viewMat * modelMat * vec4(pos, 1.0);
	position.z -= 0.0005; // Normalized Device Coordinate is left-handed for some reason
	gl_Position = position;
}

#shader geometry
#version 450 core

layout(triangles) in;
layout(line_strip, max_vertices=3) out;

void main()
{
	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;
	// if (cross((p0.xyz - p1.xyz), (p2.xyz - p1.xyz)).z < 0.0)
	// {
		gl_Position = p0;
		EmitVertex();
		gl_Position = p1;
		EmitVertex();
		gl_Position = p2;
		EmitVertex();
	// }
	EndPrimitive();
}

#shader fragment
#version 450 core

out vec4 color;

void main()
{
	color = vec4(0.0, 0.0, 0.0, 1.0);
}