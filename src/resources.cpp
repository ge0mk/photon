#include <resources.hpp>

TiledTexture::TiledTexture(const std::string &path, ivec2 size) : m_size(size) {
	Image img(path);
	texture.load(img);
	texture.bind();
	texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	texture.unbind();
}

TiledTexture::TiledTexture(const Image &img) : m_size(img.size()) {
	texture.load(img);
	texture.bind();
	texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	texture.unbind();
}

ivec2 TiledTexture::size() const {
	return m_size;
}

vec2 TiledTexture::scale() const {
	return vec2(1.0f) / vec2(m_size);
}

ivec2 TiledTexture::textureSize() const {
	return texture.size();
}

void TiledTexture::activate() {
	texture.activate();
	texture.bind();
}

mat4 TiledTexture::getUVTransform(ivec2 pos) const {
	return mat4().scale(scale()).translate(vec3(pos.x, pos.y));
}