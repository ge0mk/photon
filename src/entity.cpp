#include <entity.hpp>

Entity::Entity(std::shared_ptr<TiledTexture> texture) : texture(texture) {}

Entity::~Entity() {}

void Entity::update(float time, float dt, World *world) {}

mat4 Entity::getTransform() {
	return transform;
}

mat4 Entity::getUVTransform() {
	return uvtransform;
}

TiledTexture* Entity::getTexturePtr() {
	return texture.get();
}