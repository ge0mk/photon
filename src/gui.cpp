#include <gui.hpp>

void GuiStyleStack::pushStyle(const GuiStyle &style) {
	styles.push_back(style);
}

GuiStyle& GuiStyleStack::currentStyle() {
	if(styles.size() > 0) {
		return styles.back();
	}
	return m_defaultStyle;
}

GuiStyle& GuiStyleStack::defaultStyle() {
	return m_defaultStyle;
}

void GuiStyleStack::popStyle() {
	if(styles.size() > 0) {
		styles.pop_back();
	}
}

void GuiIO::onKeyTyped(int key) {}

void GuiIO::onKeyChanged(int key, int scancode, int modifier, int action) {}

void GuiIO::onKeyPressed(int key, int scancode, int modifier, bool repeat) {
	keys[key] = 1;
}

void GuiIO::onKeyReleased(int key, int scancode, int modifier) {
	keys[key] = 2;
}

void GuiIO::onMouseMoved(vec2 pos, vec2 dir) {
	mousePos = pos;
}

void GuiIO::onMousePressed(int button, int modifier) {
	buttons[button] = 1;
}

void GuiIO::onMouseReleased(int button, int modifier) {
	buttons[button] = 2;
}

void GuiIO::onScrollEvent(vec2 dir) {}

void GuiIO::onWindowContentScaleChanged(vec2 scale) {}

void GuiIO::onWindowFocusChanged(bool focussed) {}

void GuiIO::onFramebufferResized(ivec2 size) {
	frameBufferSize = size;
}

bool GuiIO::keyPressed(int key) {
	return keys[key] == 1;
}

bool GuiIO::keyReleased(int key) {
	return keys[key] == 2 || keys[key] == 0;
}

bool GuiIO::keyJustReleased(int key) {
	return keys[key] == 2;
}

bool GuiIO::buttonPressed(int button) {
	return buttons[button] == 1;
}

bool GuiIO::buttonReleased(int button) {
	return buttons[button] == 2 || buttons[button] == 0;
}

bool GuiIO::buttonJustReleased(int button) {
	return buttons[button] == 2;
}

void GuiIO::beginFrame() {
	mouse = false;
	keyboard = false;
}

void GuiIO::endFrame() {
	for(unsigned i = 0; i < keys.size(); i++) {
		keys[i] %= 2;	// when keys[i] == 2 -> keys[i] becomes 0, but when keys[i] == 1 nothing changes
	}
	for(unsigned i = 0; i < buttons.size(); i++) {
		buttons[i] %= 2;	// same as above
	}
}

bool GuiIO::usesMouse() {
	return mouse;
}

bool GuiIO::usesKeyboard() {
	return keyboard;
}

GuiSystem::GuiSystem(freetype::Font &&font) : textRenderer(std::move(font)) {
	std::ifstream src("assets/gui.glsl", std::ios::ate);
	std::string buffer(src.tellg(), '\0');
	src.seekg(src.beg);
	src.read(buffer.data(), buffer.size());
	prog = opengl::Program::load(buffer, opengl::Shader::VertexStage | opengl::Shader::FragmentStage);
	prog.use();
	prog.setUniform("sampler", 0);

	transformUBO.bindBase(0);
	transformUBO.setData(mat4());
}

void GuiSystem::beginFrame(float time, float dt) {
	GuiIO::beginFrame();
	mesh.clear();
	textRenderer.clear();
	depth = 0.0f;
}

void GuiSystem::endFrame() {
	GuiIO::endFrame();
	mesh.update();
	textRenderer.update();
	//std::cout<<mesh.vertexCount<<" vertices, "<<mesh.indexCount<<" indices\n";
	//std::cout<<mesh.triangles<<" triangles, "<<mesh.quads<<" quads\n";
}

void GuiSystem::render(mat4 transform) {
	glClear(GL_DEPTH_BUFFER_BIT);

	transform = transform.translate(vec3(-1, 1, 0)).scale(vec3(vec2(1.0f) / frameBufferSize * 2.0f, 1.0f));
	prog.use();
	transformUBO.bindBase(0);
	transformUBO.update(transform);
	mesh.render();

	textRenderer.render(transform);
}

void GuiSystem::rect(vec2 tl, vec2 br, vec4 color, float z) {
	tl.y = -tl.y;
	br.y = -br.y;
	mesh.addQuad(
		Vertex(vec3(tl, z), color),
		Vertex(vec3(br.x, tl.y, z), color),
		Vertex(vec3(br, z), color),
		Vertex(vec3(tl.x, br.y, z), color)
	);
}

void GuiSystem::circle(vec2 center, float radius, float innerRadius, vec4 color, int segments, float z) {
	circleSegment(center, radius, innerRadius, 0, pi * 2.0f, color, segments, z);
}

void GuiSystem::circleSegment(vec2 center, float radius, float innerRadius, float astart, float aend, vec4 color, int segments, float z) {
	if(segments == 0) {
		segments = abs(aend - astart) / pi * 16;
	}
	segments = abs(segments);
	float adist = abs(aend - astart) / segments;
	center.y = -center.y;

	if(innerRadius == 0.0f) {
		for(int i = 0; i < segments; i++) {
			float start = astart + float(i) * adist;
			float end = astart + float(i + 1) * adist;

			vec2 vtl = center + vec2(sin(start), cos(start)) * radius;
			vec2 vtr = center + vec2(sin(end), cos(end)) * radius;

			mesh.addTriangle(
				Vertex(vec3(vtl, z), color),
				Vertex(vec3(vtr, z), color),
				Vertex(vec3(center, z), color)
			);
		}
	}
	else {
		for(int i = 0; i < segments; i++) {
			float start = astart + float(i) * adist;
			float end = astart + float(i + 1) * adist;

			vec2 vtl = center + vec2(sin(start), cos(start)) * radius;
			vec2 vtr = center + vec2(sin(end), cos(end)) * radius;
			vec2 vbl = center + vec2(sin(start), cos(start)) * innerRadius;
			vec2 vbr = center + vec2(sin(end), cos(end)) * innerRadius;

			mesh.addQuad(
				Vertex(vec3(vtl, z), color),
				Vertex(vec3(vtr, z), color),
				Vertex(vec3(vbr, z), color),
				Vertex(vec3(vbl, z), color)
			);
		}
	}
}

vec2 GuiSystem::text(const std::string &text, vec2 pos, vec4 color, vec2 scale, float z) {
	auto obj = textRenderer.createObject(text, mat4().translate(vec3(pos.x, -pos.y, z)).scale(vec3(scale, 1.0f)), color);
	return textRenderer.calcSize(obj);
}

bool GuiSystem::button(const std::string &text, vec2 pos, vec4 bgcolor, vec4 textcolor, float z) {
	vec2 size = textRenderer.calcSize(text);
	rect(pos, pos + size, bgcolor, z);
	textRenderer.createObject(text, mat4().translate(vec3(pos.x, -pos.y - size.y, z)), textcolor);
	vec2 mouse = mousePos - pos;
	if(mouse.x > 0 && mouse.x < size.x && mouse.y > 0 && mouse.y < size.y) {
		GuiIO::mouse = true;
		if(buttonJustReleased(GLFW_MOUSE_BUTTON_LEFT)) {
			return true;
		}
	}
	return false;
}
