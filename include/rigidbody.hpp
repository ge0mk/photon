#pragma once

#include <array>
#include <fstream>
#include <memory>

#include <math/matrix.hpp>
#include <math/vector.hpp>

#include "entity.hpp"
#include "world.hpp"

class AABB {
public:
	AABB(vec2 pos = 0, vec2 halfSize = 0.5);
	AABB(const AABB &other) = default;
	AABB& operator=(const AABB &other) = default;

	bool intersects(const AABB &other);

protected:
	vec2 pos;
	vec2 halfSize;
};

class RigidBody : public Entity, protected AABB {
public:
	static constexpr float resolution = 1.0f;

	RigidBody(std::shared_ptr<TiledTexture> texture);

	void update(float time, float dt, World *world) override;
	void applyForce(vec2 f);

	bool checkGround(World *world, float &groundY);
	bool checkCeiling(World *world, float &ceilingY);
	bool checkLeft(World *world, float &leftX);
	bool checkRight(World *world, float &rightX);

	const std::vector<ivec2>& getCollidingTiles() const;
	vec2 getPos() const;
	vec2 getSpeed() const;

public:
	vec2 pos = 0, oldPos = 0, rpos = 0;
	vec2 speed = 0, oldSpeed = 0;
	vec2 gravity = vec2(0, -256);
	vec2 acceleration = 0;
	vec2 forces = 0;
	vec2 scale = 1;
	float mass = 1;

	float maxspeed = 512.0f;

	vec2 aabbOffset = 0;

	bool mPushedRightWall = false;
	bool mPushesRightWall = false;

	bool mPushedLeftWall = false;
	bool mPushesLeftWall = false;

	bool mWasOnGround = false;
	bool mOnGround = false;

	bool mWasAtCeiling = false;
	bool mAtCeiling = false;

	std::vector<ivec2> collidingTiles;
};