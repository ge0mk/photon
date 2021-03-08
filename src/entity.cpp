#include <entity.hpp>

Entity::Entity(std::shared_ptr<SpriteSheet> sprites) : spritesheet(sprites) {}

Entity::~Entity() {}

void Entity::update(float time, float dt, World *world) {}

mat4 Entity::getTransform() {
	return transform;
}

mat4 Entity::getUVTransform() {
	return uvtransform;
}

SpriteSheet* Entity::getSpriteSheet() {
	return spritesheet.get();
}