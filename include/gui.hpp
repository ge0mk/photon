#pragma once

#include "text.hpp"
#include <glfw/glfw3.h>

#include <array>
//#include <format>

template<typename ...Components>
class GuiMesh {
public:
	using Vertex = opengl::Vertex<Components...>;

	void clear() {
		vertices.clear();
		indices.clear();
		triangles = quads = vertexCount = indexCount = 0;
	}

	void update() {
		mesh.setData(vertices, indices);
	}

	void render() {
		mesh.drawElements();
	}

	void addTriangle(const Vertex &a, const Vertex &b, const Vertex &c) {
		unsigned ia = addVertex(a);
		unsigned ib = addVertex(b);
		unsigned ic = addVertex(c);

		addTriangle(ia, ib, ic);
	}

	void addTriangle(unsigned ia, unsigned ib, unsigned ic) {
		addIndices(ia, ib, ic);
		triangles++;
	}

	void addQuad(const Vertex &a, const Vertex &b, const Vertex &c, const Vertex &d) {
		unsigned ia = addVertex(a);
		unsigned ib = addVertex(b);
		unsigned ic = addVertex(c);
		unsigned id = addVertex(d);

		addQuad(ia, ib, ic, id);
	}

	void addQuad(unsigned ia, unsigned ib, unsigned ic, unsigned id) {
		addTriangle(ia, ib, ic);
		addTriangle(ic, id, ia);
		quads++;
	}

	unsigned addVertex(const Vertex &v) {
		unsigned index = vertices.size();
		vertices.push_back(v);
		vertexCount++;
		return index;
	}

	void addIndex(unsigned index) {
		indices.push_back(index);
		indexCount++;
	}

	void addIndices() {}

	template<typename ...Args>
	void addIndices(unsigned index, Args... next) {
		indices.push_back(index);
		addIndices(next...);
		indexCount++;
	}

private:
	friend class GuiSystem;

	int triangles, quads, vertexCount, indexCount;
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	opengl::IndexedMesh<Components...> mesh;
};

struct GuiStyle {
	struct Palette {
		vec4 text, background;
		vec4 primary, secondary;
		vec4 error, warning, info, success;
	} palette;

	struct PaletteModifiers {
		vec4 active, hovered, selected, disabled;
	} modifiers;

	float rounding;
	// corner rounding [0-1]
	int segments;
	// corner segments
	vec4 margin, border, padding;
	// size of (margin/border/padding) (left|top|right|bottom)
	// margin: outside of border, padding: inside border
	vec2 spacing, alignment, grid;
	// spacing: distance between children
	// alignment: (0|0) -> top left, (0|1) -> bottom left, (1|1) -> bottom right, (.5|.5) center
	// grid: number of items per row/column, 0 -> inf.
};

class GuiIO {
public:
	void onKeyTyped(int key);
	void onKeyChanged(int key, int scancode, int modifier, int action);
	void onKeyPressed(int key, int scancode, int modifier, bool repeat);
	void onKeyReleased(int key, int scancode, int modifier);
	void onMouseMoved(vec2 pos, vec2 dir);
	void onMousePressed(int button, int modifier);
	void onMouseReleased(int button, int modifier);
	void onScrollEvent(vec2 dir);
	void onWindowContentScaleChanged(vec2 scale);
	void onWindowFocusChanged(bool focussed);
	void onFramebufferResized(ivec2 size);

	bool keyPressed(int key);
	bool keyReleased(int key);
	bool keyJustReleased(int key);

	bool buttonPressed(int button);
	bool buttonReleased(int button);
	bool buttonJustReleased(int button);

	void beginFrame();
	void endFrame();

	bool usesMouse();
	bool usesKeyboard();

protected:
	vec2 mousePos, dmouse;
	vec2 frameBufferSize, contentScale;
	float time, dt;
	float depth;
	bool focus, mouse, keyboard;

	std::array<uint8_t, GLFW_KEY_LAST + 1> keys;
	std::array<uint8_t, GLFW_MOUSE_BUTTON_LAST + 1> buttons;
};

class GuiStyleStack {
public:
	void pushStyle(const GuiStyle &style);
	GuiStyle& currentStyle();
	GuiStyle& defaultStyle();
	void popStyle();

private:
	std::vector<GuiStyle> styles;
	GuiStyle m_defaultStyle;
};

class GuiSystem : public GuiIO, GuiStyleStack {
public:
	using Vertex = opengl::Vertex<vec3, vec4>;

	GuiSystem(freetype::Font &&font);

	void beginFrame(float time, float dt);
	void endFrame();
	void render(mat4 transform = mat4());

	void rect(vec2 tl, vec2 br, vec4 color, float z = 0.0f);
	void circle(vec2 center, float radius, float innerRadius, vec4 color, int segments = 0, float z = 0.0f);
	void circleSegment(vec2 center, float radius, float innerRadius, float astart, float aend, vec4 color, int segments = 0, float z = 0.0f);
	vec2 text(const std::string &text, vec2 pos, vec4 color = vec4(1.0f), vec2 scale = vec2(1.0f), float z = 0.0f);
	bool button(const std::string &text, vec2 pos, vec4 bgcolor, vec4 textcolor = vec4(1.0f), float z = 0.0f);

	template<typename ...Args>
	void text(const std::string &fmt, vec2 pos, vec4 color = vec4(1.0f), vec2 scale = vec2(1.0f), float z = 0.0f, Args ...args) {
		// TODO: replace with std::format when support is there
		text(fmt::format(fmt, args...), pos, color, scale, z);
	}


	// styled widgets
	//void rect(vec2 pos, vec2 size);
	//void border(vec2 pos, vec2 size);

	// more complex functions
	void empty(vec2 size);
	void text(const std::string &text);
	bool button(const std::string &text);

private:
	GuiMesh<vec3, vec4> mesh;

	opengl::UniformBuffer<mat4> transformUBO;
	opengl::Program prog;

	TextRenderer textRenderer;
};