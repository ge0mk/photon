#include <text.hpp>

TextObject::TextObject(const std::string &text, mat4 transform, vec4 color)
: text(text), transform(transform), color(color) {}

TextRenderer::TextRenderer(freetype::Font &&font) : font(std::move(font)) {
	std::ifstream src("assets/text.glsl", std::ios::ate);
	std::string buffer(src.tellg(), '\0');
	src.seekg(src.beg);
	src.read(buffer.data(), buffer.size());
	prog = opengl::Program::load(buffer, opengl::Shader::VertexStage | opengl::Shader::FragmentStage);
	prog.use();
	prog.setUniform("sampler", 0);

	transformUBO.bindBase(0);
	transformUBO.setData(mat4());

	this->font.setPixelSizes(64);
	this->font.buildFontAtlas();
	texture.load(this->font.getFontAtlas());
}

std::shared_ptr<TextObject> TextRenderer::createObject(const std::string &text, mat4 transform, vec4 color) {
	std::lock_guard<std::mutex> lock(objectMutex);
	objects.push_back(std::shared_ptr<TextObject>(new TextObject(text, transform, color)));
	changed = true;
	return objects.back();
}

void TextRenderer::removeObject(const std::shared_ptr<TextObject> &object) {
	std::lock_guard<std::mutex> lock(objectMutex);
	std::erase_if(objects, [&](const std::shared_ptr<TextObject> &o) -> bool {
		return o == object;
	});
	changed = true;
}

void TextRenderer::update() {
	std::lock_guard<std::mutex> lock(objectMutex);
	std::lock_guard<std::mutex> renderlock(renderMutex);
	vertices.clear();
	indices.clear();

	for(const auto &object : objects) {
		std::string text = object->text;

		auto quad = [&](vec2 pos, vec2 size, vec2 uv) {
			unsigned baseIndex = vertices.size();

			vertices.push_back(Vertex{
				object->transform * vec4(pos + vec2(0, -size.y), 0, 1),
				(uv + vec2(0, size.y)) / texture.size(),
				object->color
			});
			vertices.push_back(Vertex{
				object->transform * vec4(pos + vec2(0), 0, 1),
				(uv + vec2(0)) / texture.size(),
				object->color
			});
			vertices.push_back(Vertex{
				object->transform * vec4(pos + vec2(size.x, 0), 0, 1),
				(uv + vec2(size.x, 0)) / texture.size(),
				object->color
			});
			vertices.push_back(Vertex{
				object->transform * vec4(pos + vec2(size.x, -size.y), 0, 1),
				(uv + vec2(size)) / texture.size(),
				object->color
			});

			indices.push_back(baseIndex + 0);
			indices.push_back(baseIndex + 1);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex + 3);
			indices.push_back(baseIndex + 0);
		};

		int x = 0, y = 0;
		for(unsigned i = 0; i < text.length(); i++) {
			auto [pos, size, uv, advanceX] = font[text[i]];
			quad(vec2(x, y) + pos, size, uv);
			x += advanceX;
		}
	}
	changed = true;
}

void TextRenderer::render(mat4 transform) {
	if(changed) {
		std::lock_guard<std::mutex> lock(renderMutex);
		mesh.setData({vertices, indices});
		changed = false;
	}

	prog.use();
	transformUBO.bindBase(0);
	transformUBO.update(transform);
	texture.bind();
	texture.activate();
	mesh.drawElements();
}

void TextRenderer::applyTransform(mat4 transform) {
	for(auto &object : objects) {
		object->transform = transform * object->transform;
	}
}
