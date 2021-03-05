#include <math/math.hpp>

#if defined(WINDOWS)
	#include <intrin.h>

	static uint64_t __inline ctz( uint64_t x )
	{
		unsigned long r = 0;
		_BitScanForward(&r, x);
		return r;
	}
#else
	#define ctz(x) __builtin_ctz(x)
#endif

namespace math{
	unsigned int gcd(unsigned int u, unsigned int v){
	    int shift;
	    if (u == 0) return v;
	    if (v == 0) return u;
	    shift = ctz(u | v);
	    u >>= ctz(u);
	    do {
	        v >>= ctz(v);
	        if (u > v) {
	            unsigned int t = v;
	            v = u;
	            u = t;
	        }
	        v = v - u;
	    } while (v != 0);
	    return u << shift;
	}
}
