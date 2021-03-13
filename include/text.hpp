#pragma once

#include <memory>
#include <string>
#include <vector>
#include <fstream>

#include <math/matrix.hpp>
#include <math/vector.hpp>

#include <opengl/buffer.hpp>
#include <opengl/mesh.hpp>
#include <opengl/program.hpp>
#include <opengl/uniform.hpp>
#include <opengl/texture.hpp>
#include <opengl/vao.hpp>

#include <freetype/freetype.hpp>

#include <utils/image.hpp>

using namespace math;

class TextObject {
public:
	TextObject(const std::string &text, mat4 transform, vec4 color);

private:
	friend class TextRenderer;

	std::string text;
	mat4 transform;
	vec4 color;
};

class TextRenderer {
public:
	using Vertex = opengl::Vertex<vec3, vec2, vec4>;
	using Mesh = opengl::Mesh<vec3, vec2, vec4>;

	TextRenderer(freetype::Font &&font);

	std::shared_ptr<TextObject> createObject(const std::string &text, mat4 transform, vec4 color);
	void removeObject(const std::shared_ptr<TextObject> &object);

	void update();
	void render(mat4 transform);

private:
	freetype::Font font;
	std::vector<std::shared_ptr<TextObject>> objects;

	opengl::Program prog;
	opengl::Texture texture;
	opengl::UniformBuffer<mat4> transformUBO;

	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;
	std::atomic<bool> changed;
	std::mutex objectMutex, renderMutex;
	Mesh mesh;
};