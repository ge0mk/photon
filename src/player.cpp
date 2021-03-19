#include <player.hpp>

const std::vector<std::vector<ivec2>> Player::animations = {
	{ivec2(0, 0), ivec2(1, 0), ivec2(2, 0)},
	{ivec2(3, 0), ivec2(4, 0), ivec2(5, 0), ivec2(6, 0)},
	{ivec2(7, 0), ivec2(8, 0), ivec2(9, 0)},
	{ivec2(10, 0), ivec2(11, 0)},
	{ivec2(12, 0)},
	{ivec2(13, 0), ivec2(14, 0), ivec2(15, 0), ivec2(0, 1), ivec2(1, 1)},
	{ivec2(2, 1), ivec2(3, 1), ivec2(4, 1), ivec2(5, 1), ivec2(6, 1), ivec2(7, 1)},
	{ivec2(8, 1), ivec2(9, 1), ivec2(10, 1), ivec2(11, 1), ivec2(12, 1), ivec2(13, 1)},
	{ivec2(14, 1), ivec2(15, 1), ivec2(0, 2), ivec2(1, 2), ivec2(2, 2), ivec2(3, 2), ivec2(4, 2), ivec2(5, 2), ivec2(6, 2)},
	{ivec2(7, 2), ivec2(8, 2), ivec2(9, 2), ivec2(10, 2), ivec2(11, 2), ivec2(12, 2)},
	{ivec2(13, 2), ivec2(14, 2), ivec2(15, 2), ivec2(0, 3)},
	{ivec2(1, 3), ivec2(2, 3), ivec2(3, 3), ivec2(4, 3)},
	{ivec2(5, 3), ivec2(6, 3), ivec2(7, 3), ivec2(8, 3), ivec2(9, 3)},
	{ivec2(10, 3), ivec2(11, 3), ivec2(12, 3), ivec2(13, 3)},
	{ivec2(14, 3), ivec2(15, 3)},
	{ivec2(0, 4), ivec2(1, 4), ivec2(2, 4), ivec2(3, 4)},
	{ivec2(4, 4), ivec2(5, 4), ivec2(6, 4), ivec2(7, 4)},
	{ivec2(8, 4), ivec2(9, 4), ivec2(10, 4), ivec2(11, 4), ivec2(12, 4), ivec2(13, 4)},
	{ivec2(14, 4), ivec2(15, 4), ivec2(0, 5), ivec2(1, 5), ivec2(2, 5), ivec2(3, 5), ivec2(4, 5)},
	{ivec2(5, 5), ivec2(6, 5), ivec2(7, 5), ivec2(8, 5)},
	{ivec2(9, 5), ivec2(10, 5)},
	{ivec2(11, 5), ivec2(12, 5), ivec2(13, 5), ivec2(14, 5), ivec2(15, 5), ivec2(0, 6), ivec2(1, 6)},
	{ivec2(2, 6), ivec2(3, 6), ivec2(4, 6)},
	{ivec2(5, 6), ivec2(6, 6), ivec2(7, 6), ivec2(8, 6)},
	{ivec2(9, 6), ivec2(10, 6), ivec2(11, 6), ivec2(12, 6)},
	{ivec2(13, 6), ivec2(14, 6), ivec2(15, 6)},
	{ivec2(0, 7), ivec2(1, 7)},
	{ivec2(2, 7), ivec2(3, 7), ivec2(4, 7), ivec2(5, 7), ivec2(6, 7), ivec2(7, 7), ivec2(8, 7), ivec2(9, 7)},
	{ivec2(10, 7), ivec2(11, 7), ivec2(12, 7), ivec2(13, 7), ivec2(14, 7), ivec2(15, 7), ivec2(0, 8)},
	{ivec2(1, 8), ivec2(2, 8), ivec2(3, 8), ivec2(4, 8)},
	{ivec2(5, 8), ivec2(6, 8), ivec2(7, 8), ivec2(8, 8), ivec2(9, 8), ivec2(10, 8), ivec2(11, 8), ivec2(12, 8), ivec2(13, 8), ivec2(14, 8), ivec2(15, 8), ivec2(0, 9), ivec2(1, 9)},
	{ivec2(2, 9), ivec2(3, 9), ivec2(4, 9), ivec2(5, 9), ivec2(6, 9), ivec2(7, 9)},
	{ivec2(8, 9), ivec2(9, 9), ivec2(10, 9), ivec2(11, 9), ivec2(12, 9), ivec2(13, 9), ivec2(14, 9)},
	{ivec2(15, 9), ivec2(0, 10), ivec2(1, 10), ivec2(2, 10), ivec2(3, 10), ivec2(4, 10)},
	{ivec2(5, 10), ivec2(6, 10), ivec2(7, 10), ivec2(8, 10), ivec2(9, 10), ivec2(10, 10)},
	{ivec2(11, 10), ivec2(12, 10)},
	{ivec2(13, 10), ivec2(14, 10), ivec2(15, 10), ivec2(0, 11)},
	{ivec2(1, 11), ivec2(2, 11), ivec2(3, 11)},
	{ivec2(4, 11), ivec2(5, 11), ivec2(6, 11), ivec2(7, 11)},
	{ivec2(8, 11), ivec2(9, 11), ivec2(10, 11), ivec2(11, 11)},
	{ivec2(12, 11), ivec2(13, 11), ivec2(14, 11), ivec2(15, 11), ivec2(0, 12), ivec2(1, 12)},
	{ivec2(2, 12), ivec2(3, 12), ivec2(4, 12), ivec2(5, 12), ivec2(6, 12), ivec2(7, 12)},
	{ivec2(8, 12), ivec2(9, 12)},
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
				if(inputState(walk)) {
					speed.x = walkSpeed * (inputState(move) < 0 ? -1 : 1);
				}
				else {
					speed.x = sprintSpeed * (inputState(move) < 0 ? -1 : 1);
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
				if(inputState(walk)) {
					speed.x = walkSpeed;
				}
				else {
					speed.x = sprintSpeed;
				}
				if(mPushesRightWall) {
					speed.x = 0.0f;
				}
				scale.x = abs(scale.x);
			}
			else if(inputState(move) < 0.0f) {
				if(inputState(walk)) {
					speed.x = -walkSpeed;
				}
				else {
					speed.x = -sprintSpeed;
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
				if(inputState(walk)) {
					speed.x = walkSpeed;
				}
				else {
					speed.x = sprintSpeed;
				}
				if(mPushesRightWall) {
					speed.x = 0.0f;
				}
				scale.x = abs(scale.x);
			}
			else if(inputState(move) < 0.0f) {
				if(inputState(walk)) {
					speed.x = -walkSpeed;
				}
				else {
					speed.x = -sprintSpeed;
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
				if(inputState(walk)) {
					speed.x = walkSpeed;
				}
				else {
					speed.x = sprintSpeed;
				}
				if(mPushesRightWall) {
					speed.x = 0.0f;
				}
				scale.x = abs(scale.x);
			}
			else if(inputState(move) < 0.0f) {
				if(inputState(walk)) {
					speed.x = -walkSpeed;
				}
				else {
					speed.x = -sprintSpeed;
				}
				if(mPushesLeftWall) {
					speed.x = 0.0f;
				}
				scale.x = -abs(scale.x);
			}
			if(inputState(jump) && (jumptime > jumpanimtime || jumptime < 0.0f) && doublejump < doublejumpcount) {
				doublejump++;
				speed.y = jumpSpeed;
				state = State::jump;
				jumptime = 0.0f;
			}
			else if(mOnGround) {
				state = State::idle;
				doublejump = 0;
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
			if(!inputState(walk)) {
				int frames = animations[a_run].size();
				uvpos = animations[a_run][int(time * frames * 1.5f) % frames];
			}
			else {
				int frames = animations[a_walk].size();
				uvpos = animations[a_walk][int(time * frames * 1.8f) % frames];
			}
		} break;
		case State::jump:
		case State::fall: {
			if(jumptime >= 0.0f && jumptime < jumpanimtime) {	// jump
				if(doublejump) {
					int frames = animations[a_smrslt].size();
					uvpos = animations[a_smrslt][int(jumptime / jumpanimtime * frames) % frames];
				}
				else {
					int frames = animations[a_jump].size();
					std::cout<<frames<<"\n";
					uvpos = animations[a_jump][int(jumptime / jumpanimtime * frames) % frames];
				}
			}
			else {	// fall
				jumptime = -1.0f;
				playAnimation(a_fall);
				int frames = animations[a_fall].size();
				uvpos = animations[a_fall][int(time * frames * 2.0f) % frames];
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