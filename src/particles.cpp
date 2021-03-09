#include <particles.hpp>

void Particle::update(float time, float dt, World *world) {
	pos += speed * dt;
	speed += gravity * dt;
}

ParticleSystem::ParticleSystem(std::shared_ptr<TiledTexture> texture) : Entity(texture), buffer(opengl::Buffer<Particle>::Array) {
	std::ifstream src("res/particles.glsl", std::ios::ate);
	std::string buffer(src.tellg(), '\0');
	src.seekg(src.beg);
	src.read(buffer.data(), buffer.size());
	prog = opengl::Program::load(buffer, opengl::Shader::VertexStage | opengl::Shader::GeometryStage | opengl::Shader::FragmentStage);

	this->buffer.initEmpty(opengl::Buffer<Particle>::DynamicDraw, 0);

	vao.bind();
	this->buffer.bind();
	vao.setVertexAttributes<vec4, vec4, vec4, vec2>();
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

	Entity::update(time, dt, world);
}

void ParticleSystem::render() {
	prog.use();
	texture->activate();
	vao.bind();
	glDrawArrays(GL_POINTS, 0, buffer.size());
	vao.unbind();
}

bool ParticleSystem::customRenderFunction() const {
	return true;
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