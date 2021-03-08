#pragma once

#include <array>

#include <math/matrix.hpp>
#include <math/vector.hpp>

#include <opengl/mesh.hpp>
#include <opengl/vertex.hpp>

#include "tile.hpp"

using namespace math;

class World;
class Chunk {
public:
	using Vertex = opengl::Vertex<vec3, vec2>;
	using Mesh = opengl::Mesh<vec3, vec2>;

	enum eblock : uint8_t {
		center = 0,
		left,
		top_left,
		top,
		top_right,
		right,
		bottom_right,
		bottom,
		bottom_left,
	};

	Chunk(World *world, vec2 pos, vec2 tileScale);

	void fill(uint64_t tile);

	void build();
	void update(float time, float dt);
	void render();

	bool sync();

	ivec2 getPos();

	Tile& operator[](ivec2 pos);
	const Tile& operator[](ivec2 pos) const;

	Tile& at(ivec2 pos);
	const Tile& at(ivec2 pos) const;

private:
	friend class World;
	World* world;
	vec2 pos, tileScale;
	std::array<Tile, 32*32> tiles;
	bool m_sync = false;

private:
	Mesh mesh;
	bool rebuild = false;
};