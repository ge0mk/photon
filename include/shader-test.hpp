#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shadertest
{
public:
	//program id
	unsigned int ID;

	//connstructor reads and builds shader
	Shadertest(const char* vertexPath, const char* fragmentPath);
	//use or activate shader
	void use();
	//utility uniform functions
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
};

#endif // !SHADER_H
