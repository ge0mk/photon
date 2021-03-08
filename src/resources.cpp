#include <resources.hpp>

TileSet::TileSet(const std::string &path, ivec2 size) : m_size(size) {
	Image img(path);
	texture.load(img);
	texture.bind();
	texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	texture.unbind();
}

ivec2 TileSet::size() {
	return m_size;
}

vec2 TileSet::scale() {
	return vec2(1.0f) / vec2(m_size);
}

ivec2 TileSet::textureSize() {
	return texture.size();
}

void TileSet::activate() {
	texture.activate();
	texture.bind();
}


SpriteSheet::SpriteSheet(const std::string &path, ivec2 size) : m_size(size) {
	Image img(path);
	texture.load(img);
	texture.bind();
	texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	texture.unbind();
}

ivec2 SpriteSheet::size() {
	return m_size;
}

ivec2 SpriteSheet::textureSize() {
	return texture.size();
}

vec2 SpriteSheet::scale() {
	return vec2(1.0f) / vec2(m_size);
}

void SpriteSheet::activate() {
	texture.activate();
	texture.bind();
}

mat4 SpriteSheet::getUVTransform(ivec2 pos) {
	return mat4().scale(scale()).translate(vec3(pos.x, pos.y));
}