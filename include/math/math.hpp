#pragma once

#include <cmath>
#include <cstdlib>

namespace math{
	const float pi = 3.14159;
	const float e = 2.71828;

	inline float d2r(float x){
		return pi/180.0f*x;
	}
	inline float r2d(float x){
		return x/pi*180.0f;
	}

	inline bool equal(float a, float b, float epsilon = 0.00001){
		return fabs(a - b) < epsilon;
	}

	template<typename type>
	inline type fract(type x){
		return x - std::floor(x);
	}

	template<typename type>
	inline type mix(type x, type y, type a){
		return x * (1.0 - a) + y * (a);
	}

	template<typename type>
	type max(type a, type b){
		return  a > b ? a : b;
	}

	template<typename type>
	type min(type a, type b){
		return  a < b ? a : b;
	}

	template<typename type>
	type clamp(type x, type l, type u) {
		return max(min(x, u), l);
	}

	inline float smoothstep(float x, float e0 = 0.0f, float e1 = 1.0f){
		x = clamp((x - e0) / (e1 - e0), 0.0f, 1.0f);
		return x * x * (3.0f - 2.0f * x);
	}

	inline float sigmoid(float x){
		return 1.0f / (1 + exp(-x));
	}

	unsigned int gcd(unsigned int u, unsigned int v);
}
