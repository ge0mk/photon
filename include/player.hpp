#pragma once

#include "rigidbody.hpp"
#include "camera.hpp"

class Player : public RigidBody {
public:
	struct State {
		enum {
			idle = 0,
			walk = 1,
			fall = 2,
			jump = 4,
			grab = 8,
			dash = 16,
			attack = 32,
			heal = 64
		};
	};
	struct Dir {
		enum {
			none = 0,
			left = 1,
			up = 2,
			right = 4,
			down = 8
		};
	};

	Player(Camera *cam, const std::shared_ptr<TiledTexture> &sprites);
	virtual void update(float time, float dt, World *world) override;

	void moveLeft();
	void moveRight();
	void jump();
	void dash();
	void attack();
	void heal();

	inline vec2 snapToGrid(vec2 worldpos) {
		return ivec2(worldpos) - ivec2(worldpos.x < 0 ? 1 : 0, worldpos.y < 0 ? 1 : 0);
	}

private:
	Camera *cam;

	uint8_t dir;
	uint8_t state;

	// other config stuff
	vec2 maxSpeed = vec2(15.0f, 50);
	float maxGroundSpeed = 10.0f;
	const float dashTime = 0.2, jumpTimeout = 0.3;
	const float dashSpeed = 100;
	float dashTimer, jumpTimer;
	vec2 speed2;
};