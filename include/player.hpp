#pragma once

#include "rigidbody.hpp"
#include "camera.hpp"

class Player : public RigidBody {
public:
	Player(Camera *cam, const std::shared_ptr<SpriteSheet> &sprites);
	virtual void update(float time, float dt, World *world) override;

private:
	Camera *cam;
};