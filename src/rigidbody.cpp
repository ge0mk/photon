#include "rigidbody.hpp"

#include <algorithm>

AABB::AABB(vec2 pos, vec2 halfSize) : pos(pos), halfSize(halfSize) {}

bool AABB::intersects(const AABB &other) {
	if(std::abs(pos.x - other.pos.x) > halfSize.x + other.halfSize.x) return false;
	if(std::abs(pos.y - other.pos.y) > halfSize.y + other.halfSize.y) return false;
	return true;
}

RigidBody::RigidBody(std::shared_ptr<TiledTexture> texture) : Entity(texture) {}

void RigidBody::update(float time, float dt, World *world) {
	oldPos = pos;
	oldSpeed = speed;

	mWasOnGround = mOnGround;
	mPushedRightWall = mPushesRightWall;
	mPushedLeftWall = mPushesLeftWall;
	mWasAtCeiling = mAtCeiling;

	pos += speed * dt;
	acceleration = forces / mass;
	speed += (gravity + acceleration) * dt;
	forces = 0.0f;

	float groundY = 0.0f, ceilingY = 0.0f;
	float leftWallX = 0.0f, rightWallX = 0.0f;
	collidingTiles.clear();

	if (speed.x < 0.0f && checkLeft(*world, leftWallX)) {
		if(oldPos.x - halfSize.x + aabbOffset.x >= leftWallX) {
			pos.x = leftWallX + halfSize.x + aabbOffset.x + 0.01f;
			mPushesLeftWall = true;
		}
		speed.x = max(speed.x, 0.0f);
	}
	else {
		mPushesLeftWall = false;
	}

	if (speed.x > 0.0f && checkRight(*world, rightWallX)) {
		if(oldPos.x + halfSize.x + aabbOffset.x <= rightWallX) {
			pos.x = rightWallX - halfSize.x - aabbOffset.x - 0.01f;
			mPushesRightWall = true;
		}
		speed.x = min(speed.x, 0.0f);
	}
	else {
		mPushesRightWall = false;
	}

	if(speed.y <= 0.0f && checkGround(*world, groundY)) {
		pos.y = groundY + halfSize.y - aabbOffset.y;
		speed.y = 0;
		mOnGround = true;
	}
	else {
		mOnGround = false;
	}

	if(speed.y >= 0.0f && checkCeiling(*world, ceilingY)) {
		pos.y = ceilingY - halfSize.y - aabbOffset.y;
		speed.y = 0;
		mAtCeiling = true;
	}
	else {
		mAtCeiling = false;
	}

	if (speed.x == 0.0f && checkLeft(*world, leftWallX)) {
		if(oldPos.x - halfSize.x + aabbOffset.x >= leftWallX) {
			pos.x = leftWallX + halfSize.x + aabbOffset.x + 0.01f;
			mPushesLeftWall = true;
		}
	}
	else {
		mPushesLeftWall = false;
	}

	if (speed.x == 0.0f && checkRight(*world, rightWallX)) {
		if(oldPos.x + halfSize.x + aabbOffset.x <= rightWallX) {
			pos.x = rightWallX - halfSize.x - aabbOffset.x - 0.01f;
			mPushesRightWall = true;
		}
	}
	else {
		mPushesRightWall = false;
	}

	//speed = clamp(speed, -maxspeed, maxspeed);

	rpos = round((pos + aabbOffset) * 2.0f) / 2;
	Entity::pos = rpos;
	AABB::pos = rpos;
	transform = mat4().translate(rpos).scale(vec3(scale, 1.0f));
}

void RigidBody::applyForce(vec2 f) {
	forces += f;
}

void RigidBody::shift(ivec2 dir) {
	pos += vec2(dir) * Chunk::size * Tile::resolution;
	oldPos += vec2(dir) * Chunk::size * Tile::resolution;
	Entity::shift(dir);
}

bool RigidBody::checkGround(const World &world, float &groundY) {
	vec2 center = pos + aabbOffset;
	vec2 oldCenter = oldPos + aabbOffset;

	vec2 oldbl = oldCenter - halfSize + vec2(0.0f, -1.0f);
	vec2 newbl = center - halfSize + vec2(0.0f, -1.0f);
	vec2 newbr = vec2(newbl.x + halfSize.x * 2.0f, newbl.y);

	float endY = std::floor(newbl.y);
	float begY = max(std::ceil(oldbl.y) - 1.0f, endY);
	float dist = max(abs(endY - begY), 1.0f);

	for(float tileY = begY; tileY >= endY; tileY -= 1.0f) {
		vec2 bottomLeft = lerp(newbl, oldbl, abs(endY - tileY) / dist);
		vec2 bottomRight = vec2(bottomLeft.x + halfSize.x * 2.0f + Tile::resolution, bottomLeft.y);

		for(vec2 checkedPixel = newbl; checkedPixel.x <= bottomRight.x; checkedPixel.x += 0.5f * Tile::resolution) {
			checkedPixel.x = min(checkedPixel.x, bottomRight.x);
			ivec2 tileIndex = world.getTileIndex(checkedPixel);
			Tile tile = world.at(tileIndex);

			AABB testaabb = AABB(lerp(pos, oldPos, abs(endY - tileY) / dist) + aabbOffset, halfSize);
			AABB tileaabb = AABB(vec2(tileIndex) * Tile::resolution + 0.5 * Tile::resolution, 0.5 * Tile::resolution);

			if(tile.collision() && tileaabb.intersects(testaabb)) {
				collidingTiles.push_back(tileIndex);
				groundY = tileIndex.y * Tile::resolution + Tile::resolution;
				return true;
			}
		}
	}
	return false;
}

bool RigidBody::checkCeiling(const World &world, float &ceilingY) {
	vec2 center = pos + aabbOffset;
	vec2 oldCenter = oldPos + aabbOffset;

	vec2 oldtr = oldCenter + halfSize + vec2(0.0f, 1.0f);
	vec2 newtr = center + halfSize + vec2(0.0f, 1.0f);
	vec2 newtl = vec2(newtr.x - halfSize.x * 2.0f, newtr.y);

	float endY = std::ceil(newtr.y);
	float begY = max(std::floor(oldtr.y) - 1.0f, endY);
	float dist = max(abs(endY - begY), 1.0f);

	for(float tileY = begY; tileY >= endY; tileY -= 1.0f) {
		vec2 topRight = lerp(newtr, oldtr, abs(endY - tileY) / dist);
		vec2 topLeft = vec2(topRight.x - halfSize.x * 2.0f, topRight.y);

		for(vec2 checkedPixel = topLeft; checkedPixel.x <= topRight.x; checkedPixel.x += 0.5f * Tile::resolution) {
			checkedPixel.x = min(checkedPixel.x, topRight.x);
			ivec2 tileIndex = world.getTileIndex(checkedPixel);
			Tile tile = world.at(tileIndex);

			AABB testaabb = AABB(lerp(pos, oldPos, abs(endY - tileY) / dist) + aabbOffset, halfSize);
			AABB tileaabb = AABB(vec2(tileIndex) * Tile::resolution + 0.5 * Tile::resolution, 0.5 * Tile::resolution);

			if(tile.collision() && tileaabb.intersects(testaabb)) {
				collidingTiles.push_back(tileIndex);
				ceilingY = tileIndex.y * Tile::resolution;
				return true;
			}
		}
	}
	return false;
}

bool RigidBody::checkLeft(const World &world, float &leftX) {
	vec2 center = pos + aabbOffset;
	vec2 oldCenter = oldPos + aabbOffset;

	vec2 oldbl = oldCenter - halfSize + vec2(-1.0f, 1.0f);
	vec2 newbl = center - halfSize + vec2(-1.0f, 1.0f);
	vec2 newtl = vec2(newbl.x, newbl.y + halfSize.y * 2.0f - 2.0f);

	float endX = std::floor(newbl.x);
	float begX = max(std::ceil(oldbl.x) - 1.0f, endX);
	float dist = max(abs(endX - begX), 1.0f);

	for(float tileX = begX; tileX >= endX; tileX -= 1.0f) {
		vec2 bottomLeft = lerp(newbl, oldbl, abs(endX - tileX) / dist);
		vec2 topLeft = vec2(bottomLeft.x, bottomLeft.y + halfSize.y * 2.0f - 2.0f);

		for(vec2 checkedPixel = bottomLeft; checkedPixel.y <= topLeft.y; checkedPixel.y += 0.5f * Tile::resolution) {
			checkedPixel.y = min(checkedPixel.y, topLeft.y);
			ivec2 tileIndex = world.getTileIndex(checkedPixel);
			Tile tile = world.at(tileIndex);

			AABB testaabb = AABB(lerp(pos, oldPos, abs(endX - tileX) / dist) + aabbOffset, halfSize);
			AABB tileaabb = AABB(vec2(tileIndex) * Tile::resolution + 0.5 * Tile::resolution, 0.5 * Tile::resolution);

			if(tile.collision() && tileaabb.intersects(testaabb)) {
				collidingTiles.push_back(tileIndex);
				leftX = tileIndex.x * Tile::resolution - Tile::resolution;
				return true;
			}
		}
	}
	return false;
}

bool RigidBody::checkRight(const World &world, float &rightX) {
	vec2 center = pos + aabbOffset;
	vec2 oldCenter = oldPos + aabbOffset;

	vec2 oldbr = oldCenter + vec2(halfSize.x, -halfSize.y) + vec2(1.0f, 1.0f);
	vec2 newbr = center + vec2(halfSize.x, -halfSize.y) + vec2(1.0f, 1.0f);
	vec2 newtr = vec2(newbr.x, newbr.y + halfSize.y * 2.0f - 2.0f);

	float endX = std::ceil(newbr.x);
	float begX = max(std::floor(oldbr.x) - 1.0f, endX);
	float dist = max(abs(endX - begX), 1.0f);

	for(float tileX = begX; tileX >= endX; tileX -= 1.0f) {
		vec2 bottomRight = lerp(newbr, oldbr, abs(endX - tileX) / dist);
		vec2 topRight = vec2(bottomRight.x, bottomRight.y + halfSize.y * 2.0f - 2.0f);

		for(vec2 checkedPixel = bottomRight; checkedPixel.y <= topRight.y; checkedPixel.y += 0.5f * Tile::resolution) {
			checkedPixel.y = min(checkedPixel.y, topRight.y);
			ivec2 tileIndex = world.getTileIndex(checkedPixel);
			Tile tile = world.at(tileIndex);

			AABB testaabb = AABB(lerp(pos, oldPos, abs(endX - tileX) / dist) + aabbOffset, halfSize);
			AABB tileaabb = AABB(vec2(tileIndex) * Tile::resolution + 0.5 * Tile::resolution, 0.5 * Tile::resolution);

			if(tile.collision() && tileaabb.intersects(testaabb)) {
				collidingTiles.push_back(tileIndex);
				rightX = tileIndex.x * Tile::resolution;
				return true;
			}
		}
	}
	return false;
}

const std::vector<ivec2>& RigidBody::getCollidingTiles() const {
	return collidingTiles;
}

vec2 RigidBody::getPos() const {
	return pos;
}

vec2 RigidBody::getSpeed() const {
	return speed;
}