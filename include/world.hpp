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
#include "resources.hpp"

using namespace math;

class RigidBody;

class World {
public:
	World(const std::string &shader, const Camera *cam);
	~World();

	void loadShader(const std::string &path);
	void init();

	void setTileSetPtr(const std::shared_ptr<TileSet> &tileset);

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
	void render(mat4 proj);
	void renderCollisions(RigidBody *entity);

	Tile& operator[](const ivec2 &pos);
	const Tile& operator[](const ivec2 &pos) const;
	Tile& at(const ivec2 &pos);
	const Tile& at(const ivec2 &pos) const;

	Tile getTileOrEmpty(const ivec2 &pos) const;

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

//private:
	bool autogrow = false;
	opengl::Mesh<vec3, vec2> unitplane;
	opengl::UniformBuffer<ModelInfo> modelInfoUBO;
	opengl::UniformBuffer<CameraInfo> cameraInfoUBO;
	opengl::UniformBuffer<RenderInfo> renderInfoUBO;
	opengl::Program shader;

	std::vector<std::unique_ptr<Chunk>> chunks;
	std::vector<std::unique_ptr<Entity>> entities;
	std::shared_ptr<TileSet> tileset;
	const Camera *cam;
};