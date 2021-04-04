#pragma once

#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

#include "math.hpp"

namespace math{
	class PerlinNoise {
	public:
		PerlinNoise(unsigned int seed = 0);
		double calc(double x, double y, double z = 0.0f) const;

		inline double at(double x, double y, double z = 0.0f) const {
			return calc(x, y, z);
		}

	private:
		static double fade(double t);
		static double grad(int hash, double x, double y, double z);

		std::vector<int> permutation;
	};
}
