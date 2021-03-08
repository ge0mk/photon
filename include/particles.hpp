#pragma once

#include <memory>
#include <string>
#include <vector>
#include <fstream>

#include <math/matrix.hpp>
#include <math/vector.hpp>

#include <opengl/buffer.hpp>
#include <opengl/program.hpp>
#include <opengl/texture.hpp>
#include <opengl/vao.hpp>

#include "resources.hpp"

using namespace math;

class World;

struct Particle {
	vec2 pos = vec2(0), speed = vec2(0);
	vec2 uvtl = vec2(0), uvbr = vec2(1);
	vec2 gravity = vec2(0, -10);
	float rotation = 0, scale = 0.1;

	void update(float time, float dt, World *world);
};

class ParticleSystem {
public:
	ParticleSystem(std::shared_ptr<TiledTexture> texture = {});

	void setTexturePtr(std::shared_ptr<TiledTexture> texture);
	void createParticle(const Particle &particle);

	void update(float time, float dt, World *world);
	void render();

	size_t size();

	Particle& operator[](size_t index);
	const Particle& operator[](size_t index) const;

	std::vector<Particle>::iterator begin();
	std::vector<Particle>::iterator end();

private:
	std::vector<Particle> particles;
	opengl::Buffer<Particle> buffer;
	opengl::VertexArray vao;
	opengl::Program prog;
	std::shared_ptr<TiledTexture> texture;
};