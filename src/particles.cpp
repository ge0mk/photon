#include <particles.hpp>

#include <world.hpp>

Particle::Particle(uint32_t type, vec2 pos, vec2 speed, vec2 gravity, vec2 scale, float rotation, float rotspeed)
: type(type), pos(pos), speed(speed), gravity(gravity), scale(scale), rotation(rotation), rotspeed(rotspeed) {
	switch(type) {
		case null: break;
		case rain: {
			uvtl = vec2(20.5/24.0, 0.5);
			uvbr = vec2(20.5/24.0, 0.5);
		} break;
		case blood: {
			uvtl = vec2(10.5/24.0, 0.5);
			uvbr = vec2(10.5/24.0, 0.5);
		} break;
	}
}

void Particle::update(float time, float dt, const WorldContainer &world) {
	pos += speed * dt;
	speed += gravity * dt;
	rotation += rotspeed * dt * 5;
	lifetime += dt;
	if(world.at(world.getTileIndex(pos)).type != Tile::null) {
		if(abs(speed.x) > abs(speed.y) && fract(pos.y) > 0.95)
			speed.x = 0;
		else {
			speed.y = 0;
		}
		vec2 tmp = pos + vec2(rotspeed * dt * 8, 0.1);
		vec2 tmp2 = pos + vec2(-rotspeed * dt, -0.1);
		if(fract(pos.y) > 0.95 && world.at(world.getTileIndex(tmp)).type == Tile::null) {
			pos.x += rotspeed * dt * 8;
			speed.x = rotspeed * 8;
		}
		else if(world.at(world.getTileIndex(tmp2)).type == Tile::null) {
			pos.y -= 0.2 * dt;
		}
		else {
			rotspeed = -0.9 * rotspeed;
		}
	}
}

ParticleSystem::ParticleSystem(std::shared_ptr<TiledTexture> texture) : texture(texture), buffer(opengl::Buffer<Particle>::Array) {
	std::ifstream src("res/particles.glsl", std::ios::ate);
	std::string buffer(src.tellg(), '\0');
	src.seekg(src.beg);
	src.read(buffer.data(), buffer.size());
	prog = opengl::Program::load(buffer, opengl::Shader::VertexStage | opengl::Shader::GeometryStage | opengl::Shader::FragmentStage);

	this->buffer.initEmpty(opengl::Buffer<Particle>::DynamicDraw, 0);
	transformUBO.bindBase(0);
	transformUBO.setData(mat4());

	vao.bind();
	this->buffer.bind();
	vao.setVertexAttributes<vec4, vec4, vec4, vec4>();
	vao.unbind();
}

void ParticleSystem::update(float time, float dt, const WorldContainer &world) {
	for(Particle &p : particles) {
		p.update(time, dt, world);
	}
	changed = true;
}

void ParticleSystem::render(mat4 transform) {
	if(changed) {
		buffer.setData(particles, opengl::Buffer<Particle>::DynamicDraw);
		changed = false;
	}

	prog.use();
	transformUBO.bindBase(0);
	transformUBO.update(transform);
	if(texture) {
		texture->activate();
	}
	vao.bind();
	glDrawArrays(GL_POINTS, 0, buffer.size());
	vao.unbind();
}

void ParticleSystem::shift(ivec2 dir) {
	for(Particle &p : particles) {
		p.pos += vec2(dir) * Chunk::size * Tile::resolution;
	}
}

void ParticleSystem::setTexture(const std::shared_ptr<TiledTexture> &texture) {
	this->texture = texture;
}

size_t ParticleSystem::size() {
	return particles.size();
}

Particle& ParticleSystem::operator[](size_t index) {
	return particles[index];
}

const Particle& ParticleSystem::operator[](size_t index) const {
	return particles[index];
}

std::vector<Particle>::iterator ParticleSystem::begin() {
	return particles.begin();
}

std::vector<Particle>::iterator ParticleSystem::end() {
	return particles.end();
}