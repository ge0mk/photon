#pragma once

#include "rigidbody.hpp"
#include "camera.hpp"

class Player : public RigidBody {
public:
	enum Action : uint8_t {
		move = 0,
		jump,
		dash,
		sneak,
		count
	};

	enum class State {
		idle = 0,
		walk, sneak,
		jump, fall,
		dash, grab,
		count
	};

	Player(Camera *cam, const std::shared_ptr<TiledTexture> &sprites);

	void update(float time, float dt, World *world) override;
	void setInput(uint8_t action, float value);

protected:
	bool inputStarted(uint8_t action);
	bool inputStopped(uint8_t action);
	float inputState(uint8_t action);

	std::array<float, Action::count> inputs, prevInputs;
	State state = State::idle;
	float jumpSpeed = 10, walkSpeed = 5, sneakSpeed = 2;
	Camera *cam;
};
