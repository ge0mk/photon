#include <world.hpp>

#include <algorithm>
#include <fstream>

#include <rigidbody.hpp>

World::World(const std::string &path, const Camera *cam) : cam(cam), textRenderer(freetype::Font("res/jetbrains-mono.ttf")) {
	loadShader(path);
	init();

	textRenderer.createObject("Hello World!", mat4().scale(0.05), vec4(1));
}

World::~World() {}

void World::loadShader(const std::string &path) {
	std::ifstream src(path, std::ios::ate);
	std::string buffer(src.tellg(), '\0');
	src.seekg(src.beg);
	src.read(buffer.data(), buffer.size());

	shader = opengl::Program::load(buffer, opengl::Shader::VertexStage | opengl::Shader::FragmentStage);
	shader.use();
	shader.setUniform("sampler", 0);
}

void World::init() {
	shader.use();
	// init ubos
	cameraInfoUBO.bindBase(0);
	cameraInfoUBO.setData({mat4(), mat4()});

	modelInfoUBO.bindBase(1);
	modelInfoUBO.setData({mat4(), mat4()});

	renderInfoUBO.bindBase(2);
	renderInfoUBO.setData({vec4(0), cam->res, 0.0f, 0.0f});

	// init meshes
	unitplane = opengl::Mesh<vec3, vec2>({
		{ math::vec3( 0.5f, 0.5f, 0.0f), math::vec2(1,0) },
		{ math::vec3(-0.5f, 0.5f, 0.0f), math::vec2(0,0) },
		{ math::vec3(-0.5f,-0.5f, 0.0f), math::vec2(0,1) },
		{ math::vec3( 0.5f,-0.5f, 0.0f), math::vec2(1,1) },
	}, {
		0, 1, 2, 2, 3, 0,
	});
}

void World::setTexturePtr(const std::shared_ptr<TiledTexture> &texture) {
	this->texture = texture;
}

void World::setParticleTexturePtr(const std::shared_ptr<TiledTexture> &texture) {
	particleSystem.setTexture(texture);
}

Chunk* World::createChunk(ivec2 pos) {
	chunks.push_back(std::unique_ptr<Chunk>(new Chunk(this, pos, texture->scale())));
	return chunks.back().get();
}

Chunk* World::getChunk(ivec2 pos) {
	for(const auto &chunk : chunks) {
		if(chunk->getPos() == pos) {
			return chunk.get();
		}
	}
	if(autogrow) {
		return createChunk(pos);
	}
	throw std::runtime_error("no chunk at (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")!");
}

const Chunk* World::getChunk(ivec2 pos) const {
	for(const auto &chunk : chunks) {
		if(chunk->getPos() == pos) {
			return chunk.get();
		}
	}
	throw std::runtime_error("no chunk at (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")!");
}

void World::setAutoGrow(bool state) {
	autogrow = state;
}

bool World::getAutoGrow() {
	return autogrow;
}

void World::update(float time, float dt) {
	for(auto &chunk : chunks) {
		chunk->update(time, dt);
	}

	for(auto &entity : entities) {
		entity->update(time, dt, this);
	}

	if(particleSystem.size() < 10000) {
		for(unsigned i = 0; i < 10; i++) {
			vec2 pos = vec2(float(rand()) / float(RAND_MAX) * 100 - 50, 40) * 8;
			vec2 scale = vec2(0.02, 0.2) * ((float(rand()) / float(RAND_MAX) + 0.9) / 2 + 0.4) * 8;
			vec2 speed = vec2(0.05, -float(rand()) / float(RAND_MAX) * 2 - 4) * 8;
			vec2 gravity = vec2(0, 0);
			particleSystem.spawn(Particle(Particle::rain, pos, speed, gravity, scale, 0, 0));
		}
	}
	for(Particle &p : particleSystem) {
		if(p.type == Particle::rain && p.speed.y == 0.0) {
			p.pos = vec2(float(rand()) / float(RAND_MAX) * 100 - 50, 40) * 8;
			p.speed = vec2(0.05, -float(rand()) / float(RAND_MAX) * 2 - 4) * 8;
		}
	}
	particleSystem.erase([](const Particle &p) -> bool {
		if(p.type == Particle::blood && p.lifetime > 10) {
			return true;
		}
		return false;
	});

	particleSystem.update(time, dt, this);
	textRenderer.update();
}

void World::render() {
	shader.use();
	texture->activate();

	cameraInfoUBO.bindBase(0);
	modelInfoUBO.bindBase(1);
	renderInfoUBO.bindBase(2);

	cameraInfoUBO.update({cam->proj, cam->view});
	renderInfoUBO.update({vec4(0), cam->res, 0.0f, 0.0f});

	for(auto &chunk : chunks) {
		mat4 transform = mat4().translate(vec3(chunk->getPos() * Chunk::size * Tile::resolution));
		if(dist(cam->pos.xy, vec2(chunk->getPos()) * Chunk::size * Tile::resolution) < Chunk::size * Tile::resolution * 2) {
			modelInfoUBO.update({transform, mat4()});
			chunk->render();
		}
	}

	for(auto &entity : entities) {
		mat4 transform = entity->getTransform();
		vec2 pos = transform * vec4(0.0f, 0.0f, 0.0f, 1.0f);
		if(dist(cam->pos.xy, pos) < Chunk::size * Tile::resolution * 2) {
			modelInfoUBO.update({transform, entity->getUVTransform()});
			entity->getTexturePtr()->activate();
			unitplane.drawElements();
		}
	}

	particleSystem.render(cam->proj * cam->view);
	textRenderer.render(cam->proj * cam->view);
}

void World::renderCollisions(std::vector<ivec2> tiles, std::shared_ptr<TiledTexture> texture) {
	shader.use();
	if(texture) {
		texture->activate();
	}

	cameraInfoUBO.bindBase(0);
	modelInfoUBO.bindBase(1);
	renderInfoUBO.bindBase(2);

	cameraInfoUBO.update({cam->proj, cam->view});
	renderInfoUBO.setData({vec4(0.6f, 0.0f, 0.0f, 1.0f), cam->res, 0.0f, 0.0f});

	for(ivec2 tile : tiles) {
		mat4 transform = mat4().translate(vec3(vec2(tile) + 0.5f, 0.0f)).scale(0.666f);
		modelInfoUBO.update({transform, mat4()});
		unitplane.drawElements();
	}
}

Tile& World::operator[](const ivec2 &pos) {
	ivec2 chunkpos = pos / ivec2(Chunk::size) - ivec2(pos.x < 0, pos.y < 0);
	ivec2 tilepos = (pos - chunkpos * Chunk::size) % Chunk::size;
	Chunk *chunk = getChunk(chunkpos);
	return chunk->at(tilepos);
}

const Tile& World::operator[](const ivec2 &pos) const {
	ivec2 chunkpos = pos / ivec2(Chunk::size) - ivec2(pos.x < 0, pos.y < 0);
	ivec2 tilepos = (pos - chunkpos * Chunk::size) % Chunk::size;
	const Chunk *chunk = getChunk(chunkpos);
	return chunk->at(tilepos);
}

Tile& World::at(const ivec2 &pos) {
	return this->operator[](pos);
}

Tile World::at(const ivec2 &pos) const {
	Tile tile;
	try {
		tile = this->operator[](pos);
	} catch(std::exception &e) {}
	return tile;
}

Tile World::getTileOrEmpty(const ivec2 &pos) const {
	Tile tile;
	try {
		tile = this->operator[](pos);
	} catch(std::exception &e) {}
	return tile;
}

vec2 World::snapToGrid(vec2 worldpos) {
	return ivec2(worldpos) - ivec2(worldpos.x < 0 ? 1 : 0, worldpos.y < 0 ? 1 : 0);
}

ivec2 World::getTileIndex(vec2 worldpos) {
	return snapToGrid(worldpos / Tile::resolution);
}

void World::createBloodParticles(vec2 pos) {
	for(int i = 0; i < 10; i++) {
		vec2 speed = vec2(float(rand()) / float(RAND_MAX) * 16 - 8, float(rand()) / float(RAND_MAX) * 16 - 8);
		float rspeed = float(rand()) / float(RAND_MAX) * 8 - 4;
		particleSystem.spawn(Particle(Particle::blood, pos + 0.5, speed, vec2(0, -32), vec2(0.5), 0, rspeed));
	}
}