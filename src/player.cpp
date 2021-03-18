#include <player.hpp>

Player::Player(Camera *cam, const std::shared_ptr<TiledTexture> &texture) : RigidBody(texture), cam(cam) {
	pos = vec2(0, 5);
	AABB::halfSize = vec2(0.5, 1.2);
	aabbOffset = vec2(0.5, 1.2);
	scale = vec2(50, 37) / 37.0f * 3.0f;
	uvpos = ivec2(1, 6);
}

void Player::update(float time, float dt, World *world) {
	switch(state) {
		case State::idle: {
			speed = 0;
			if(!mOnGround) {
				state = State::fall;
			}
			else if(inputState(jump) != 0.0f) {
				speed.y = jumpSpeed;
				state = State::jump;
			}
			else if(inputState(move) != 0.0f) {
				state = State::walk;
			}
		} break;
		case State::walk: {
			if(inputState(move) == 0.0f) {
				state = State::idle;
				speed = 0;
			}
			else if(inputState(move) > 0.0f) {
				speed.x = walkSpeed;
				if(mPushesRightWall) {
					speed.x = 0.0f;
				}
				scale.x = abs(scale.x);
			}
			else if(inputState(move) < 0.0f) {
				speed.x = -walkSpeed;
				if(mPushesLeftWall) {
					speed.x = 0.0f;
				}
				scale.x = -abs(scale.x);
			}
			if(inputState(jump)) {
				speed.y = jumpSpeed;
				state = State::jump;
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
				speed.x = walkSpeed;
				if(mPushesRightWall) {
					speed.x = 0.0f;
				}
				scale.x = abs(scale.x);
			}
			else if(inputState(move) < 0.0f) {
				speed.x = -walkSpeed;
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
				speed.x = walkSpeed;
				if(mPushesRightWall) {
					speed.x = 0.0f;
				}
				scale.x = abs(scale.x);
			}
			else if(inputState(move) < 0.0f) {
				speed.x = -walkSpeed;
				if(mPushesLeftWall) {
					speed.x = 0.0f;
				}
				scale.x = -abs(scale.x);
			}
			if(mOnGround) {
				state = State::idle;
			}
		} break;
		case State::dash: {} break;
		case State::grab: {} break;
		default: break;
	}

	updateAnimation(time, dt, world);

	RigidBody::update(time, dt, world);
	transform = mat4().translate(rpos + vec3(0.0f, 0.315f, 0.0f)).scale(vec3(scale, 1)).translate(vec3(0, -0.0275, 0));
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

bool Player::inputStarted(uint8_t action) const {
	return (inputs[action] != 0) && (prevInputs[action] == 0);
}

bool Player::inputStopped(uint8_t action) const {
	return (inputs[action] == 0) && (prevInputs[action] != 0);
}

float Player::inputState(uint8_t action) const {
	return inputs[action];
}