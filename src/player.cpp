#include <player.hpp>

Player::Player(Camera *cam, const std::shared_ptr<TiledTexture> &texture) : RigidBody(texture), cam(cam) {
	hitbox = vec2(2, 2);
	pos = vec2(-2, 0);
}

void Player::update(float time, float dt, World *world) {
	RigidBody::update(time, dt, world);
	if(onGround()) {
		speed.x = clamp(speed.x, -10.0f, 10.0f);
	}
	cam->pos = vec3(this->pos, cam->pos.z);
}