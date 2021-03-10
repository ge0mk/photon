#include "rigidbody.hpp"

#include <algorithm>

RigidBody::RigidBody(std::shared_ptr<TiledTexture> texture) : Entity(texture) {}

void RigidBody::applyForce(vec2 f) {
	forces += f;
}

bool RigidBody::checkIntersection(vec2 bl, vec2 tr) {
	vec2 mybl = pos;
	vec2 mytr = pos + hitbox;

	if(mybl.x > tr.x || mytr.x < bl.x) return false;
	if(mybl.y > tr.y || mytr.y < bl.y) return false;

	return true;
}

void RigidBody::update(float time, float dt, World *world) {
	vec2 prev = pos;
	pos += speed * dt;

	collidingTiles.clear();
	const int rx = round(hitbox.x);
	const int ry = round(hitbox.y);
	const ivec2 tile = round(pos);
	if(pos.x > tile.x) {
		for(int y = -ry; y <= ry; y++) {
			if(world->getTileOrEmpty(tile + ivec2(rx, y)).collision() && checkIntersection(tile + ivec2(rx, y), tile + ivec2(rx, y) + ivec2(1, 1))) {
				collidingTiles.push_back(ivec2(rx, y));
			}
		}
	}
	else if(pos.x < tile.x) {
		for(int y = -ry; y <= ry; y++) {
			if(world->getTileOrEmpty(tile + ivec2(-1, y)).collision() && checkIntersection(tile + ivec2(-1, y), tile + ivec2(-1, y) + ivec2(1, 1))) {
				collidingTiles.push_back(ivec2(-1, y));
			}
		}
	}

	if(pos.y > tile.y) {
		for(int x = -rx; x <= rx; x++) {
			if(world->getTileOrEmpty(tile + ivec2(x, ry)).collision() && checkIntersection(tile + ivec2(x, ry), tile + ivec2(x, ry) + ivec2(1, 1))) {
				collidingTiles.push_back(ivec2(x, ry));
			}
		}
	}
	else if(pos.y < tile.y) {
		for(int x = -rx; x <= rx; x++) {
			if(world->getTileOrEmpty(tile + ivec2(x, -1)).collision() && checkIntersection(tile + ivec2(x, -1), tile + ivec2(x, -1) + ivec2(1, 1))) {
				collidingTiles.push_back(ivec2(x, -1));
			}
		}
	}

	collidingTiles.erase(std::unique(collidingTiles.begin(), collidingTiles.end()), collidingTiles.end());
	std::sort(collidingTiles.begin(), collidingTiles.end(), [](ivec2 a, ivec2 b) -> int {
		return length(vec2(a)) < length(vec2(b));
	});

	collision = 0;

	for(ivec2 offset : collidingTiles) {
		if(checkIntersection(tile + offset, tile + offset + ivec2(1, 1))) {
			if(offset.y == -1 && offset.x > -1 && offset.x < rx) {
				speed.y = 0;
				pos.y = ceil(pos.y) + 0.001;
				collision |= bottom;
			}
			else if(offset.y == ry && offset.x > -1 && offset.x < rx) {
				speed.y = 0;
				pos.y = floor(pos.y) - 0.001;
				collision |= top;
			}
			else if(offset.x == -1 && offset.y > -1 && offset.y < ry) {
				speed.x = 0;
				pos.x = ceil(pos.x) + 0.001;
				collision |= left;
			}
			else if(offset.x == rx && offset.y > -1 && offset.y < ry) {
				speed.x = 0;
				pos.x = floor(pos.x) - 0.001;
				collision |= right;
			}
			else if(length(speed) > 0.001) {
				vec2 tmp = pos;
				vec2 minspeed;
				if(abs(speed.x) < abs(speed.y) && abs(speed.x) > 0.001) {
					minspeed = vec2(speed.x, 0);
				}
				else if(abs(speed.y) > 0.001){
					minspeed = vec2(0, speed.y);
				}
				else {
					minspeed = speed;
				}
				for(int i = 0; i < 10 && checkIntersection(tile + offset, tile + offset + ivec2(1, 1)); i++) {
					pos -= minspeed * dt * 0.1;
				}
				vec2 diff = tmp - pos;
				if(length(diff) > 0.1) {
					pos = tmp;
					if(offset.y < 0) {
						speed.y = 0;
						pos.y = ceil(pos.y) + 0.001;
						collision |= bottom;
					}
					else if(offset.y > 0) {
						speed.y = 0;
						pos.y = floor(pos.y) - 0.001;
						collision |= top;
					}
				}
				else if(abs(diff.x) > abs(diff.y)) {
					speed.x = 0;
					if(diff.x < 0) {
						collision |= left;
					}
					else {
						collision |= right;
					}
				}
				else {
					speed.y = 0;
					if(diff.y < 0) {
						collision |= bottom;
					}
					else {
						collision |= top;
					}
				}
			}
		}
	}

	speed += gravity * dt;
	acceleration = forces / mass;
	speed += acceleration * dt;

	if(onGround()) {
		speed.x *= groundFriction;
	}
	else {
		speed.x *= airFriction;
	}

	transform = mat4().translate(pos).scale(hitbox * 0.5).translate(vec3(1,1,0));
	forces = 0;
}

std::vector<ivec2> RigidBody::getCollidingTiles() {
	return collidingTiles;
}

bool RigidBody::onGround() {
	return collision & bottom;
}

vec2 RigidBody::getPos() {
	return pos;
}