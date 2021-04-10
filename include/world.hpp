#pragma once

#include <array>
#include <fstream>
#include <memory>

#include <math/matrix.hpp>
#include <math/vector.hpp>

#include <opengl/program.hpp>
#include <opengl/texture.hpp>
#include <opengl/uniform.hpp>

#include <utils/image.hpp>

#include "camera.hpp"
#include "chunk.hpp"
#include "entity.hpp"
#include "particles.hpp"
#include "resources.hpp"
#include "text.hpp"

#include <mutex>

using namespace math;

class WorldContainer {
public:
	WorldContainer() = default;
	WorldContainer(const WorldContainer &other) = default;
	~WorldContainer() = default;

	WorldContainer& operator=(const WorldContainer &other) = default;

	void setChunk(lvec2 pos, const std::shared_ptr<Chunk> &chunk);
	std::shared_ptr<Chunk> getChunk(lvec2 pos);
	std::shared_ptr<Chunk> getChunk(lvec2 pos) const;
	void eraseChunk(lvec2 pos);

	virtual std::shared_ptr<Chunk> loadChunk(lvec2 pos) = 0;

	void setChunkAbsolute(lvec2 pos, const std::shared_ptr<Chunk> &chunk);
	std::shared_ptr<Chunk> getChunkAbsolute(lvec2 pos);
	std::shared_ptr<Chunk> getChunkAbsolute(lvec2 pos) const;
	void eraseChunkAbsolute(lvec2 pos);

	template<typename T, typename ...Args>
	std::shared_ptr<T> createEntity(Args ...args) {
		std::shared_ptr<T> entity = std::shared_ptr<T>(new T(args...));
		m_entities.push_back(entity);
		return entity;
	}

	const std::map<lvec2, std::shared_ptr<Chunk>>& chunks() const;
	const std::vector<std::shared_ptr<Entity>>& entities() const;

	Tile& at(lvec2 tileoffset);
	Tile at(lvec2 tileoffset) const;

	Tile& operator[](lvec2 tileoffset);
	Tile operator[](lvec2 tileoffset) const;

	lvec2 getTileIndex(vec2 pixel) const;
	lvec2 snapToGrid(vec2 pos) const;

	virtual void shift(lvec2 offset);
	lvec2 offset() const;

	Image renderTileProperties() const;
	Image renderLightingData() const;

protected:
	std::map<lvec2, std::shared_ptr<Chunk>> m_chunks;
	std::vector<std::shared_ptr<Entity>> m_entities;

	lvec2 m_offset;
};

class WorldGenerator {
public:
	WorldGenerator(const WorldContainer &container, vec2 tileScale);
	virtual std::shared_ptr<Chunk> getChunk(lvec2 pos);

private:
	const WorldContainer &container;

public:
	vec2 tileScale;
};

class WorldRenderer {
public:
	WorldRenderer(const WorldContainer &container, const Camera &cam, const std::shared_ptr<Entity> &mainEntity, const std::shared_ptr<TiledTexture> &texture);
	virtual void update();
	virtual void render();

	mat4 getCamTransform();
	std::mutex& getCameraMutex();

protected:
	struct ModelInfo {
		mat4 transform, uvtransform;
	};

	struct CameraInfo {
		mat4 proj, view;
	};

	struct RenderInfo {
		vec4 tint;
		vec2 res;
		float time, dt;
	};

	const WorldContainer &container;
	const Camera &cam;

	std::shared_ptr<Entity> mainEntity;

	opengl::Program prog;
	opengl::ComputeProgram distanceCalculator;

	opengl::Mesh<vec3, vec2> unitplane;
	opengl::UniformBuffer<ModelInfo> modelInfoUBO;
	opengl::UniformBuffer<CameraInfo> cameraInfoUBO;
	opengl::UniformBuffer<RenderInfo> renderInfoUBO;

	std::shared_ptr<TiledTexture> texture;
	opengl::Texture distanceMapRaw;
	opengl::Texture distanceMap;

	std::mutex cameraMutex;
};

template<typename Generator_t = WorldGenerator, typename Renderer_t = WorldRenderer>
class DynamicWorld : public WorldContainer {
public:
	DynamicWorld(const std::shared_ptr<Entity> &mainEntity = {}) : mainEntity(mainEntity) {}

	void setMainEntity(const std::shared_ptr<Entity> &mainEntity) {
		this->mainEntity = mainEntity;
	}

	template<typename ...Args>
	void initGenerator(Args &&...args) {
		generator = std::unique_ptr<Generator_t>(new Generator_t(*this, args...));
	}

	template<typename ...Args>
	void initRenderer(Args &&...args) {
		renderer = std::unique_ptr<Renderer_t>(new Renderer_t(*this, args...));
	}

	void initParticleSystem(const std::shared_ptr<TiledTexture> texture = {}) {
		particleSystem = std::unique_ptr<ParticleSystem>(new ParticleSystem(texture));
	}

	void initTextRenderer(freetype::Font &&font) {
		textRenderer = std::unique_ptr<TextRenderer>(new TextRenderer(std::move(font)));
		textRenderer->createObject("Hello World!", mat4().scale(0.2), vec4(1));
	}

	void update(float time, float dt) {
		updateMainEntity(time, dt);
		updateChunks(time, dt);

		for(auto &entity : entities()) {
			entity->update(time, dt, *this);
		}

		updateParticles(time, dt);
		textRenderer->update();

		if(updateLight) {
			renderer->update();
			exit(0);
			updateLight = false;
		}
	}

	void render() {
		mat4 transform = renderer->getCamTransform();
		if(renderer) {
			renderer->render();
		}

		particleSystem->render(transform);
		textRenderer->render(transform);
	}

	void shift(lvec2 offset) override {
		WorldContainer::shift(offset);
		particleSystem->shift(offset);
		textRenderer->applyTransform(mat4().translate(vec3(vec2(offset) * Chunk::size * Tile::resolution)));
	}

protected:
	std::shared_ptr<Chunk> loadChunk(lvec2 pos) override {
		auto chunk = std::as_const(*this).getChunkAbsolute(pos);
		if(!chunk) {
			if(generator) {
				chunk = generator->getChunk(pos);
			}
			else {
				chunk = std::shared_ptr<Chunk>(new Chunk(*this, pos, generator->tileScale));
			}
			setChunkAbsolute(pos, chunk);
		}
		return chunk;
	}

	void updateMainEntity(float time, float dt) {
		renderer->getCameraMutex().lock();
		ivec2 shiftDir;

		if(mainEntity->pos.x < 0.0f) {
			shiftDir += ivec2(1, 0);
		}
		else if(mainEntity->pos.x > Chunk::size * Tile::resolution) {
			shiftDir += ivec2(-1, 0);
		}

		if(mainEntity->pos.y < 0.0f) {
			shiftDir += ivec2(0, 1);
		}
		else if(mainEntity->pos.y > Chunk::size * Tile::resolution) {
			shiftDir += ivec2(0, -1);
		}

		shift(shiftDir);
		mainEntity->shift(shiftDir);

		mainEntity->update(time, dt, *this);

		renderer->getCameraMutex().unlock();
	}

	void updateChunks(float time, float dt) {
		std::vector<lvec2> outOfRangeChunks;

		for(auto &[pos, chunk] : chunks()) {
			if(length(vec2(pos - m_offset)) > 8.0f) {
				outOfRangeChunks.push_back(pos);
			}
			else {
				chunk->update(time, dt);
			}
		}

		for(lvec2 pos : outOfRangeChunks) {
			eraseChunkAbsolute(pos);
		}

		for(int y = -4; y <= 4; y++) {
			for(int x = -4; x <= 4; x++) {
				if(!std::as_const(*this).getChunk(lvec2(x, y))) {
					loadChunk(lvec2(x, y) + offset());
				}
			}
		}
	}

	void updateParticles(float time, float dt) {
		if(particleSystem->size() < 8192) {
			for(unsigned i = 0; i < 16; i++) {
				vec2 pos = vec2(rand(-1024.0f, 1536.0f), rand(256.0f, 512.0f)) - vec2(0, offset().y * Chunk::size * Tile::resolution);
				vec2 scale = vec2(1, 8) * rand(0.8, 1.2);
				vec2 speed = vec2(0.0, rand(-112.0f, -96.0f));
				vec2 gravity = vec2(0, -1);
				particleSystem->spawn(Particle::rain, pos, speed, gravity, scale, 0, 0);
			}
		}
		for(Particle &p : *particleSystem) {
			switch(p.type) {
				case Particle::rain: {
					if(p.speed.y == 0.0f || p.pos.y < -512.0f) {
						p.pos = vec2(rand(-1024.0f, 1536.0f), 512) - vec2(0, offset().y * Chunk::size * Tile::resolution);
						p.speed = vec2(0.0, rand(-112.0f, -96.0f));
					}
				} break;
			}
		}
		particleSystem->erase([](const Particle &p) -> bool {
			if(p.type == Particle::blood && p.lifetime > 10) {
				return true;
			}
			return false;
		});

		particleSystem->update(time, dt, *this);
	}

private:
	std::unique_ptr<ParticleSystem> particleSystem;
	std::unique_ptr<TextRenderer> textRenderer;
	std::unique_ptr<Generator_t> generator;
	std::unique_ptr<Renderer_t> renderer;

	std::shared_ptr<Entity> mainEntity;

	bool updateLight = false;
};