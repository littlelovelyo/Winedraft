#include "gl.h"
#include "Environment/memory.h"
#ifdef _DEBUG
#include <iostream>
#endif
#include <string.h>

using namespace glex;

Shader Gl::CreateShader(char const* source)
{
	Shader prog;
	prog.m_handle = glCreateProgram();
	uint32_t shaders[3] = {};
	char const* p = strstr(source, "#shader ");
	while (p != nullptr)
	{
		p += 8;
		uint32_t type;
		uint32_t index;
		if (strncmp(p, "vertex", 6) == 0)
		{
			type = GL_VERTEX_SHADER;
			index = 0;
			p += 6;
		}
		else if (strncmp(p, "geometry", 8) == 0)
		{
			type = GL_GEOMETRY_SHADER;
			index = 1;
			p += 8;
		}
		else if (strncmp(p, "fragment", 8) == 0)
		{
			type = GL_FRAGMENT_SHADER;
			index = 2;
			p += 8;
		}
		else
			throw Exception("Shader error: unknown type.");
		char* q = const_cast<char*>(strstr(p, "#shader "));
		if (q != nullptr)
			*q = 0;
		uint32_t shader = glCreateShader(type);
		glShaderSource(shader, 1, &p, nullptr);
		glCompileShader(shader);
		shaders[index] = shader;
		int status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
#ifdef _DEBUG
			std::cout << GLEX_LOG_ERROR "Error from shader compiling: ";
			int length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			if (length != 0)
			{
				char* log = reinterpret_cast<char*>(StackAllocator::Allocate(length));
				glGetShaderInfoLog(shader, length, nullptr, log);
				std::cout << log << std::endl;
				StackAllocator::Deallocate();
			}
#endif
			throw Exception("Shader error: failed to compile.");
		}
		if (q != nullptr)
			*q = '#';
		glAttachShader(prog.m_handle, shader);
		glDeleteShader(shader);
		p = q;
	}
	glLinkProgram(prog.m_handle);
	int status;
	glGetProgramiv(prog.m_handle, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
#ifdef _DEBUG
		int length;
		glGetProgramiv(prog.m_handle, GL_INFO_LOG_LENGTH, &length);
		if (length != 0)
		{
			std::cout << GLEX_LOG_ERROR "Error from shader linking: ";
			char* log = reinterpret_cast<char*>(StackAllocator::Allocate(length));
			glGetProgramInfoLog(prog.m_handle, length, nullptr, log);
			std::cout << log << std::endl;
			StackAllocator::Deallocate();
		}
#endif
		throw Exception("Shader error: failed to link.");
	}
#ifdef _DEBUG
	s_programCount++;
#endif
	for (uint32_t i = 0; i < 3; i++)
	{
		if (shaders[i] != 0)
			glDetachShader(prog.m_handle, shaders[i]);
	}
	return prog;
}

void Gl::DestroyShader(Shader& shader)
{
	glDeleteProgram(shader.m_handle);
#ifdef _DEBUG
	s_programCount--;
#endif
}