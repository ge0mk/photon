#include <player.hpp>

const std::vector<std::vector<ivec2>> Player::animations = {
	{ivec2(1, 6), ivec2(2, 6), ivec2(3, 6), ivec2(4, 6)},
	{ivec2(5, 6), ivec2(6, 6), ivec2(7, 6), ivec2(8, 6)},
	{ivec2(10, 11), ivec2(11, 11), ivec2(12, 11), ivec2(13, 11), ivec2(14, 11), ivec2(15, 11)},
	{ivec2(13, 9), ivec2(14, 9), ivec2(15, 9), ivec2(0, 10), ivec2(1, 10), ivec2(2, 10)},
	{ivec2(0, 9), ivec2(1, 9), ivec2(2, 9), ivec2(3, 9), ivec2(4, 9), ivec2(5, 9)},
	{ivec2(5, 5), ivec2(6, 5)},
	{ivec2(14, 6), ivec2(15, 6)},
	{ivec2(12, 6), ivec2(13, 6), ivec2(14, 6), ivec2(15, 6)},
};

Player::Player(Camera *cam, const std::shared_ptr<TiledTexture> &texture) : RigidBody(texture), cam(cam) {
	pos = vec2(0, 0);
	AABB::halfSize = vec2(6, 15);
	aabbOffset = vec2(8, 10);
	scale = vec2(50, 37);
	uvpos = ivec2(1, 6);
}

void Player::update(float time, float dt, World *world) {
	if(jumptime >= 0) {
		jumptime += dt;
	}
	switch(state) {
		case State::idle: {
			speed = 0;
			if(!mOnGround) {
				state = State::fall;
			}
			else if(inputState(jump) != 0.0f) {
				speed.y = jumpSpeed;
				state = State::jump;
				jumptime = 0.0f;
			}
			else if(inputState(move) != 0.0f) {
				if(inputState(sprint)) {
					speed.x = sprintSpeed * (inputState(move) < 0 ? -1 : 1);
				}
				else {
					speed.x = walkSpeed * (inputState(move) < 0 ? -1 : 1);
				}
				state = State::walk;
			}
		} break;
		case State::walk: {
			if(inputState(move) == 0.0f) {
				state = State::idle;
				speed = 0;
			}
			else if(inputState(move) > 0.0f) {
				if(inputState(sprint)) {
					speed.x = sprintSpeed;
				}
				else {
					speed.x = walkSpeed;
				}
				if(mPushesRightWall) {
					speed.x = 0.0f;
				}
				scale.x = abs(scale.x);
			}
			else if(inputState(move) < 0.0f) {
				if(inputState(sprint)) {
					speed.x = -sprintSpeed;
				}
				else {
					speed.x = -walkSpeed;
				}
				if(mPushesLeftWall) {
					speed.x = 0.0f;
				}
				scale.x = -abs(scale.x);
			}
			if(inputState(jump)) {
				speed.y = jumpSpeed;
				state = State::jump;
				jumptime = 0.0f;
			}
			else if(!mOnGround) {
				state = State::fall;
			}
		} break;
		case State::sneak: {} break;
		case State::jump: {
			state = State::fall;
			if(inputState(move) == 0.0f) {
				speed.x = 0;
			}
			else if(inputState(move) > 0.0f) {
				if(inputState(sprint)) {
					speed.x = sprintSpeed;
				}
				else {
					speed.x = walkSpeed;
				}
				if(mPushesRightWall) {
					speed.x = 0.0f;
				}
				scale.x = abs(scale.x);
			}
			else if(inputState(move) < 0.0f) {
				if(inputState(sprint)) {
					speed.x = -sprintSpeed;
				}
				else {
					speed.x = -walkSpeed;
				}
				if(mPushesLeftWall) {
					speed.x = 0.0f;
				}
				scale.x = -abs(scale.x);
			}
		} break;
		case State::fall: {
			if(inputState(move) == 0.0f) {
				speed.x = 0;
			}
			else if(inputState(move) > 0.0f) {
				if(inputState(sprint)) {
					speed.x = sprintSpeed;
				}
				else {
					speed.x = walkSpeed;
				}
				if(mPushesRightWall) {
					speed.x = 0.0f;
				}
				scale.x = abs(scale.x);
			}
			else if(inputState(move) < 0.0f) {
				if(inputState(sprint)) {
					speed.x = -sprintSpeed;
				}
				else {
					speed.x = -walkSpeed;
				}
				if(mPushesLeftWall) {
					speed.x = 0.0f;
				}
				scale.x = -abs(scale.x);
			}
			if(inputState(jump) && (jumptime > jumpanimtime || jumptime < 0.0f)) {
				speed.y = jumpSpeed;
				state = State::jump;
				jumptime = 0.0f;
			}
			else if(mOnGround) {
				state = State::idle;
			}
		} break;
		case State::dash: {} break;
		case State::grab: {} break;
		default: break;
	}

	updateAnimation(time, dt, world);

	RigidBody::update(time, dt, world);
	transform = mat4().translate(rpos + vec3(0, 2.5, 0)).scale(vec3(scale, 1));
	uvtransform = texture->getUVTransform(uvpos);
	cam->pos.xy = rpos;

	if(inputState(dash) != 0.0f) {
		gravity.y = abs(gravity.y);
	}
	else {
		gravity.y = -abs(gravity.y);
	}

	prevInputs = inputs;
	inputs = {0.0f, 0.0f, 0.0f, 0.0f};
}

void Player::updateAnimation(float time, float dt, World *world) {
	switch(state) {
		case State::idle: {
			int frames = animations[a_idle].size();
			uvpos = animations[a_idle][int(time * frames) % frames];
		} break;
		case State::walk: {
			if(inputState(sprint)) {
				int frames = animations[a_run].size();
				uvpos = animations[a_run][int(time * frames) % frames];
			}
			else {
				int frames = animations[a_walk].size();
				uvpos = animations[a_walk][int(time * frames) % frames];
			}
		} break;
		case State::jump:
		case State::fall: {
			if(jumptime >= 0.0f && jumptime < jumpanimtime) {	// jump
				int frames = animations[a_jump].size();
				uvpos = animations[a_jump][int(jumptime / jumpanimtime * frames) % frames];
			}
			else {	// fall
				jumptime = -1.0f;
				playAnimation(a_fall);
				int frames = animations[a_fall].size();
				uvpos = animations[a_fall][int(time * frames) % frames];
			}
		} break;
		default: break;
	}
}

void Player::setInput(uint8_t action, float value) {
	if(action == move) {
		inputs[action] = clamp(value, -1.0f, 1.0f);
	}
	else {
		inputs[action] = clamp(value, 0.0f, 1.0f);
	}
}

void Player::playAnimation(uint8_t animation) {
	animationState = animation;
}

bool Player::inputStarted(uint8_t action) const {
	return (inputs[action] != 0) && (prevInputs[action] == 0);
}

bool Player::inputStopped(uint8_t action) const {
	return (inputs[action] == 0) && (prevInputs[action] != 0);
}

float Player::inputState(uint8_t action) const {
	return inputs[action];
}