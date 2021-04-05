#include <tile.hpp>

Tile::Tile(uint32_t type, uint32_t variant, uint64_t custom) : type(type), variant(variant), custom(custom) {}

void Tile::init(uint32_t type) {
	this->type = type;
}

void Tile::update([[maybe_unused]] float time, [[maybe_unused]] float dt, [[maybe_unused]] ivec2 pos, [[maybe_unused]] const Chunk &chunk) {
	switch(type) {}
}

bvec4 Tile::hitbox() const {
	switch(type) {
		default: return bvec4(0, 0, resolution, resolution);
	}
}

vec2 Tile::texture(svec2 pos) const {
	switch(type) {
		case null: return vec2(0, 0);
		case grass: return vec2(0 + (pos.x % 2), 2 + variant);
		case dirt: return vec2(0 + (pos.x % 2), 4 + variant);
		case stone: return vec2(0 + (pos.x % 2), 14 + variant);
		case rock: return vec2(0 + (pos.x % 2), 22 + ((pos.y + 1) % 2));
		default: return vec2();
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

bool Tile::transparent() const {
	switch(type) {
		case null: return true;
		default: return false;
	}
}

bool Tile::visible() const {
	switch(type) {
		case null: return false;
		default: return true;
	}
}

bool Tile::solid() const {
	switch(type) {
		case null: return false;
		default: return true;
	}
}
