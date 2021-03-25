#include <entity.hpp>

Entity::Entity(std::shared_ptr<TiledTexture> texture) : texture(texture) {}

Entity::~Entity() {}

void Entity::update(float time, float dt, World *world) {
	transform = mat4().translate(pos).rotate(rot).scale(scale);
}

void Entity::render() {}

bool Entity::customRenderFunction() const {
	return false;
}

void Entity::shift(ivec2 dir) {
	pos.xy += vec2(dir) * Chunk::size * Tile::resolution;
}

mat4 Entity::getTransform() {
	return transform;
}

mat4 Entity::getUVTransform() {
	return uvtransform;
}

TiledTexture* Entity::getTexturePtr() {
	return texture.get();
}

void Entity::setTexturePtr(std::shared_ptr<TiledTexture> texture) {
	this->texture = texture;
}