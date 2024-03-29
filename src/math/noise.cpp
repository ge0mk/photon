#include <math/noise.hpp>

namespace math{
	static const std::vector<int> permutation_base = {
			151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
			8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
			35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
			134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
			55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
			18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
			250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
			189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,
			43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,
			97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
			107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
			138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	// Generate a new permutation vector based on the value of seed
	perlin::perlin(uint64_t seed) : permutation(permutation_base) {
		std::default_random_engine rng(seed);
		std::shuffle(permutation.begin(), permutation.end(), rng);

		// Duplicate the permutation vector
		permutation.insert(permutation.end(), permutation.begin(), permutation.end());
	}

	double perlin::noise(double x, double y, double z) const {
		// Find the unit cube that contains the point
		int X = (int) ::floor(x) & 255;
		int Y = (int) ::floor(y) & 255;
		int Z = (int) ::floor(z) & 255;

		// Find relative x, y,z of point in cube
		x -= ::floor(x);
		y -= ::floor(y);
		z -= ::floor(z);

		// Compute fade curves for each of x, y, z
		double u = fade(x);
		double v = fade(y);
		double w = fade(z);

		// Hash coordinates of the 8 cube corners
		int A = permutation[X] + Y;
		int AA = permutation[A] + Z;
		int AB = permutation[A + 1] + Z;
		int B = permutation[X + 1] + Y;
		int BA = permutation[B] + Z;
		int BB = permutation[B + 1] + Z;

		// Add blended results from 8 corners of cube
		double res = lerp(w, lerp(v, lerp(u, grad(permutation[AA], x, y, z), grad(permutation[BA], x-1, y, z)), lerp(u, grad(permutation[AB], x, y-1, z), grad(permutation[BB], x-1, y-1, z))),	lerp(v, lerp(u, grad(permutation[AA+1], x, y, z-1), grad(permutation[BA+1], x-1, y, z-1)), lerp(u, grad(permutation[AB+1], x, y-1, z-1),	grad(permutation[BB+1], x-1, y-1, z-1))));
		return (res + 1.0)/2.0;
	}

	double perlin::fade(double t) {
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	double perlin::grad(int hash, double x, double y, double z) {
		int h = hash & 15;
		// Convert lower 4 bits of hash into 12 gradient directions
		double u = h < 8 ? x : y,
			   v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

	simplex::simplex(uint64_t seed) {
		std::array<short, size> source;
		for (short i = 0; i < size; i++) {
			source[i] = i;
		}
		for (int i = size - 1; i >= 0; i--) {
			seed = seed * 6364136223846793005L + 1442695040888963407L;
			int r = (int)((seed + 31) % (i + 1));
			if (r < 0) {
				r += (i + 1);
			}
			perm[i] = source[r];
			permGrad2[i] = gradients2D[perm[i]];
			permGrad3[i] = gradients3D[perm[i]];
			permGrad4[i] = gradients4D[perm[i]];
			source[r] = source[i];
		}
	}

	double simplex::noise2d(math::dvec2 pos) {
		// Get points for A2* lattice
		double s = 0.366025403784439 * (pos.x + pos.y);
		dvec2 ps = pos + s;
		return noise2dBase(ps);
	}

	double simplex::noise3d(math::dvec3 pos) {
		// Re-orient the cubic lattices via rotation, to produce the expected look on cardinal planar slices.
		// If texturing objects that don't tend to have cardinal plane faces, you could even remove this.
		// Orthonormal rotation. Not a skew transform.
		double r = (2.0 / 3.0) * (pos.x + pos.y + pos.z);
		dvec2 posr = dvec2(r) - pos;

		// Evaluate both lattices to form a BCC lattice.
		return noise3dBCC(posr);
	}

	double simplex::noise4d(math::dvec4 pos) {
		// Get points for A4 lattice
		double s = -0.138196601125011 * (pos.x + pos.y + pos.z + pos.w);
		math::dvec4 spos = pos + math::dvec4(s);
		return noise4dBase(spos);
	}

	double simplex::noise2dBase(math::dvec2 pos) {
		double value = 0;

		// Get base points and offsets
		int xsb = fastFloor(pos.x), ysb = fastFloor(pos.y);
		double xsi = pos.x - xsb, ysi = pos.y - ysb;

		// Index to point list
		int index = (int)((ysi - xsi) / 2 + 1);

		double ssi = (xsi + ysi) * -0.211324865405187;
		double xi = xsi + ssi, yi = ysi + ssi;

		// Point contributions
		for (int i = 0; i < 3; i++) {
			LatticePoint2D c = lookup2D[index + i];

			double dx = xi + c.dx, dy = yi + c.dy;
			double attn = 0.5 - dx * dx - dy * dy;
			if (attn <= 0) continue;

			int pxm = (xsb + c.xsv) & mask, pym = (ysb + c.ysv) & mask;
			Grad2 grad = permGrad2[perm[pxm] ^ pym];
			double extrapolation = grad.x * dx + grad.y * dy;

			attn *= attn;
			value += attn * attn * extrapolation;
		}
		return value;
	}

	double simplex::noise3dBCC(math::dvec3 pos) {
		// Get base and offsets inside cube of first lattice.
		int xrb = fastFloor(pos.x), yrb = fastFloor(pos.y), zrb = fastFloor(pos.z);
		double xri = pos.x - xrb, yri = pos.y - yrb, zri = pos.z - zrb;

		// Identify which octant of the cube we're in. This determines which cell
		// in the other cubic lattice we're in, and also narrows down one point on each.
		int xht = (int)(xri + 0.5), yht = (int)(yri + 0.5), zht = (int)(zri + 0.5);
		int index = (xht << 0) | (yht << 1) | (zht << 2);

		// Point contributions
		double value = 0;
		std::shared_ptr<LatticePoint3D> c = lookup3D[index];
		while (c) {
			double dxr = xri + c->dxr, dyr = yri + c->dyr, dzr = zri + c->dzr;
			double attn = 0.5 - dxr * dxr - dyr * dyr - dzr * dzr;
			if (attn < 0) {
				c = c->nextOnFailure;
			} else {
				int pxm = (xrb + c->xrv) & mask, pym = (yrb + c->yrv) & mask, pzm = (zrb + c->zrv) & mask;
				Grad3 grad = permGrad3[perm[perm[pxm] ^ pym] ^ pzm];
				double extrapolation = grad.x * dxr + grad.y * dyr + grad.z * dzr;

				attn *= attn;
				value += attn * attn * extrapolation;
				c = c->nextOnSuccess;
			}
		}
		return value;
	}

	double simplex::noise4dBase(math::dvec4 pos) {
		double value = 0;

		// Get base points and offsets
		int xsb = fastFloor(pos.x), ysb = fastFloor(pos.y), zsb = fastFloor(pos.z), wsb = fastFloor(pos.w);
		double xsi = pos.x - xsb, ysi = pos.y - ysb, zsi = pos.z - zsb, wsi = pos.w - wsb;

		// If we're in the lower half, flip so we can repeat the code for the upper half. We'll flip back later.
		double siSum = xsi + ysi + zsi + wsi;
		double ssi = siSum * 0.309016994374947; // Prep for vertex contributions.
		bool inLowerHalf = (siSum < 2);
		if (inLowerHalf) {
			xsi = 1 - xsi; ysi = 1 - ysi; zsi = 1 - zsi; wsi = 1 - wsi;
			siSum = 4 - siSum;
		}

		// Consider opposing vertex pairs of the octahedron formed by the central cross-section of the stretched tesseract
		double aabb = xsi + ysi - zsi - wsi, abab = xsi - ysi + zsi - wsi, abba = xsi - ysi - zsi + wsi;
		double aabbScore = std::fabs(aabb), ababScore = std::fabs(abab), abbaScore = std::fabs(abba);

		// Find the closest point on the stretched tesseract as if it were the upper half
		int vertexIndex, via, vib;
		double asi, bsi;
		if (aabbScore > ababScore && aabbScore > abbaScore) {
			if (aabb > 0) {
				asi = zsi; bsi = wsi; vertexIndex = 0b0011; via = 0b0111; vib = 0b1011;
			} else {
				asi = xsi; bsi = ysi; vertexIndex = 0b1100; via = 0b1101; vib = 0b1110;
			}
		} else if (ababScore > abbaScore) {
			if (abab > 0) {
				asi = ysi; bsi = wsi; vertexIndex = 0b0101; via = 0b0111; vib = 0b1101;
			} else {
				asi = xsi; bsi = zsi; vertexIndex = 0b1010; via = 0b1011; vib = 0b1110;
			}
		} else {
			if (abba > 0) {
				asi = ysi; bsi = zsi; vertexIndex = 0b1001; via = 0b1011; vib = 0b1101;
			} else {
				asi = xsi; bsi = wsi; vertexIndex = 0b0110; via = 0b0111; vib = 0b1110;
			}
		}
		if (bsi > asi) {
			via = vib;
			double temp = bsi;
			bsi = asi;
			asi = temp;
		}
		if (siSum + asi > 3) {
			vertexIndex = via;
			if (siSum + bsi > 4) {
				vertexIndex = 0b1111;
			}
		}

		// Now flip back if we're actually in the lower half.
		if (inLowerHalf) {
			xsi = 1 - xsi; ysi = 1 - ysi; zsi = 1 - zsi; wsi = 1 - wsi;
			vertexIndex ^= 0b1111;
		}

		// Five points to add, total, from five copies of the A4 lattice.
		for (int i = 0; i < 5; i++) {

			// Update xsb/etc. and add the lattice point's contribution.
			LatticePoint4D c = vertices4D[vertexIndex];
			xsb += c.xsv; ysb += c.ysv; zsb += c.zsv; wsb += c.wsv;
			double xi = xsi + ssi, yi = ysi + ssi, zi = zsi + ssi, wi = wsi + ssi;
			double dx = xi + c.dx, dy = yi + c.dy, dz = zi + c.dz, dw = wi + c.dw;
			double attn = 0.5 - dx * dx - dy * dy - dz * dz - dw * dw;
			if (attn > 0) {
				int pxm = xsb & mask, pym = ysb & mask, pzm = zsb & mask, pwm = wsb & mask;
				Grad4 grad = permGrad4[perm[perm[perm[pxm] ^ pym] ^ pzm] ^ pwm];
				double ramped = grad.x * dx + grad.y * dy + grad.z * dz + grad.w * dw;

				attn *= attn;
				value += attn * attn * ramped;
			}

			// Maybe this helps the compiler/JVM/LLVM/etc. know we can end the loop here. Maybe not.
			if (i == 4) break;

			// Update the relative skewed coordinates to reference the vertex we just added.
			// Rather, reference its counterpart on the lattice copy that is shifted down by
			// the vector <-0.2, -0.2, -0.2, -0.2>
			xsi += c.xsi; ysi += c.ysi; zsi += c.zsi; wsi += c.wsi;
			ssi += c.ssiDelta;

			// Next point is the closest vertex on the 4-simplex whose base vertex is the aforementioned vertex.
			double score0 = 1.0 + ssi * (-1.0 / 0.309016994374947); // Seems slightly faster than 1.0-xsi-ysi-zsi-wsi
			vertexIndex = 0b0000;
			if (xsi >= ysi && xsi >= zsi && xsi >= wsi && xsi >= score0) {
				vertexIndex = 0b0001;
			}
			else if (ysi > xsi && ysi >= zsi && ysi >= wsi && ysi >= score0) {
				vertexIndex = 0b0010;
			}
			else if (zsi > xsi && zsi > ysi && zsi >= wsi && zsi >= score0) {
				vertexIndex = 0b0100;
			}
			else if (wsi > xsi && wsi > ysi && wsi > zsi && wsi >= score0) {
				vertexIndex = 0b1000;
			}
		}

		return value;
	}

	const std::array<simplex::Grad2, simplex::size> simplex::gradients2D = []() -> std::array<simplex::Grad2, simplex::size> {
		std::array<simplex::Grad2, simplex::size> result;
		std::array<simplex::Grad2, 24> grad2 = {
			Grad2( 0.130526192220052,  0.99144486137381),
			Grad2( 0.38268343236509,   0.923879532511287),
			Grad2( 0.608761429008721,  0.793353340291235),
			Grad2( 0.793353340291235,  0.608761429008721),
			Grad2( 0.923879532511287,  0.38268343236509),
			Grad2( 0.99144486137381,   0.130526192220051),
			Grad2( 0.99144486137381,  -0.130526192220051),
			Grad2( 0.923879532511287, -0.38268343236509),
			Grad2( 0.793353340291235, -0.60876142900872),
			Grad2( 0.608761429008721, -0.793353340291235),
			Grad2( 0.38268343236509,  -0.923879532511287),
			Grad2( 0.130526192220052, -0.99144486137381),
			Grad2(-0.130526192220052, -0.99144486137381),
			Grad2(-0.38268343236509,  -0.923879532511287),
			Grad2(-0.608761429008721, -0.793353340291235),
			Grad2(-0.793353340291235, -0.608761429008721),
			Grad2(-0.923879532511287, -0.38268343236509),
			Grad2(-0.99144486137381,  -0.130526192220052),
			Grad2(-0.99144486137381,   0.130526192220051),
			Grad2(-0.923879532511287,  0.38268343236509),
			Grad2(-0.793353340291235,  0.608761429008721),
			Grad2(-0.608761429008721,  0.793353340291235),
			Grad2(-0.38268343236509,   0.923879532511287),
			Grad2(-0.130526192220052,  0.99144486137381)
		};
		for (unsigned i = 0; i < grad2.size(); i++) {
			grad2[i].x /= N2; grad2[i].y /= N2;
		}
		for (unsigned i = 0; i < simplex::size; i++) {
			result[i] = grad2[i % grad2.size()];
		}
		return result;
	}();

	const std::array<simplex::Grad3, simplex::size> simplex::gradients3D = []() -> std::array<simplex::Grad3, simplex::size> {
		std::array<simplex::Grad3, simplex::size> result;
		std::array<simplex::Grad3, 48> grad3 = {
			Grad3(-2.22474487139,      -2.22474487139,      -1.0),
			Grad3(-2.22474487139,      -2.22474487139,       1.0),
			Grad3(-3.0862664687972017, -1.1721513422464978,  0.0),
			Grad3(-1.1721513422464978, -3.0862664687972017,  0.0),
			Grad3(-2.22474487139,      -1.0,                -2.22474487139),
			Grad3(-2.22474487139,       1.0,                -2.22474487139),
			Grad3(-1.1721513422464978,  0.0,                -3.0862664687972017),
			Grad3(-3.0862664687972017,  0.0,                -1.1721513422464978),
			Grad3(-2.22474487139,      -1.0,                 2.22474487139),
			Grad3(-2.22474487139,       1.0,                 2.22474487139),
			Grad3(-3.0862664687972017,  0.0,                 1.1721513422464978),
			Grad3(-1.1721513422464978,  0.0,                 3.0862664687972017),
			Grad3(-2.22474487139,       2.22474487139,      -1.0),
			Grad3(-2.22474487139,       2.22474487139,       1.0),
			Grad3(-1.1721513422464978,  3.0862664687972017,  0.0),
			Grad3(-3.0862664687972017,  1.1721513422464978,  0.0),
			Grad3(-1.0,                -2.22474487139,      -2.22474487139),
			Grad3( 1.0,                -2.22474487139,      -2.22474487139),
			Grad3( 0.0,                -3.0862664687972017, -1.1721513422464978),
			Grad3( 0.0,                -1.1721513422464978, -3.0862664687972017),
			Grad3(-1.0,                -2.22474487139,       2.22474487139),
			Grad3( 1.0,                -2.22474487139,       2.22474487139),
			Grad3( 0.0,                -1.1721513422464978,  3.0862664687972017),
			Grad3( 0.0,                -3.0862664687972017,  1.1721513422464978),
			Grad3(-1.0,                 2.22474487139,      -2.22474487139),
			Grad3( 1.0,                 2.22474487139,      -2.22474487139),
			Grad3( 0.0,                 1.1721513422464978, -3.0862664687972017),
			Grad3( 0.0,                 3.0862664687972017, -1.1721513422464978),
			Grad3(-1.0,                 2.22474487139,       2.22474487139),
			Grad3( 1.0,                 2.22474487139,       2.22474487139),
			Grad3( 0.0,                 3.0862664687972017,  1.1721513422464978),
			Grad3( 0.0,                 1.1721513422464978,  3.0862664687972017),
			Grad3( 2.22474487139,      -2.22474487139,      -1.0),
			Grad3( 2.22474487139,      -2.22474487139,       1.0),
			Grad3( 1.1721513422464978, -3.0862664687972017,  0.0),
			Grad3( 3.0862664687972017, -1.1721513422464978,  0.0),
			Grad3( 2.22474487139,      -1.0,                -2.22474487139),
			Grad3( 2.22474487139,       1.0,                -2.22474487139),
			Grad3( 3.0862664687972017,  0.0,                -1.1721513422464978),
			Grad3( 1.1721513422464978,  0.0,                -3.0862664687972017),
			Grad3( 2.22474487139,      -1.0,                 2.22474487139),
			Grad3( 2.22474487139,       1.0,                 2.22474487139),
			Grad3( 1.1721513422464978,  0.0,                 3.0862664687972017),
			Grad3( 3.0862664687972017,  0.0,                 1.1721513422464978),
			Grad3( 2.22474487139,       2.22474487139,      -1.0),
			Grad3( 2.22474487139,       2.22474487139,       1.0),
			Grad3( 3.0862664687972017,  1.1721513422464978,  0.0),
			Grad3( 1.1721513422464978,  3.0862664687972017,  0.0)
		};
		for (unsigned i = 0; i < grad3.size(); i++) {
			grad3[i].x /= N3; grad3[i].y /= N3; grad3[i].z /= N3;
		}
		for (unsigned i = 0; i < simplex::size; i++) {
			result[i] = grad3[i % grad3.size()];
		}
		return result;
	}();

	const std::array<simplex::Grad4, simplex::size> simplex::gradients4D = []() -> std::array<simplex::Grad4, simplex::size> {
		std::array<simplex::Grad4, simplex::size> result;
		std::array<simplex::Grad4, 160> grad4 = {
			Grad4(-0.753341017856078,    -0.37968289875261624,  -0.37968289875261624,  -0.37968289875261624),
			Grad4(-0.7821684431180708,   -0.4321472685365301,   -0.4321472685365301,    0.12128480194602098),
			Grad4(-0.7821684431180708,   -0.4321472685365301,    0.12128480194602098,  -0.4321472685365301),
			Grad4(-0.7821684431180708,    0.12128480194602098,  -0.4321472685365301,   -0.4321472685365301),
			Grad4(-0.8586508742123365,   -0.508629699630796,     0.044802370851755174,  0.044802370851755174),
			Grad4(-0.8586508742123365,    0.044802370851755174, -0.508629699630796,     0.044802370851755174),
			Grad4(-0.8586508742123365,    0.044802370851755174,  0.044802370851755174, -0.508629699630796),
			Grad4(-0.9982828964265062,   -0.03381941603233842,  -0.03381941603233842,  -0.03381941603233842),
			Grad4(-0.37968289875261624,  -0.753341017856078,    -0.37968289875261624,  -0.37968289875261624),
			Grad4(-0.4321472685365301,   -0.7821684431180708,   -0.4321472685365301,    0.12128480194602098),
			Grad4(-0.4321472685365301,   -0.7821684431180708,    0.12128480194602098,  -0.4321472685365301),
			Grad4( 0.12128480194602098,  -0.7821684431180708,   -0.4321472685365301,   -0.4321472685365301),
			Grad4(-0.508629699630796,    -0.8586508742123365,    0.044802370851755174,  0.044802370851755174),
			Grad4( 0.044802370851755174, -0.8586508742123365,   -0.508629699630796,     0.044802370851755174),
			Grad4( 0.044802370851755174, -0.8586508742123365,    0.044802370851755174, -0.508629699630796),
			Grad4(-0.03381941603233842,  -0.9982828964265062,   -0.03381941603233842,  -0.03381941603233842),
			Grad4(-0.37968289875261624,  -0.37968289875261624,  -0.753341017856078,    -0.37968289875261624),
			Grad4(-0.4321472685365301,   -0.4321472685365301,   -0.7821684431180708,    0.12128480194602098),
			Grad4(-0.4321472685365301,    0.12128480194602098,  -0.7821684431180708,   -0.4321472685365301),
			Grad4( 0.12128480194602098,  -0.4321472685365301,   -0.7821684431180708,   -0.4321472685365301),
			Grad4(-0.508629699630796,     0.044802370851755174, -0.8586508742123365,    0.044802370851755174),
			Grad4( 0.044802370851755174, -0.508629699630796,    -0.8586508742123365,    0.044802370851755174),
			Grad4( 0.044802370851755174,  0.044802370851755174, -0.8586508742123365,   -0.508629699630796),
			Grad4(-0.03381941603233842,  -0.03381941603233842,  -0.9982828964265062,   -0.03381941603233842),
			Grad4(-0.37968289875261624,  -0.37968289875261624,  -0.37968289875261624,  -0.753341017856078),
			Grad4(-0.4321472685365301,   -0.4321472685365301,    0.12128480194602098,  -0.7821684431180708),
			Grad4(-0.4321472685365301,    0.12128480194602098,  -0.4321472685365301,   -0.7821684431180708),
			Grad4( 0.12128480194602098,  -0.4321472685365301,   -0.4321472685365301,   -0.7821684431180708),
			Grad4(-0.508629699630796,     0.044802370851755174,  0.044802370851755174, -0.8586508742123365),
			Grad4( 0.044802370851755174, -0.508629699630796,     0.044802370851755174, -0.8586508742123365),
			Grad4( 0.044802370851755174,  0.044802370851755174, -0.508629699630796,    -0.8586508742123365),
			Grad4(-0.03381941603233842,  -0.03381941603233842,  -0.03381941603233842,  -0.9982828964265062),
			Grad4(-0.6740059517812944,   -0.3239847771997537,   -0.3239847771997537,    0.5794684678643381),
			Grad4(-0.7504883828755602,   -0.4004672082940195,    0.15296486218853164,   0.5029860367700724),
			Grad4(-0.7504883828755602,    0.15296486218853164,  -0.4004672082940195,    0.5029860367700724),
			Grad4(-0.8828161875373585,    0.08164729285680945,   0.08164729285680945,   0.4553054119602712),
			Grad4(-0.4553054119602712,   -0.08164729285680945,  -0.08164729285680945,   0.8828161875373585),
			Grad4(-0.5029860367700724,   -0.15296486218853164,   0.4004672082940195,    0.7504883828755602),
			Grad4(-0.5029860367700724,    0.4004672082940195,   -0.15296486218853164,   0.7504883828755602),
			Grad4(-0.5794684678643381,    0.3239847771997537,    0.3239847771997537,    0.6740059517812944),
			Grad4(-0.3239847771997537,   -0.6740059517812944,   -0.3239847771997537,    0.5794684678643381),
			Grad4(-0.4004672082940195,   -0.7504883828755602,    0.15296486218853164,   0.5029860367700724),
			Grad4( 0.15296486218853164,  -0.7504883828755602,   -0.4004672082940195,    0.5029860367700724),
			Grad4( 0.08164729285680945,  -0.8828161875373585,    0.08164729285680945,   0.4553054119602712),
			Grad4(-0.08164729285680945,  -0.4553054119602712,   -0.08164729285680945,   0.8828161875373585),
			Grad4(-0.15296486218853164,  -0.5029860367700724,    0.4004672082940195,    0.7504883828755602),
			Grad4( 0.4004672082940195,   -0.5029860367700724,   -0.15296486218853164,   0.7504883828755602),
			Grad4( 0.3239847771997537,   -0.5794684678643381,    0.3239847771997537,    0.6740059517812944),
			Grad4(-0.3239847771997537,   -0.3239847771997537,   -0.6740059517812944,    0.5794684678643381),
			Grad4(-0.4004672082940195,    0.15296486218853164,  -0.7504883828755602,    0.5029860367700724),
			Grad4( 0.15296486218853164,  -0.4004672082940195,   -0.7504883828755602,    0.5029860367700724),
			Grad4( 0.08164729285680945,   0.08164729285680945,  -0.8828161875373585,    0.4553054119602712),
			Grad4(-0.08164729285680945,  -0.08164729285680945,  -0.4553054119602712,    0.8828161875373585),
			Grad4(-0.15296486218853164,   0.4004672082940195,   -0.5029860367700724,    0.7504883828755602),
			Grad4( 0.4004672082940195,   -0.15296486218853164,  -0.5029860367700724,    0.7504883828755602),
			Grad4( 0.3239847771997537,    0.3239847771997537,   -0.5794684678643381,    0.6740059517812944),
			Grad4(-0.6740059517812944,   -0.3239847771997537,    0.5794684678643381,   -0.3239847771997537),
			Grad4(-0.7504883828755602,   -0.4004672082940195,    0.5029860367700724,    0.15296486218853164),
			Grad4(-0.7504883828755602,    0.15296486218853164,   0.5029860367700724,   -0.4004672082940195),
			Grad4(-0.8828161875373585,    0.08164729285680945,   0.4553054119602712,    0.08164729285680945),
			Grad4(-0.4553054119602712,   -0.08164729285680945,   0.8828161875373585,   -0.08164729285680945),
			Grad4(-0.5029860367700724,   -0.15296486218853164,   0.7504883828755602,    0.4004672082940195),
			Grad4(-0.5029860367700724,    0.4004672082940195,    0.7504883828755602,   -0.15296486218853164),
			Grad4(-0.5794684678643381,    0.3239847771997537,    0.6740059517812944,    0.3239847771997537),
			Grad4(-0.3239847771997537,   -0.6740059517812944,    0.5794684678643381,   -0.3239847771997537),
			Grad4(-0.4004672082940195,   -0.7504883828755602,    0.5029860367700724,    0.15296486218853164),
			Grad4( 0.15296486218853164,  -0.7504883828755602,    0.5029860367700724,   -0.4004672082940195),
			Grad4( 0.08164729285680945,  -0.8828161875373585,    0.4553054119602712,    0.08164729285680945),
			Grad4(-0.08164729285680945,  -0.4553054119602712,    0.8828161875373585,   -0.08164729285680945),
			Grad4(-0.15296486218853164,  -0.5029860367700724,    0.7504883828755602,    0.4004672082940195),
			Grad4( 0.4004672082940195,   -0.5029860367700724,    0.7504883828755602,   -0.15296486218853164),
			Grad4( 0.3239847771997537,   -0.5794684678643381,    0.6740059517812944,    0.3239847771997537),
			Grad4(-0.3239847771997537,   -0.3239847771997537,    0.5794684678643381,   -0.6740059517812944),
			Grad4(-0.4004672082940195,    0.15296486218853164,   0.5029860367700724,   -0.7504883828755602),
			Grad4( 0.15296486218853164,  -0.4004672082940195,    0.5029860367700724,   -0.7504883828755602),
			Grad4( 0.08164729285680945,   0.08164729285680945,   0.4553054119602712,   -0.8828161875373585),
			Grad4(-0.08164729285680945,  -0.08164729285680945,   0.8828161875373585,   -0.4553054119602712),
			Grad4(-0.15296486218853164,   0.4004672082940195,    0.7504883828755602,   -0.5029860367700724),
			Grad4( 0.4004672082940195,   -0.15296486218853164,   0.7504883828755602,   -0.5029860367700724),
			Grad4( 0.3239847771997537,    0.3239847771997537,    0.6740059517812944,   -0.5794684678643381),
			Grad4(-0.6740059517812944,    0.5794684678643381,   -0.3239847771997537,   -0.3239847771997537),
			Grad4(-0.7504883828755602,    0.5029860367700724,   -0.4004672082940195,    0.15296486218853164),
			Grad4(-0.7504883828755602,    0.5029860367700724,    0.15296486218853164,  -0.4004672082940195),
			Grad4(-0.8828161875373585,    0.4553054119602712,    0.08164729285680945,   0.08164729285680945),
			Grad4(-0.4553054119602712,    0.8828161875373585,   -0.08164729285680945,  -0.08164729285680945),
			Grad4(-0.5029860367700724,    0.7504883828755602,   -0.15296486218853164,   0.4004672082940195),
			Grad4(-0.5029860367700724,    0.7504883828755602,    0.4004672082940195,   -0.15296486218853164),
			Grad4(-0.5794684678643381,    0.6740059517812944,    0.3239847771997537,    0.3239847771997537),
			Grad4(-0.3239847771997537,    0.5794684678643381,   -0.6740059517812944,   -0.3239847771997537),
			Grad4(-0.4004672082940195,    0.5029860367700724,   -0.7504883828755602,    0.15296486218853164),
			Grad4( 0.15296486218853164,   0.5029860367700724,   -0.7504883828755602,   -0.4004672082940195),
			Grad4( 0.08164729285680945,   0.4553054119602712,   -0.8828161875373585,    0.08164729285680945),
			Grad4(-0.08164729285680945,   0.8828161875373585,   -0.4553054119602712,   -0.08164729285680945),
			Grad4(-0.15296486218853164,   0.7504883828755602,   -0.5029860367700724,    0.4004672082940195),
			Grad4( 0.4004672082940195,    0.7504883828755602,   -0.5029860367700724,   -0.15296486218853164),
			Grad4( 0.3239847771997537,    0.6740059517812944,   -0.5794684678643381,    0.3239847771997537),
			Grad4(-0.3239847771997537,    0.5794684678643381,   -0.3239847771997537,   -0.6740059517812944),
			Grad4(-0.4004672082940195,    0.5029860367700724,    0.15296486218853164,  -0.7504883828755602),
			Grad4( 0.15296486218853164,   0.5029860367700724,   -0.4004672082940195,   -0.7504883828755602),
			Grad4( 0.08164729285680945,   0.4553054119602712,    0.08164729285680945,  -0.8828161875373585),
			Grad4(-0.08164729285680945,   0.8828161875373585,   -0.08164729285680945,  -0.4553054119602712),
			Grad4(-0.15296486218853164,   0.7504883828755602,    0.4004672082940195,   -0.5029860367700724),
			Grad4( 0.4004672082940195,    0.7504883828755602,   -0.15296486218853164,  -0.5029860367700724),
			Grad4( 0.3239847771997537,    0.6740059517812944,    0.3239847771997537,   -0.5794684678643381),
			Grad4( 0.5794684678643381,   -0.6740059517812944,   -0.3239847771997537,   -0.3239847771997537),
			Grad4( 0.5029860367700724,   -0.7504883828755602,   -0.4004672082940195,    0.15296486218853164),
			Grad4( 0.5029860367700724,   -0.7504883828755602,    0.15296486218853164,  -0.4004672082940195),
			Grad4( 0.4553054119602712,   -0.8828161875373585,    0.08164729285680945,   0.08164729285680945),
			Grad4( 0.8828161875373585,   -0.4553054119602712,   -0.08164729285680945,  -0.08164729285680945),
			Grad4( 0.7504883828755602,   -0.5029860367700724,   -0.15296486218853164,   0.4004672082940195),
			Grad4( 0.7504883828755602,   -0.5029860367700724,    0.4004672082940195,   -0.15296486218853164),
			Grad4( 0.6740059517812944,   -0.5794684678643381,    0.3239847771997537,    0.3239847771997537),
			Grad4( 0.5794684678643381,   -0.3239847771997537,   -0.6740059517812944,   -0.3239847771997537),
			Grad4( 0.5029860367700724,   -0.4004672082940195,   -0.7504883828755602,    0.15296486218853164),
			Grad4( 0.5029860367700724,    0.15296486218853164,  -0.7504883828755602,   -0.4004672082940195),
			Grad4( 0.4553054119602712,    0.08164729285680945,  -0.8828161875373585,    0.08164729285680945),
			Grad4( 0.8828161875373585,   -0.08164729285680945,  -0.4553054119602712,   -0.08164729285680945),
			Grad4( 0.7504883828755602,   -0.15296486218853164,  -0.5029860367700724,    0.4004672082940195),
			Grad4( 0.7504883828755602,    0.4004672082940195,   -0.5029860367700724,   -0.15296486218853164),
			Grad4( 0.6740059517812944,    0.3239847771997537,   -0.5794684678643381,    0.3239847771997537),
			Grad4( 0.5794684678643381,   -0.3239847771997537,   -0.3239847771997537,   -0.6740059517812944),
			Grad4( 0.5029860367700724,   -0.4004672082940195,    0.15296486218853164,  -0.7504883828755602),
			Grad4( 0.5029860367700724,    0.15296486218853164,  -0.4004672082940195,   -0.7504883828755602),
			Grad4( 0.4553054119602712,    0.08164729285680945,   0.08164729285680945,  -0.8828161875373585),
			Grad4( 0.8828161875373585,   -0.08164729285680945,  -0.08164729285680945,  -0.4553054119602712),
			Grad4( 0.7504883828755602,   -0.15296486218853164,   0.4004672082940195,   -0.5029860367700724),
			Grad4( 0.7504883828755602,    0.4004672082940195,   -0.15296486218853164,  -0.5029860367700724),
			Grad4( 0.6740059517812944,    0.3239847771997537,    0.3239847771997537,   -0.5794684678643381),
			Grad4( 0.03381941603233842,   0.03381941603233842,   0.03381941603233842,   0.9982828964265062),
			Grad4(-0.044802370851755174, -0.044802370851755174,  0.508629699630796,     0.8586508742123365),
			Grad4(-0.044802370851755174,  0.508629699630796,    -0.044802370851755174,  0.8586508742123365),
			Grad4(-0.12128480194602098,   0.4321472685365301,    0.4321472685365301,    0.7821684431180708),
			Grad4( 0.508629699630796,    -0.044802370851755174, -0.044802370851755174,  0.8586508742123365),
			Grad4( 0.4321472685365301,   -0.12128480194602098,   0.4321472685365301,    0.7821684431180708),
			Grad4( 0.4321472685365301,    0.4321472685365301,   -0.12128480194602098,   0.7821684431180708),
			Grad4( 0.37968289875261624,   0.37968289875261624,   0.37968289875261624,   0.753341017856078),
			Grad4( 0.03381941603233842,   0.03381941603233842,   0.9982828964265062,    0.03381941603233842),
			Grad4(-0.044802370851755174,  0.044802370851755174,  0.8586508742123365,    0.508629699630796),
			Grad4(-0.044802370851755174,  0.508629699630796,     0.8586508742123365,   -0.044802370851755174),
			Grad4(-0.12128480194602098,   0.4321472685365301,    0.7821684431180708,    0.4321472685365301),
			Grad4( 0.508629699630796,    -0.044802370851755174,  0.8586508742123365,   -0.044802370851755174),
			Grad4( 0.4321472685365301,   -0.12128480194602098,   0.7821684431180708,    0.4321472685365301),
			Grad4( 0.4321472685365301,    0.4321472685365301,    0.7821684431180708,   -0.12128480194602098),
			Grad4( 0.37968289875261624,   0.37968289875261624,   0.753341017856078,     0.37968289875261624),
			Grad4( 0.03381941603233842,   0.9982828964265062,    0.03381941603233842,   0.03381941603233842),
			Grad4(-0.044802370851755174,  0.8586508742123365,   -0.044802370851755174,  0.508629699630796),
			Grad4(-0.044802370851755174,  0.8586508742123365,    0.508629699630796,    -0.044802370851755174),
			Grad4(-0.12128480194602098,   0.7821684431180708,    0.4321472685365301,    0.4321472685365301),
			Grad4( 0.508629699630796,     0.8586508742123365,   -0.044802370851755174, -0.044802370851755174),
			Grad4( 0.4321472685365301,    0.7821684431180708,   -0.12128480194602098,   0.4321472685365301),
			Grad4( 0.4321472685365301,    0.7821684431180708,    0.4321472685365301,   -0.12128480194602098),
			Grad4( 0.37968289875261624,   0.753341017856078,     0.37968289875261624,   0.37968289875261624),
			Grad4( 0.9982828964265062,    0.03381941603233842,   0.03381941603233842,   0.03381941603233842),
			Grad4( 0.8586508742123365,   -0.044802370851755174, -0.044802370851755174,  0.508629699630796),
			Grad4( 0.8586508742123365,   -0.044802370851755174,  0.508629699630796,    -0.044802370851755174),
			Grad4( 0.7821684431180708,   -0.12128480194602098,   0.4321472685365301,    0.4321472685365301),
			Grad4( 0.8586508742123365,    0.508629699630796,    -0.044802370851755174, -0.044802370851755174),
			Grad4( 0.7821684431180708,    0.4321472685365301,   -0.12128480194602098,   0.4321472685365301),
			Grad4( 0.7821684431180708,    0.4321472685365301,    0.4321472685365301,   -0.12128480194602098),
			Grad4( 0.753341017856078,     0.37968289875261624,   0.37968289875261624,   0.37968289875261624)
		};
		for (unsigned i = 0; i < grad4.size(); i++) {
			grad4[i].x /= N4; grad4[i].y /= N4; grad4[i].z /= N4; grad4[i].w /= N4;
		}
		for (unsigned i = 0; i < simplex::size; i++) {
			result[i] = grad4[i % grad4.size()];
		}
		return result;
	}();

	const std::array<simplex::LatticePoint2D, 4> simplex::lookup2D = {
		LatticePoint2D(1, 0),
		LatticePoint2D(0, 0),
		LatticePoint2D(1, 1),
		LatticePoint2D(0, 1)
	};

	const std::array<std::shared_ptr<simplex::LatticePoint3D>, 8> simplex::lookup3D = []() -> std::array<std::shared_ptr<simplex::LatticePoint3D>, 8> {
		std::array<std::shared_ptr<simplex::LatticePoint3D>, 8> tmp;
		for (int i = 0; i < 8; i++) {
			int i1, j1, k1, i2, j2, k2;
			i1 = (i >> 0) & 1; j1 = (i >> 1) & 1; k1 = (i >> 2) & 1;
			i2 = i1 ^ 1; j2 = j1 ^ 1; k2 = k1 ^ 1;

			// The two points within this octant, one from each of the two cubic half-lattices.
			std::shared_ptr<LatticePoint3D> c0(new LatticePoint3D(i1, j1, k1, 0));
			std::shared_ptr<LatticePoint3D> c1(new LatticePoint3D(i1 + i2, j1 + j2, k1 + k2, 1));

			// Each single step away on the first half-lattice.
			std::shared_ptr<LatticePoint3D> c2(new LatticePoint3D(i1 ^ 1, j1, k1, 0));
			std::shared_ptr<LatticePoint3D> c3(new LatticePoint3D(i1, j1 ^ 1, k1, 0));
			std::shared_ptr<LatticePoint3D> c4(new LatticePoint3D(i1, j1, k1 ^ 1, 0));

			// Each single step away on the second half-lattice.
			std::shared_ptr<LatticePoint3D> c5(new LatticePoint3D(i1 + (i2 ^ 1), j1 + j2, k1 + k2, 1));
			std::shared_ptr<LatticePoint3D> c6(new LatticePoint3D(i1 + i2, j1 + (j2 ^ 1), k1 + k2, 1));
			std::shared_ptr<LatticePoint3D> c7(new LatticePoint3D(i1 + i2, j1 + j2, k1 + (k2 ^ 1), 1));

			// First two are guaranteed.
			c0->nextOnFailure = c0->nextOnSuccess = c1;
			c1->nextOnFailure = c1->nextOnSuccess = c2;

			// Once we find one on the first half-lattice, the rest are out.
			// In addition, knowing c2 rules out c5.
			c2->nextOnFailure = c3; c2->nextOnSuccess = c6;
			c3->nextOnFailure = c4; c3->nextOnSuccess = c5;
			c4->nextOnFailure = c4->nextOnSuccess = c5;

			// Once we find one on the second half-lattice, the rest are out.
			c5->nextOnFailure = c6; c5->nextOnSuccess = nullptr;
			c6->nextOnFailure = c7; c6->nextOnSuccess = nullptr;
			c7->nextOnFailure = c7->nextOnSuccess = nullptr;

			tmp[i] = c0;
		}
		return tmp;
	}();

	const std::array<simplex::LatticePoint4D, 16> simplex::vertices4D = []() -> std::array<simplex::LatticePoint4D, 16> {
		std::array<simplex::LatticePoint4D, 16> tmp;
		for (int i = 0; i < 16; i++) {
			tmp[i] = LatticePoint4D((i >> 0) & 1, (i >> 1) & 1, (i >> 2) & 1, (i >> 3) & 1);		}
		return tmp;
	}();
}
