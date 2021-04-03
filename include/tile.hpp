#pragma once

#include <math/matrix.hpp>
#include <math/vector.hpp>

using namespace math;

class Chunk;

class Tile {
public:
	static constexpr uint8_t resolution = 8;
	enum etype : uint32_t {
		null = 0,
		grass,
		dirt,
		stone,
		rock,
	};

	Tile(uint32_t type = null, uint32_t variant = 0, uint64_t custom = 0);
	Tile(const Tile &other) = default;

	Tile& operator=(const Tile &other) = default;

	void init(uint32_t type);
	void update(float time, float dt, ivec2 pos, const Chunk &chunk);
	bvec4 hitbox() const;
	vec2 texture(svec2 pos) const;	// tile position on the texture
	bool render() const;
	bool collision() const;
	void destroy();		// remove tile during game -> may trigger effects
	void clear();		// remove tile from editor -> no effects

	uint32_t type, variant;
	uint64_t custom;
};