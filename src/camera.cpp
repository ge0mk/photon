#include <camera.hpp>

Camera::Camera(vec3 pos, vec3 rot, vec2 res, float fov, float znear, float zfar)
	: pos(pos), rot(rot), res(res), fov(fov), znear(znear), zfar(zfar) {}


void Camera::update(float time, float dt) {
	std::lock_guard<std::mutex> lock(mutex);
	m_proj = mat4::projection(d2r(fov), res.x / res.y, znear, zfar);
	m_view = mat4().translate(pos).rotate(rot).inverse();
}

mat4 Camera::proj() {
	std::lock_guard<std::mutex> lock(mutex);
	return m_proj;
}

mat4 Camera::view() {
	std::lock_guard<std::mutex> lock(mutex);
	return m_view;
}
