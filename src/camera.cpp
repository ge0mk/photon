#include <camera.hpp>

Camera::Camera(vec3 pos, vec3 rot, vec2 res, float fov, float znear, float zfar)
	: pos(pos), rot(rot), res(res), fov(fov), znear(znear), zfar(zfar) {}


void Camera::update(float time, float dt) {
	proj = mat4::projection(d2r(fov), res.x / res.y, znear, zfar);
	view = mat4().translate(pos).rotate(rot).inverse();
}