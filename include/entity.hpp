#pragma once

#include <array>

#include <math/matrix.hpp>
#include <math/vector.hpp>

#include <opengl/mesh.hpp>
#include <opengl/program.hpp>
#include <opengl/texture.hpp>
#include <opengl/uniform.hpp>
#include <opengl/vertex.hpp>

#include "chunk.hpp"
#include "resources.hpp"
#include "tile.hpp"

using namespace math;

class World;

class Entity {
public:
	Entity(std::shared_ptr<TiledTexture> texture = {});
	virtual ~Entity();

	virtual void update(float time, float dt, World *world);
	virtual void render();
	virtual bool customRenderFunction() const;

	virtual void shift(ivec2 dir);

	mat4 getTransform();
	mat4 getUVTransform();
	void setTexturePtr(std::shared_ptr<TiledTexture> texture);
	TiledTexture* getTexturePtr();

	vec3 pos, rot, scale = vec3(1);

protected:
	mat4 transform, uvtransform;
	std::shared_ptr<TiledTexture> texture;
};