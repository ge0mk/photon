#pragma once

#include <fstream>
#include <string>
#include <vector>

#include <math/matrix.hpp>
#include <math/vector.hpp>
#include <math/quaternion.hpp>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

template<uint64_t type>
unsigned loadShader(std::string filename) {
	std::ifstream file(filename, std::ios::ate);
	size_t size = file.tellg();
	std::string source(size, '\0');
	file.seekg(0, std::ios::beg);
	file.read(&source[0], size);

	unsigned shader = glCreateShader(type);
	const char *data = source.c_str();
	glShaderSource(shader, 1, &data, nullptr);
	glCompileShader(shader);

	int  success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		spdlog::error("shader compilation failed ({}): {}", filename, infoLog);
	}
	return shader;
}

unsigned loadProgram(std::string filename) {
	unsigned vertexShader = loadShader<GL_VERTEX_SHADER>(filename + ".vert");
	unsigned fragmentShader = loadShader<GL_FRAGMENT_SHADER>(filename + ".frag");
	unsigned prog = glCreateProgram();
	glAttachShader(prog, vertexShader);
	glAttachShader(prog, fragmentShader);
	glLinkProgram(prog);

	int  success;
	char infoLog[512];
	glGetProgramiv(prog, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(prog, 512, NULL, infoLog);
		spdlog::error("shader linking failed ({}): {}", filename, infoLog);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return prog;
}