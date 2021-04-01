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

class WorldContainer;
class Chunk {
public:
	using Vertex = opengl::Vertex<vec3, vec2>;
	using Mesh = opengl::IndexedMesh<vec3, vec2>;

	Chunk(const WorldContainer &container, lvec2 pos, vec2 tileScale);

	void fill(uint64_t tile);

	void build();
	void update(float time, float dt);
	void render();

	lvec2 getPos();

	Tile& operator[](ivec2 pos);
	const Tile& operator[](ivec2 pos) const;

	Tile& at(ivec2 pos);
	const Tile& at(ivec2 pos) const;

	static constexpr uint8_t size = 64;

private:
	const WorldContainer &container;

	lvec2 pos;
	vec2 tileScale;
	std::array<Tile, size * size> tiles;

	std::unique_ptr<Mesh> mesh;
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	std::mutex meshMutex;
	std::atomic<bool> rebuild = false;
	std::atomic<bool> sync = false;
};