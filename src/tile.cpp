#include <tile.hpp>

Tile::Tile(uint32_t type) : type(type) {}

void Tile::init(uint32_t type) {
	this->type = type;
}

void Tile::update(float time, float dt, ivec2 pos, Chunk *chunk) {
	variant = (pos.x % 2) + ((pos.y - 1) % 2) * 2;
	switch(type) {}
}

bvec4 Tile::hitbox() const {
	switch(type) {
		default: return bvec4(0, 0, resolution, resolution);
	}
}

vec2 Tile::texture() const {
	switch(type) {
		case null: return vec2(0, 0);
		case grass: {
			return vec2(0 + (variant & 1), 2 + ((variant & 2)>>1));
			if(variant == 0) return vec2(0, 2);
			if(variant == 1) return vec2(1, 2);
			if(variant == 2) return vec2(0, 3);
			if(variant == 3) return vec2(1, 3);
		}
		case dirt: return vec2(0 + (variant & 1), 4 + ((variant & 2)>>1));
		case stone: return vec2(0 + (variant & 1), 14 + ((variant & 2)>>1));
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
