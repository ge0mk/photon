#pragma once

#include <memory>
#include <vector>

#include <math/vector.hpp>
#include <math/matrix.hpp>
#include <opengl/texture.hpp>
#include <utils/image.hpp>

using namespace math;

class TiledTexture {
public:
	TiledTexture(const std::string &path, ivec2 size);

	vec2 scale() const;
	ivec2 size() const;
	ivec2 textureSize() const;

	void activate();

	mat4 getUVTransform(ivec2 pos) const;

private:
	opengl::Texture texture;
	ivec2 m_size;
};

template<typename T>
class ResourceCache {
public:
	ResourceCache() {}
	~ResourceCache() {}

	template<typename ...Args>
	std::shared_ptr<T> load(Args ...args) {
		items.push_back(std::shared_ptr<T>(new T(args...)));
		return items.back();
	}

	size_t size() { return items.size(); }
	std::shared_ptr<T> get(unsigned index) {
		return items[index];
	}

private:
	std::vector<std::shared_ptr<T>> items;
};