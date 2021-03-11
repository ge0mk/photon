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

void Particle::update(float time, float dt, World *world) {
	pos += speed * dt;
	speed += gravity * dt;
	rotation += rotspeed * dt * 3;
	lifetime += dt;
	if(world->getTileOrEmpty(ivec2(pos) - ivec2(pos.x < 0 ? 1 : 0, pos.y < 0 ? 1 : 0)).type != Tile::null) {
		speed = 0;
		vec2 tmp = pos + vec2(rotspeed*dt, 0.1);
		if(fract(pos.y) > 0.95 && world->getTileOrEmpty(ivec2(tmp) - ivec2(tmp.x < 0 ? 1 : 0, tmp.y < 0 ? 1 : 0)).type == Tile::null) {
			pos.x += rotspeed * dt;
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

void ParticleSystem::spawn(const Particle &particle) {
	particles.push_back(particle);
}

void ParticleSystem::update(float time, float dt, World *world) {
	for(Particle &p : particles) {
		p.update(time, dt, world);
	}
	buffer.setData(particles, opengl::Buffer<Particle>::DynamicDraw);
}

void ParticleSystem::render(mat4 transform) {
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