#pragma once

#include <array>
#include <fstream>
#include <memory>

#include <math/matrix.hpp>
#include <math/vector.hpp>

#include <opengl/program.hpp>
#include <opengl/texture.hpp>
#include <opengl/uniform.hpp>

#include "camera.hpp"
#include "chunk.hpp"
#include "entity.hpp"
#include "particles.hpp"
#include "resources.hpp"
#include "text.hpp"

using namespace math;

class RigidBody;

class World {
public:
	World(const std::string &shader, const Camera *cam);
	~World();

	void loadShader(const std::string &path);
	void init();

	void setTexturePtr(const std::shared_ptr<TiledTexture> &texture);
	void setParticleTexturePtr(const std::shared_ptr<TiledTexture> &texture);

	Chunk* createChunk(ivec2 pos);
	Chunk* getChunk(ivec2 pos);
	const Chunk* getChunk(ivec2 pos) const;

	template<typename T, typename ...Args>
	T* createEntity(Args ...args) {
		T *entity = new T(args...);
		entities.push_back(std::unique_ptr<Entity>(entity));
		return entity;
	}

	void save(const std::string &path) const;
	void load(const std::string &path);

	void setAutoGrow(bool state);
	bool getAutoGrow();

	void update(float time, float dt);
	void render();
	void renderCollisions(std::vector<ivec2> tiles, ivec2 pos, std::shared_ptr<TiledTexture> texture = {});

	Tile& operator[](const ivec2 &pos);
	const Tile& operator[](const ivec2 &pos) const;
	Tile& at(const ivec2 &pos);
	Tile at(const ivec2 &pos) const;

	Tile getTileOrEmpty(const ivec2 &pos) const;

	vec2 snapToGrid(vec2 worldpos);

	void createBloodParticles(vec2 pos);

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

private:
	bool autogrow = false;
	opengl::Mesh<vec3, vec2> unitplane;
	opengl::UniformBuffer<ModelInfo> modelInfoUBO;
	opengl::UniformBuffer<CameraInfo> cameraInfoUBO;
	opengl::UniformBuffer<RenderInfo> renderInfoUBO;
	opengl::Program shader;

	std::vector<std::unique_ptr<Chunk>> chunks;
	std::vector<std::unique_ptr<Entity>> entities;
	std::shared_ptr<TiledTexture> texture;
	const Camera *cam;

	ParticleSystem particleSystem;
	TextRenderer textRenderer;
};