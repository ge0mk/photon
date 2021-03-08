#pragma once

#include <math/matrix.hpp>
#include <math/vector.hpp>

using namespace math;

struct Camera {
	Camera() = default;
	Camera(const Camera &other) = default;
	Camera(vec3 pos, vec3 rot, vec2 res, float fov, float znear = 0.1, float zfar = 100);

	Camera& operator=(const Camera &other) = default;

	void update(float time, float dt);

	vec3 pos, rot;
	float fov, znear, zfar;
	vec2 res;

	mat4 proj, view;
};