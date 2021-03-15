#pragma once

#include "text.hpp"
#include <glfw/glfw3.h>

#include <array>

template<typename ...Components>
class DrawList {
public:
	using Vertex = opengl::Vertex<Components...>;

	void clear() {
		vertices.clear();
		indices.clear();
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
	}

	unsigned addVertex(const Vertex &v) {
		unsigned index = vertices.size();
		vertices.push_back(v);
		return index;
	}

	void addIndex(unsigned index) {
		indices.push_back(index);
	}

	void addIndices() {}

	template<typename ...Args>
	void addIndices(unsigned index, Args... next) {
		indices.push_back(index);
		addIndices(next...);
	}

private:
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	opengl::Mesh<Components...> mesh;
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

protected:
	vec2 mousePos, dmouse;
	vec2 frameBufferSize, contentScale;
	float time, dt;
	float depth;
	bool focus;

	std::array<bool, GLFW_KEY_LAST + 1> keys;
	std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> buttons;
};

class GuiSystem : public GuiIO {
public:
	using Vertex = opengl::Vertex<vec3, vec4, vec2>;

	void beginFrame(float time, float dt);
	void endFrame();
	void render(mat4 transform = mat4());

	void pushStyle(const GuiStyle &style);
	GuiStyle& currentStyle();
	GuiStyle& defaultStyle();
	void popStyle();

	void empty(vec2 size);
	void text(const std::string &text);
	bool button(const std::string &text);

protected:
	vec2 widget(vec2 pos, vec2 size);
	vec2 text(const std::string &text, vec2 pos);

	void rect(vec2 pos, vec2 size);
	void border(vec2 pos, vec2 size);

private:
	TextRenderer textRenderer;
	DrawList<vec3, vec4, vec2> drawList;
	std::vector<GuiStyle> styleStack;
	GuiStyle m_defaultStyle;

	opengl::UniformBuffer<mat4> transformUBO;
	opengl::Program prog;
};