#pragma once

#include "rigidbody.hpp"
#include "camera.hpp"

class Player : public RigidBody {
public:
	enum Action : uint8_t {
		move = 0,
		walk,
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

	enum Animation : uint8_t {
		a_air_attack_3_end,
		a_air_attack1,
		a_air_attack2,
		a_air_attack3_loop,
		a_air_attack3_rdy,
		a_attack1,
		a_attack2,
		a_attack3,
		a_bow,
		a_bow_jump,
		a_cast,
		a_cast_loop,
		a_crnr_clmb,
		a_crnr_grb,
		a_crnr_jmp,
		a_crouch,
		a_crouch_jump,
		a_crouch_walk,
		a_die,
		a_drop_kick,
		a_fall,
		a_get_up,
		a_hurt,
		a_idle,
		a_idle_2,
		a_items,
		a_jump,
		a_kick,
		a_knock_dwn,
		a_ladder_climb,
		a_punch,
		a_run,
		a_run_punch,
		a_run2,
		a_run3,
		a_slide,
		a_smrslt,
		a_stand,
		a_swrd_drw,
		a_swrd_shte,
		a_walk,
		a_wall_run,
		a_wall_slide,
	};

	Player(Camera *cam, const std::shared_ptr<TiledTexture> &sprites);

	void update(float time, float dt, WorldContainer &world) override;
	void updateAnimation(float time);
	void setInput(uint8_t action, float value);
	void playAnimation(uint8_t animation);

protected:
	bool inputStarted(uint8_t action) const;
	bool inputStopped(uint8_t action) const;
	float inputState(uint8_t action) const;

	std::array<float, Action::count> inputs = {}, prevInputs = {};
	State state = State::idle;
	float jumpSpeed = 128, walkSpeed = 64, sprintSpeed = 128, sneakSpeed = 5, dashSpeed = 1024;
	Camera *cam;
	float animspeed = 2.0f;

	float jumptime = 0.0f, falltime = 0.0f, dashTime = 0.0f;
	const float dashDuration = 0.15f;
	const float jumpanimtime = 0.5f;
	const float dashCountdown = 1.0f;

	uint8_t doublejump = 0;
	const uint8_t doublejumpcount = 1;

	ivec2 uvpos;
	uint8_t animationState;

	static const std::vector<std::vector<ivec2>> animations;
};
