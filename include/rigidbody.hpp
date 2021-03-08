#pragma once

#include <array>
#include <fstream>
#include <memory>

#include <math/matrix.hpp>
#include <math/vector.hpp>

#include "entity.hpp"
#include "world.hpp"

class RigidBody : public Entity {
public:
	enum sides {
		none = 0,
		left = 1,
		right = 2,
		top = 4,
		bottom = 8
	};

	RigidBody(std::shared_ptr<TiledTexture> texture);
	~RigidBody() override = default;

	void applyForce(vec2 f);
	bool checkIntersection(vec2 bl, vec2 tr);
	virtual void update(float time, float dt, World *world) override;
	std::vector<ivec2> getCollidingTiles();
	bool onGround();

	vec2 getPos();

protected:
	// body information
	float mass = 1.0f;
	vec2 pos = vec2(0);
	vec2 speed = vec2(0);
	vec2 acceleration = vec2(0);
	vec2 hitbox = vec2(1);

	// environment
	// friction is only applied on the horizontal axis
	float groundFriction = 0.9;
	float airFriction = 0.99;
	vec2 gravity = vec2(0, -50);

	// other config stuff
	float maxSpeed = 1000.0f;

	// collision
	uint8_t collision;
	std::vector<ivec2> collidingTiles;
};