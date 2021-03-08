#include <world.hpp>

#include <algorithm>
#include <fstream>

#include <rigidbody.hpp>

World::World(const std::string &path, const Camera *cam) : cam(cam) {
	loadShader(path);
	init();
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
	// init ubos
	cameraInfoUBO.bindBase(0);
	cameraInfoUBO.setData({mat4(), mat4()});

	modelInfoUBO.bindBase(1);
	modelInfoUBO.setData({mat4(), mat4()});

	renderInfoUBO.bindBase(2);
	renderInfoUBO.setData({vec4(0), cam->res, 0.0f, 0.0f});

	// init meshes
	unitplane = opengl::Mesh<vec3, vec2>({
		{ math::vec3( 1.0f, 1.0f, 0.0f), math::vec2(1,0) },
		{ math::vec3(-1.0f, 1.0f, 0.0f), math::vec2(0,0) },
		{ math::vec3(-1.0f,-1.0f, 0.0f), math::vec2(0,1) },
		{ math::vec3( 1.0f,-1.0f, 0.0f), math::vec2(1,1) },
	}, {
		0, 1, 2, 2, 3, 0,
	});
}

void World::setTileSetPtr(const std::shared_ptr<TileSet> &tileset) {
	this->tileset = tileset;
}

Chunk* World::createChunk(ivec2 pos) {
	chunks.push_back(std::unique_ptr<Chunk>(new Chunk(this, pos, tileset->scale())));
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
}

void World::render(mat4 proj) {
	shader.use();
	tileset->activate();

	cameraInfoUBO.update({cam->proj, cam->view});
	renderInfoUBO.setData({vec4(0), cam->res, 0.0f, 0.0f});

	for(auto &chunk : chunks) {
		mat4 transform = mat4().translate(vec3(chunk->getPos() * 32));
		if(dist(cam->pos.xy, vec2(chunk->getPos()) * 32) < 64) {
			modelInfoUBO.update({transform, mat4()});
			chunk->render();
		}
	}

	for(auto &entity : entities) {
		mat4 transform = entity->getTransform();
		vec2 pos = transform * vec4(0,0,0,1);
		if(dist(cam->pos.xy, pos) < 64) {
			modelInfoUBO.update({transform, mat4()});
			entity->getSpriteSheet()->activate();
			unitplane.drawElements();
		}
	}
}

void World::renderCollisions(RigidBody *entity) {
	renderInfoUBO.setData({vec4(1,0,0,1), cam->res, 0.0f, 0.0f});
	for(ivec2 tile : entity->getCollidingTiles()) {
		mat4 transform = mat4().translate(round(entity->getPos()) + vec2(tile)).scale(0.5).translate(vec3(1,1,0));
		modelInfoUBO.update({transform, mat4()});
		unitplane.drawElements();
	}
}

Tile& World::operator[](const ivec2 &pos) {
	ivec2 chunkpos = pos / ivec2(32) - ivec2(pos.x < 0, pos.y < 0);
	ivec2 tilepos = (pos - chunkpos * 32) % 32;
	Chunk *chunk = getChunk(chunkpos);
	return chunk->at(tilepos);
}

const Tile& World::operator[](const ivec2 &pos) const {
	ivec2 chunkpos = pos / ivec2(32) - ivec2(pos.x < 0, pos.y < 0);
	ivec2 tilepos = (pos - chunkpos * 32) % 32;
	const Chunk *chunk = getChunk(chunkpos);
	return chunk->at(tilepos);
}

Tile& World::at(const ivec2 &pos) {
	return this->operator[](pos);
}

const Tile& World::at(const ivec2 &pos) const {
	return this->operator[](pos);
}

Tile World::getTileOrEmpty(const ivec2 &pos) const {
	Tile tile;
	try {
		tile = this->operator[](pos);
	} catch(std::exception &e) {}
	return tile;
}