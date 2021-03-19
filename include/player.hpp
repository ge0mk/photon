#pragma once

#include "rigidbody.hpp"
#include "camera.hpp"

class Player : public RigidBody {
public:
	enum Action : uint8_t {
		move = 0,
		sprint,
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

	enum Animation {
		a_idle,
		a_idle_sword,
		a_walk,
		a_run,
		a_run_sword,
		a_fall,
		a_jump,
		a_crouch_jump,
	};

	Player(Camera *cam, const std::shared_ptr<TiledTexture> &sprites);

	void update(float time, float dt, World *world) override;
	void updateAnimation(float time, float dt, World *world);
	void setInput(uint8_t action, float value);
	void playAnimation(uint8_t animation);

protected:
	bool inputStarted(uint8_t action) const;
	bool inputStopped(uint8_t action) const;
	float inputState(uint8_t action) const;

	std::array<float, Action::count> inputs, prevInputs;
	State state = State::idle;
	float jumpSpeed = 128, walkSpeed = 40, sprintSpeed = 100, sneakSpeed = 5;
	Camera *cam;
	float jumptime;
	const float jumpanimtime = 0.5f;

	ivec2 uvpos;
	uint8_t animationState;

	static const std::vector<std::vector<ivec2>> animations;
};
