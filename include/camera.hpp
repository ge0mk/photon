#pragma once

#include <math/matrix.hpp>
#include <math/vector.hpp>

using namespace math;

struct Camera {
	Camera() = default;
	Camera(const Camera &other) = default;
	Camera(vec3 pos, vec3 rot, vec2 res, float fov, float znear = 0.1, float zfar = 100);

	Camera& operator=(const Camera &other) = default;

	void update();

	mat4 proj() const;
	mat4 view() const;

	tvec3<float> pos, rot;
	tvec2<float> res;
	float fov, znear, zfar;

private:
	mat4 m_proj, m_view;
};