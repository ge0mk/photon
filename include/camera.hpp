#pragma once

#include <math/matrix.hpp>
#include <math/vector.hpp>

#include <atomic>
#include <mutex>

using namespace math;

struct Camera {
	Camera() = default;
	Camera(const Camera &other) = delete;
	Camera(vec3 pos, vec3 rot, vec2 res, float fov, float znear = 0.1, float zfar = 100);

	Camera& operator=(const Camera &other) = delete;

	void update(float time, float dt);

	mat4 proj();
	mat4 view();

	tvec3<std::atomic<float>> pos, rot;
	tvec2<std::atomic<float>> res;
	std::atomic<float> fov, znear, zfar;

private:
	std::mutex mutex;

	mat4 m_proj, m_view;
};