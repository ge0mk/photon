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

#include <mutex>

using namespace math;

class RigidBody;

class World {
public:
	World(const std::string &shader, Camera &cam);
	~World();

	void loadShader(const std::string &path);
	void init();

	void setCameraHostPtr(const std::shared_ptr<Entity> &host);
	void setTexturePtr(const std::shared_ptr<TiledTexture> &texture);
	void setParticleTexturePtr(const std::shared_ptr<TiledTexture> &texture);

	Chunk* createChunk(lvec2 pos);
	Chunk* getChunk(lvec2 pos);
	const Chunk* getChunk(lvec2 pos) const;

	Chunk* generateFlatChunk(lvec2 pos);

	template<typename T, typename ...Args>
	std::shared_ptr<T> createEntity(Args ...args) {
		std::shared_ptr<T> entity = std::shared_ptr<T>(new T(args...));
		entities.push_back(entity);
		return entity;
	}

	std::shared_ptr<TextObject> createTextObject(const std::string &text, const mat4 &transform, vec4 color = vec4(1));

	void save(const std::string &path) const;
	void load(const std::string &path);

	void setAutoGrow(bool state);
	bool getAutoGrow();

	void update(float time, float dt);
	void render();
	void renderCollisions(std::vector<ivec2> tiles, std::shared_ptr<TiledTexture> texture = {});

	void shift(ivec2 dir);
	lvec2 getShiftOffset() const;

	Tile& operator[](const ivec2 &pos);
	const Tile& operator[](const ivec2 &pos) const;
	Tile& at(const ivec2 &pos);
	const Tile& at(const ivec2 &pos) const;

	Tile getTileOrEmpty(const ivec2 &pos) const;

	vec2 snapToGrid(vec2 worldpos) const;
	ivec2 getTileIndex(vec2 worldpos) const;

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
	std::vector<std::shared_ptr<Entity>> entities;
	std::shared_ptr<TiledTexture> texture;

	Camera &cam;
	std::mutex cameraMutex;
	std::shared_ptr<Entity> camHost;

	ParticleSystem particleSystem;
	TextRenderer textRenderer;

	Tile fallback;

	ivec2 shiftOffset = 0;
};