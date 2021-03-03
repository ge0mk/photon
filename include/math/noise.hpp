#pragma once

#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

namespace math{
	class perlin {
	public:
		// Initialize with the reference values for the permutation vector
		perlin();
		// Generate a new permutation vector based on the value of seed
		perlin(unsigned int seed);
		// Get a noise value, for 2D images z can have any value
		double noise(double x, double y, double z);
	private:
		double fade(double t);
		double lerp(double t, double a, double b);
		double grad(int hash, double x, double y, double z);

		std::vector<int> p;	// The permutation vector
	};
}
