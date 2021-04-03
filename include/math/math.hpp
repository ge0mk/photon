#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdlib>

namespace math{
	const float pi = 3.14159;
	const float e = 2.71828;

	template<typename type>
	inline type fract(type x){
		return x - std::floor(x);
	}

	template<typename type>
	inline type mix(type x, type y, type a){
		return x * (1.0 - a) + y * (a);
	}

	inline float d2r(float x){
		return pi/180.0f*x;
	}

	inline float r2d(float x){
		return x/pi*180.0f;
	}

	inline bool equal(float a, float b, float epsilon = 0.00001){
		return fabs(a - b) < epsilon;
	}

	inline float rand(float min, float max) {
		return (float(::rand()) / float(RAND_MAX)) * (max - min) + min;
	}

	inline double rand(double min, double max) {
		return (double(::rand()) / double(RAND_MAX)) * (max - min) + min;
	}

	inline float smoothstep(float x, float e0 = 0.0f, float e1 = 1.0f) {
		x = std::clamp((x - e0) / (e1 - e0), 0.0f, 1.0f);
		return x * x * (3.0f - 2.0f * x);
	}

	inline double smoothstep(double x, double e0 = 0.0, double e1 = 1.0) {
		x = std::clamp((x - e0) / (e1 - e0), 0.0, 1.0);
		return x * x * (3.0 - 2.0 * x);
	}

	inline float sigmoid(float x) {
		return 1.0f / (1.0f + exp(-x));
	}

	inline double sigmoid(double x) {
		return 1.0 / (1.0 + exp(-x));
	}

	unsigned int gcd(unsigned int u, unsigned int v);
}
