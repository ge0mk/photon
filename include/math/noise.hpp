#pragma once

#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <memory>
#include <numeric>

#include "math.hpp"
#include "vector.hpp"

namespace math{
	class perlin {
	public:
		perlin(uint64_t seed = 0);
		double noise(double x, double y, double z = 0.0f) const;

		inline double at(double x, double y, double z = 0.0f) const {
			return noise(x, y, z);
		}

	private:
		static double fade(double t);
		static double grad(int hash, double x, double y, double z);

		std::vector<int> permutation;
	};

	// actually Open Simplex Noise
	// see https://gist.github.com/KdotJPG/b1270127455a94ac5d19
	class simplex {
	public:
		simplex(uint64_t seed = 0);

		double noise2d(math::dvec2 pos);
		double noise3d(math::dvec3 pos);
		double noise4d(math::dvec4 pos);

	protected:
		using Grad2 = math::dvec2;
		using Grad3 = math::dvec3;
		using Grad4 = math::dvec4;

		struct LatticePoint2D {
			int xsv, ysv;
			double dx, dy;

			LatticePoint2D() = default;
			LatticePoint2D(const LatticePoint2D &other) = default;

			inline LatticePoint2D(int xsv, int ysv) : xsv(xsv), ysv(ysv) {
				double ssv = double(xsv + ysv) * -0.211324865405187;
				dx = double(-xsv) - ssv;
				dy = double(-ysv) - ssv;
			}

			LatticePoint2D& operator=(const LatticePoint2D &other) = default;
		};
		struct LatticePoint3D {
			double dxr, dyr, dzr;
			int xrv, yrv, zrv;
			std::shared_ptr<LatticePoint3D> nextOnFailure = {}, nextOnSuccess = {};

			LatticePoint3D() = default;
			LatticePoint3D(const LatticePoint3D &other) = default;

			inline LatticePoint3D(int xrv, int yrv, int zrv, int lattice)
			: dxr(-xrv + lattice * 0.5), dyr(-yrv + lattice * 0.5), dzr(-zrv + lattice * 0.5),
			  xrv(xrv + lattice * 1024), yrv(yrv + lattice * 1024), zrv(zrv + lattice * 1024)
			{}

			LatticePoint3D& operator=(const LatticePoint3D &other) = default;
		};

		struct LatticePoint4D {
			int xsv, ysv, zsv, wsv;
			double dx, dy, dz, dw;
			double xsi, ysi, zsi, wsi;
			double ssiDelta;

			LatticePoint4D() = default;
			LatticePoint4D(const LatticePoint4D &other) = default;

			inline LatticePoint4D(int xsv, int ysv, int zsv, int wsv)
			: xsv(xsv + 409), ysv(ysv + 409), zsv(zsv + 409), wsv(wsv + 409),
			  xsi(0.2 - xsv), ysi(0.2 - ysv), zsi(0.2 - zsv), wsi(0.2 - wsv),
			  ssiDelta((0.8 - xsv - ysv - zsv - wsv) * 0.309016994374947)
			{
				double ssv = (xsv + ysv + zsv + wsv) * 0.309016994374947;
				dx = -xsv - ssv, dy = -ysv - ssv, dz = -zsv - ssv, dw = -wsv - ssv;
			}

			LatticePoint4D& operator=(const LatticePoint4D &other) = default;
		};

	private:
		static int fastFloor(double x) {
			int xi = (int)x;
			return x < xi ? xi - 1 : xi;
		}

		double noise2dBase(math::dvec2 pos);
		double noise3dBCC(math::dvec3 pos);
		double noise4dBase(math::dvec4 pos);

		static constexpr int size = 2048;
		static constexpr int mask = 2047;

		static constexpr double N2 = 0.010016341213657120;
		static constexpr double N3 = 0.030485933181293584;
		static constexpr double N4 = 0.009202377986303158;

		std::array<short, size> perm;
		std::array<Grad2, size> permGrad2;
		std::array<Grad3, size> permGrad3;
		std::array<Grad4, size> permGrad4;

		static const std::array<Grad2, size> gradients2D;
		static const std::array<Grad3, size> gradients3D;
		static const std::array<Grad4, size> gradients4D;

		static const std::array<LatticePoint2D, 4> lookup2D;
		static const std::array<std::shared_ptr<LatticePoint3D>, 8> lookup3D;
		static const std::array<LatticePoint4D, 16> vertices4D;
	};
}
