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

	switch(state) {
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
	if(onGround()) {
		state = State::walk;
		dir = Dir::left;
		applyForce(vec2(-150, 0));
	}
	else {
		applyForce(vec2(-50, 0));
	}
}

void Player::moveRight() {
	if(onGround()) {
		state = State::walk;
		dir = Dir::right;
		applyForce(vec2(150, 0));
	}
	else {
		applyForce(vec2(50, 0));
	}
}

void Player::jump() {
	if(onGround() && state != State::jump) {
		state = State::jump;
		dir = Dir::up;
		pos.y += 0.1;
		applyForce(vec2(0, 3000));
	}
}

void Player::dash() {
	std::cout<<"dash\n";
}

void Player::attack() {
	;
}

void Player::heal() {
	;
}
