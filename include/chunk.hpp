#pragma once

#include <array>

#include <math/matrix.hpp>
#include <math/vector.hpp>

#include <opengl/mesh.hpp>
#include <opengl/vertex.hpp>

#include "tile.hpp"

#include <atomic>
#include <memory>
#include <mutex>

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

	static constexpr uint8_t size = 64;

private:
	friend class World;
	World* world;
	vec2 pos, tileScale;
	std::array<Tile, size * size> tiles;
	bool m_sync = false;

private:
	std::unique_ptr<Mesh> mesh;
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	std::mutex meshMutex;
	std::atomic<bool> rebuild = false;
	std::atomic<bool> sync = false;
};