#pragma once

#include "math.hpp"
#include "vector.hpp"

namespace math {
	template<typename type>
	class tquaternion {	// q = w +xi +yj +zk
	public:
		tquaternion() : w(1), x(0), y(0), z(0) {}
		tquaternion(type w, type x, type y, type z) : w(w), x(x), y(y), z(z) {}
		tquaternion(type angle, tvec3<type> axis) :
			w(cosf(angle/2)),
			x(axis.x * sin(angle/2)),
			y(axis.y * sin(angle/2)),
			z(axis.z * sin(angle/2)) {}
		inline bool normalized() {
			return abs(w*w + x*x + y*y + z*z - 1) < 0.001;
		}
		tquaternion<type>& operator=(const tquaternion<type> &other) {
			w = other.w, x = other.x, y = other.y, z = other.z;
			return *this;
		}
		tquaternion<type> operator*(const tquaternion<type> &other) const {
			return tquaternion(
				w*other.w - x*other.x - y*other.y - z*other.z,
				w*other.x + x*other.w + y*other.z - z*other.y,
				w*other.y - x*other.z + y*other.w + z*other.x,
				w*other.z + x*other.y - y*other.x + z*other.w
			);
		}
		tquaternion<type>& operator*=(const tquaternion<type> &other) {
			return (*this = (*this) * other);
		}
		tquaternion<type>& rotate(const tquaternion<type> &other) {
			return (*this = other * (*this));
		}
		tquaternion<type>& rotate(type w, type x, type y, type z) {
			return (*this = tquaternion<type>(w, x, y, z) * (*this));
		}

		type w, x, y, z;
	};

	template<typename type>
	type length(const tquaternion<type> &q){
		return sqrt(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);
	}
	template<typename type>
	tquaternion<type> normalize(const tquaternion<type> &v){
		return v / length(v);
	}
	template<typename type>
	tquaternion<type> mix(const tquaternion<type> &a, const tquaternion<type> &b, const tquaternion<float> &amnt) {
		return tquaternion<type>(mix(a.x, b.x, amnt.x), mix(a.y, b.y, amnt.y), mix(a.z, b.z, amnt.z), mix(a.w, b.w, amnt.w));
	}

	typedef tquaternion<float> quaternion;
}