#include <player.hpp>

Player::Player(Camera *cam, const std::shared_ptr<TiledTexture> &texture) : RigidBody(texture), cam(cam) {
	hitbox = vec2(1, 2);
	pos = vec2(-2, 0);
}

void Player::update(float time, float dt, World *world) {
	RigidBody::update(time, dt, world);
	if(onGround()) {
		speed.x = clamp(speed.x, -maxGroundSpeed, maxGroundSpeed);
	}
	else {
		speed.x = clamp(speed.x, -maxSpeed.x, maxSpeed.x);
		speed.y = clamp(speed.y, -maxSpeed.y, maxSpeed.y);
	}
	if(pos.y < -32) {
		speed = 0;
		pos = vec2(0, 5);
	}
	cam->pos = vec3(this->pos, cam->pos.z);

	dashTimer += dt;
	jumpTimer += dt;
	switch(state) {
		case State::dash: {
			if(dashTimer < dashTime) {
				speed.y = 0;
				if(dir == Dir::left) {
					speed.x = -dashSpeed;
				}
				else if(dir == Dir::right) {
					speed.x = dashSpeed;
				}
			}
			else {
				speed = speed2;
				if(onGround()) {
					if(abs(speed.x) > 0.001) {
						state = State::walk;
					}
					else {
						state = State::idle;
					}
				}
				else {
					state = State::fall;
				}
			}
		} break;
		case State::jump: {
			if(!onGround()) {
				state = State::fall;
			} else {
				state = State::idle;
			}
		} break;
	}
}

void Player::moveLeft() {
	dir = Dir::left;
	if(onGround()) {
		state = State::walk;
		applyForce(vec2(-150, 0));
	}
	else {
		applyForce(vec2(-50, 0));
	}
}

void Player::moveRight() {
	dir = Dir::right;
	if(onGround()) {
		state = State::walk;
		applyForce(vec2(150, 0));
	}
	else {
		applyForce(vec2(50, 0));
	}
}

void Player::jump() {
	if(onGround() && state != State::jump && jumpTimer > jumpTimeout) {
		jumpTimer = 0;
		state = State::jump;
		dir = Dir::up;
		pos.y += 0.1;
		collision ^= bottom;
		applyForce(vec2(0, 3000));
	}
}

void Player::dash() {
	speed2 = speed;
	state = State::dash;
	dashTimer = 0;
}

void Player::attack() {
	;
}

void Player::heal() {
	;
}
