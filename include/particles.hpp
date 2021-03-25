#pragma once

#include <memory>
#include <atomic>
#include <string>
#include <vector>
#include <fstream>

#include <math/matrix.hpp>
#include <math/vector.hpp>

#include <opengl/buffer.hpp>
#include <opengl/program.hpp>
#include <opengl/texture.hpp>
#include <opengl/uniform.hpp>
#include <opengl/vao.hpp>

#include "chunk.hpp"
#include "resources.hpp"
#include "tile.hpp"

using namespace math;

class World;

struct Particle {
	enum {
		null = 0,
		rain = 2,
		blood = 4,
	};

	Particle(uint32_t type, vec2 pos, vec2 speed, vec2 gravity, vec2 scale, float rotation, float rotspeed);

	vec2 pos = vec2(0), speed = vec2(0);
	vec2 uvtl = vec2(0), uvbr = vec2(1);
	vec2 gravity = vec2(0, -10), scale = 0.1;
	float rotation = 0, rotspeed = 0.0f;
	float lifetime = 0;
	uint32_t type;

	void update(float time, float dt, World *world);
};

class ParticleSystem {
public:
	ParticleSystem(std::shared_ptr<TiledTexture> texture = {});

	void update(float time, float dt, World *world);
	void render(mat4 transform);

	void shift(ivec2 dir);

	void setTexture(const std::shared_ptr<TiledTexture> &texture);

	void spawn(const Particle &particle);

	size_t size();

	Particle& operator[](size_t index);
	const Particle& operator[](size_t index) const;

	std::vector<Particle>::iterator begin();
	std::vector<Particle>::iterator end();

	template<typename func_t>
	void erase(func_t func) {
		std::erase_if(particles, func);
	}

private:
	std::vector<Particle> particles;
	std::atomic<bool> changed;

	std::shared_ptr<TiledTexture> texture;
	opengl::Buffer<Particle> buffer;
	opengl::UniformBuffer<mat4> transformUBO;
	opengl::VertexArray vao;
	opengl::Program prog;
};