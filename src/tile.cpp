#include <tile.hpp>

Tile::Tile(uint32_t type) : type(type) {}

void Tile::init(uint32_t type) {
	this->type = type;
}

void Tile::update(float time, float dt, ivec2 pos, Chunk *chunk) {
	switch(type) {}
}

bvec4 Tile::hitbox() const {
	switch(type) {
		default: return bvec4(0, 0, resolution, resolution);
	}
}

vec2 Tile::texture() const {
	switch(type) {
		case null: return vec2(0,0);
		case stone: return vec2(0,7);
		case grass: return vec2(0,1);
		default: return vec2();
	}
}

bool Tile::render() const {
	switch(type) {
		case null: return false;
		default: return true;
	}
}

bool Tile::collision() const {
	switch(type) {
		case null: return false;
		default: return true;
	}
}

void Tile::destroy() {
	type = null;
	variant = 0;
}

void Tile::clear() {
	switch(type) {
		default: destroy(); break;
	}
}
