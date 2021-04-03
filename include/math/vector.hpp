#pragma once

#include <iostream>
#include <cmath>
#include <array>

#include "math.hpp"

#include <imgui/imgui.h>

namespace math {
	template<typename type>	class tvec2;
	template<typename type>	class tvec3;
	template<typename type>	class tvec4;


	template<typename type>
	class tvec2{
	public:
		tvec2(type v = 0.0f) : x(v), y(v) {}
		tvec2(type x, type y) : x(x), y(y) {}

		tvec2(const ImVec2 &v) : x(v.x), y(v.y) {}
		tvec2(const tvec2<type> &other);
		tvec2(const tvec3<type> &other);
		tvec2(const tvec4<type> &other);

		template<typename type2>
		tvec2(const tvec2<type2> &other) : x(other.x), y(other.y) {}

		tvec2<type> operator-() const {return tvec2<type>(-x, -y);}
		tvec2<type> operator+(const tvec2<type> &other) const {return tvec2<type>(x + other.x, y + other.y);}
		tvec2<type> operator-(const tvec2<type> &other) const {return tvec2<type>(x - other.x, y - other.y);}
		tvec2<type> operator*(const tvec2<type> &other) const {return tvec2<type>(x * other.x, y * other.y);}
		tvec2<type> operator/(const tvec2<type> &other) const {return tvec2<type>(x / other.x, y / other.y);}
		tvec2<type> operator%(const tvec2<type> &other) const {return tvec2<type>(x % other.x, y % other.y);}

		tvec2<type>& operator+=(const tvec2<type> &other) {x += other.x, y += other.y; return *this;}
		tvec2<type>& operator-=(const tvec2<type> &other) {x -= other.x, y -= other.y; return *this;}
		tvec2<type>& operator*=(const tvec2<type> &other) {x *= other.x, y *= other.y; return *this;}
		tvec2<type>& operator/=(const tvec2<type> &other) {x /= other.x, y /= other.y; return *this;}
		tvec2<type>& operator%=(const tvec2<type> &other) {x %= other.x, y %= other.y; return *this;}

		tvec2<type>& operator=(const tvec2<type> &other) {x = other.x, y = other.y; return *this;}
		template <typename t2>
		tvec2<type>& operator=(const tvec2<t2> &other) {x = other.x, y = other.y; return *this;}

		auto operator<=>(const tvec2<type> &other) const {
			if constexpr(std::is_floating_point<type>()) {
				return length(*this) <=> length(other);
			}
			else {
				if(x == other.x) {
					return y <=> other.y;
				}
				return x <=> other.x;
			}
		}

		operator ImVec2() {
			return {x, y};
		}

		bool operator==(const tvec2<type> &other) const {return x == other.x && y == other.y;}

		type& operator[](unsigned i) {
			switch(i) {
				case 0: return x;
				case 1: return y;
			}
			return x;
		}
		const type& operator[](unsigned i) const {
			switch(i) {
				case 0: return x;
				case 1: return y;
			}
			return x;
		}

		union {
			struct {
				type r, g;
			};
			struct {
				type x, y;
			};
		};
	};

	template <>
	inline tvec2<float> tvec2<float>::operator%(const tvec2<float> &other) const {return tvec2<float>(std::fmod(x ,other.x), std::fmod(y, other.y));}
	template <>
	inline tvec2<float>& tvec2<float>::operator%=(const tvec2<float> &other) {x = std::fmod(x ,other.x); y = std::fmod(y, other.y); return *this;}

	template <>
	inline tvec2<double> tvec2<double>::operator%(const tvec2<double> &other) const {return tvec2<double>(std::fmod(x ,other.x), std::fmod(y, other.y));}
	template <>
	inline tvec2<double>& tvec2<double>::operator%=(const tvec2<double> &other) {x = std::fmod(x ,other.x); y = std::fmod(y, other.y); return *this;}

	template<typename type>
	class tvec3{
	public:
		tvec3(type v = 0.0f) : x(v), y(v), z(v) {}
		tvec3(type x, type y, type z) : x(x), y(y), z(z) {}

		tvec3(const tvec2<type> &other, type z = 0);
		tvec3(const tvec3<type> &other);
		tvec3(const tvec4<type> &other);

		template<typename type2>
		tvec3(const tvec3<type2> &other) : x(other.x), y(other.y), z(other.z) {}

		tvec3<type> operator-() const {return tvec3<type>(-x, -y, -z);}
		tvec3<type> operator+(const tvec3<type> &other) const {return tvec3<type>(x + other.x, y + other.y, z + other.z);}
		tvec3<type> operator-(const tvec3<type> &other) const {return tvec3<type>(x - other.x, y - other.y, z - other.z);}
		tvec3<type> operator*(const tvec3<type> &other) const {return tvec3<type>(x * other.x, y * other.y, z * other.z);}
		tvec3<type> operator/(const tvec3<type> &other) const {return tvec3<type>(x / other.x, y / other.y, z / other.z);}
		tvec3<type> operator%(const tvec3<type> &other) const {return tvec3<type>(x % other.x, y % other.y, z % other.z);}

		tvec3<type>& operator+=(const tvec3<type> &other) {x += other.x, y += other.y, z += other.z; return *this;}
		tvec3<type>& operator-=(const tvec3<type> &other) {x -= other.x, y -= other.y, z -= other.z; return *this;}
		tvec3<type>& operator*=(const tvec3<type> &other) {x *= other.x, y *= other.y, z *= other.z; return *this;}
		tvec3<type>& operator/=(const tvec3<type> &other) {x /= other.x, y /= other.y, z /= other.z; return *this;}
		tvec3<type>& operator%=(const tvec3<type> &other) {x %= other.x, y %= other.y, z %= other.z; return *this;}

		tvec3<type>& operator=(const tvec3<type> &other){x = other.x, y = other.y, z = other.z; return *this;}
		template <typename t2>
		tvec3<type>& operator=(const tvec3<t2> &other){x = other.x, y = other.y, z = other.z; return *this;}

		bool operator==(const tvec3<type> &other) const {return x == other.x && y == other.y && z == other.z;}

		type& operator[](unsigned i) {
			switch(i) {
				case 0: return x;
				case 1: return y;
				case 2: return z;
			}
			return x;
		}
		const type& operator[](unsigned i) const {
			switch(i) {
				case 0: return x;
				case 1: return y;
				case 2: return z;
			}
			return x;
		}

		auto operator<=>(const tvec3<type> &other) const {
			if constexpr(std::is_floating_point<type>()) {
				return length(*this) <=> length(other);
			}
			else {
				if(x == other.x) {
					if(y == other.y) {
						return z <=> other.z;
					}
					return y <=> other.y;
				}
				return x <=> other.x;
			}
		}

		union {
			struct {
				type r, g, b;
			};
			struct {
				type x, y, z;
			};
			tvec2<type> xy;
		};
	};

	template <>
	inline tvec3<float> tvec3<float>::operator%(const tvec3<float> &other) const {return tvec3<float>(std::fmod(x ,other.x), std::fmod(y, other.y), std::fmod(z, other.z));}
	template <>
	inline tvec3<float>& tvec3<float>::operator%=(const tvec3<float> &other) {x = std::fmod(x ,other.x), y = std::fmod(y, other.y), z = std::fmod(z, other.z); return *this;}

	template <>
	inline tvec3<double> tvec3<double>::operator%(const tvec3<double> &other) const {return tvec3<double>(std::fmod(x ,other.x), std::fmod(y, other.y), std::fmod(z, other.z));}
	template <>
	inline tvec3<double>& tvec3<double>::operator%=(const tvec3<double> &other) {x = std::fmod(x ,other.x), y = std::fmod(y, other.y), z = std::fmod(z, other.z); return *this;}

	template<typename type>
	class tvec4{
	public:
		tvec4(type v = 0) : x(v), y(v) , z(v), w(v) {}
		tvec4(type x, type y, type z = 0, type w = 0) : x(x), y(y), z(z), w(w) {}

		tvec4(const ImVec4 &v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
		tvec4(const tvec2<type> &other, tvec2<type> rest);
		tvec4(const tvec2<type> &other, type z = 0, type w = 0);
		tvec4(const tvec3<type> &other, type w = 0);
		tvec4(const tvec4<type> &other);

		template<typename type2>
		tvec4(const tvec4<type2> &other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

		tvec4<type> operator-() const {return tvec4<type>(-x, -y, -z, -w);}
		tvec4<type> operator+(const tvec4<type> &other) const {return tvec4<type>(x + other.x, y + other.y, z + other.z, w + other.w);}
		tvec4<type> operator-(const tvec4<type> &other) const {return tvec4<type>(x - other.x, y - other.y, z - other.z, w - other.w);}
		tvec4<type> operator*(const tvec4<type> &other) const {return tvec4<type>(x * other.x, y * other.y, z * other.z, w * other.w);}
		tvec4<type> operator/(const tvec4<type> &other) const {return tvec4<type>(x / other.x, y / other.y, z / other.z, w / other.w);}
		tvec4<type> operator%(const tvec4<type> &other) const {return tvec4<type>(x % other.x, y % other.y, z % other.z, w % other.w);}

		tvec4<type>& operator+=(const tvec4<type> &other) {x += other.x, y += other.y, z += other.z, w += other.w; return *this;}
		tvec4<type>& operator-=(const tvec4<type> &other) {x -= other.x, y -= other.y, z -= other.z, w -= other.w; return *this;}
		tvec4<type>& operator*=(const tvec4<type> &other) {x *= other.x, y *= other.y, z *= other.z, w *= other.w; return *this;}
		tvec4<type>& operator/=(const tvec4<type> &other) {x /= other.x, y /= other.y, z /= other.z, w /= other.w; return *this;}
		tvec4<type>& operator%=(const tvec4<type> &other) {x %= other.x, y %= other.y, z %= other.z, w %= other.w; return *this;}

		tvec4<type>& operator=(const tvec4<type> &other){x = other.x, y = other.y, z = other.z, w = other.w; return *this;}
		template <typename t2>
		tvec4<type>& operator=(const tvec4<t2> &other){x = other.x, y = other.y, z = other.z, w = other.w; return *this;}
		operator ImVec4() {
			return {x, y, z, w};
		}

		bool operator==(const tvec4<type> &other) const {return x == other.x && y == other.y && z == other.z && w == other.w;}

		type& operator[](unsigned i) {
			switch(i) {
				case 0: return x;
				case 1: return y;
				case 2: return z;
				case 3: return w;
			}
			return x;
		}
		const type& operator[](unsigned i) const {
			switch(i) {
				case 0: return x;
				case 1: return y;
				case 2: return z;
				case 3: return w;
			}
			return x;
		}

		auto operator<=>(const tvec3<type> &other) const {
			if constexpr(std::is_floating_point<type>()) {
				return length(*this) <=> length(other);
			}
			else {
				if(x == other.x) {
					if(y == other.y) {
						if(z == other.z) {
							return w <=> other.w;
						}
						return z <=> other.z;
					}
					return y <=> other.y;
				}
				return x <=> other.x;
			}
		}

		union {
			struct {
				type r, g, b, a;
			};
			struct {
				type x, y, z, w;
			};
			struct {
				tvec2<type> xy, zw;
			};
			struct {
				tvec2<type> rg, ba;
			};
			tvec3<type> xyz;
			tvec3<type> rgb;
		};
	};

	template <>
	inline tvec4<float> tvec4<float>::operator%(const tvec4<float> &other) const {return tvec4<float>(std::fmod(x ,other.x), std::fmod(y, other.y), std::fmod(z, other.z), std::fmod(w, other.w));}
	template <>
	inline tvec4<float>& tvec4<float>::operator%=(const tvec4<float> &other) {x = std::fmod(x ,other.x), y = std::fmod(y, other.y), z = std::fmod(z, other.z), w = std::fmod(w, other.w); return *this;}

	template <>
	inline tvec4<double> tvec4<double>::operator%(const tvec4<double> &other) const {return tvec4<double>(std::fmod(x ,other.x), std::fmod(y, other.y), std::fmod(z, other.z), std::fmod(w, other.w));}
	template <>
	inline tvec4<double>& tvec4<double>::operator%=(const tvec4<double> &other) {x = std::fmod(x ,other.x), y = std::fmod(y, other.y), z = std::fmod(z, other.z), w = std::fmod(w, other.w); return *this;}

	template<typename type>	tvec2<type>::tvec2(const tvec2<type> &other) : x(other.x), y(other.y) {}
	template<typename type>	tvec2<type>::tvec2(const tvec3<type> &other) : x(other.x), y(other.y) {}
	template<typename type>	tvec2<type>::tvec2(const tvec4<type> &other) : x(other.x), y(other.y) {}

	template<typename type>	tvec3<type>::tvec3(const tvec2<type> &other, type z) : x(other.x), y(other.y), z(z) {}
	template<typename type>	tvec3<type>::tvec3(const tvec3<type> &other) : x(other.x), y(other.y), z(other.z) {}
	template<typename type>	tvec3<type>::tvec3(const tvec4<type> &other) : x(other.x), y(other.y), z(other.z) {}

	template<typename type>	tvec4<type>::tvec4(const tvec2<type> &other, tvec2<type> rest) : x(other.x), y(other.y), z(rest.x), w(rest.y) {}
	template<typename type>	tvec4<type>::tvec4(const tvec2<type> &other, type z, type w) : x(other.x), y(other.y), z(z), w(w) {}
	template<typename type>	tvec4<type>::tvec4(const tvec3<type> &other, type w) : x(other.x), y(other.y), z(other.z), w(w) {}
	template<typename type>	tvec4<type>::tvec4(const tvec4<type> &other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

	template<typename type>
	type length(const tvec2<type> &v){
		return sqrt(v.x*v.x + v.y*v.y);
	}
	template<typename type>
	type length(const tvec3<type> &v){
		return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	}
	template<typename type>
	type length(const tvec4<type> &v){
		return sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
	}

	template<typename type>
	type dist(const tvec2<type> &a, const tvec2<type> &b){
		return length(a-b);
	}
	template<typename type>
	type dist(const tvec3<type> &a, const tvec3<type> &b){
		return length(a-b);
	}
	template<typename type>
	type dist(const tvec4<type> &a, const tvec4<type> &b){
		return length(a-b);
	}

	template<typename type>
	tvec2<type> normalize(const tvec2<type> &v){
		return v / length(v);
	}
	template<typename type>
	tvec3<type> normalize(const tvec3<type> &v){
		return v / length(v);
	}
	template<typename type>
	tvec4<type> normalize(const tvec4<type> &v){
		return v / length(v);
	}

	template<typename type>
	type dot(const tvec2<type> &a, const tvec2<type> &b){
		return a.x*b.x + a.y*b.y;
	}
	template<typename type>
	type dot(const tvec3<type> &a, const tvec3<type> &b){
		return a.x*b.x + a.y*b.y + a.z*b.z;
	}
	template<typename type>
	type dot(const tvec4<type> &a, const tvec4<type> &b){
		return a.x*b.x + a.y*b.y + a.z*b.z;
	}

	template<typename type>
	tvec3<type> cross(const tvec3<type> &v, const tvec3<type> &a){
		return tvec3<type>(v.y*a.z - v.z*a.y, v.z*a.x - v.x*a.z, v.x*a.y - v.y*a.x);
	}

	template<typename type>
	tvec2<type> floor(const tvec2<type> &v) {
		return tvec2<type>(std::floor(v.x), std::floor(v.y));
	}
	template<typename type>
	tvec3<type> floor(const tvec3<type> &v) {
		return tvec3<type>(std::floor(v.x), std::floor(v.y), std::floor(v.z));
	}
	template<typename type>
	tvec4<type> floor(const tvec4<type> &v) {
		return tvec4<type>(std::floor(v.x), std::floor(v.y), std::floor(v.z), std::floor(v.w));
	}

	template<typename type>
	tvec2<type> ceil(const tvec2<type> &v) {
		return tvec2<type>(std::ceil(v.x), std::ceil(v.y));
	}
	template<typename type>
	tvec3<type> ceil(const tvec3<type> &v) {
		return tvec3<type>(std::ceil(v.x), std::ceil(v.y), std::ceil(v.z));
	}
	template<typename type>
	tvec4<type> ceil(const tvec4<type> &v) {
		return tvec4<type>(std::ceil(v.x), std::ceil(v.y), std::ceil(v.z), std::ceil(v.w));
	}

	template<typename type>
	tvec2<type> round(const tvec2<type> &v) {
		return tvec2<type>(std::round(v.x), std::round(v.y));
	}
	template<typename type>
	tvec3<type> round(const tvec3<type> &v) {
		return tvec3<type>(std::round(v.x), std::round(v.y), std::round(v.z));
	}
	template<typename type>
	tvec4<type> round(const tvec4<type> &v) {
		return tvec4<type>(std::round(v.x), std::round(v.y), std::round(v.z), std::round(v.w));
	}

	template<typename type>
	tvec2<type> fract(const tvec2<type> &v) {
		return tvec2<type>(fract(v.x), fract(v.y));
	}
	template<typename type>
	tvec3<type> fract(const tvec3<type> &v) {
		return tvec3<type>(fract(v.x), fract(v.y), fract(v.z));
	}
	template<typename type>
	tvec4<type> fract(const tvec4<type> &v) {
		return tvec4<type>(fract(v.x), fract(v.y), fract(v.z), fract(v.w));
	}

	template<typename type>
	tvec2<type> min(const tvec2<type> &a, const tvec2<type> &b) {
		return tvec2<type>(std::min(a.x, b.x), std::min(a.y, b.y));
	}
	template<typename type>
	tvec3<type> min(const tvec2<type> &a, const tvec2<type> &b) {
		return tvec3<type>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
	}
	template<typename type>
	tvec4<type> min(const tvec2<type> &a, const tvec2<type> &b) {
		return tvec4<type>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w));
	}

	template<typename type>
	tvec2<type> max(const tvec2<type> &a, const tvec2<type> &b) {
		return tvec2<type>(std::max(a.x, b.x), std::max(a.y, b.y));
	}
	template<typename type>
	tvec3<type> max(const tvec2<type> &a, const tvec2<type> &b) {
		return tvec3<type>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
	}
	template<typename type>
	tvec4<type> max(const tvec2<type> &a, const tvec2<type> &b) {
		return tvec4<type>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w));
	}

	template<typename type>
	tvec2<type> clamp(const tvec2<type> &v, type min, type max) {
		return tvec2<type>(clamp(v.x, min, max), clamp(v.y, min, max));
	}
	template<typename type>
	tvec3<type> clamp(const tvec3<type> &v, type min, type max) {
		return tvec3<type>(clamp(v.x, min, max), clamp(v.y, min, max), clamp(v.z, min, max));
	}
	template<typename type>
	tvec4<type> clamp(const tvec4<type> &v, type min, type max) {
		return tvec4<type>(clamp(v.x, min, max), clamp(v.y, min, max), clamp(v.z, min, max), clamp(v.w, min, max));
	}

	template<typename type>
	tvec2<type> mix(const tvec2<type> &a, const tvec2<type> &b, float amnt) {
		return tvec2<type>(mix(a.x, b.x, amnt), mix(a.y, b.y, amnt));
	}
	template<typename type>
	tvec3<type> mix(const tvec3<type> &a, const tvec3<type> &b, float amnt) {
		return tvec3<type>(mix(a.x, b.x, amnt), mix(a.y, b.y, amnt), mix(a.z, b.z, amnt));
	}
	template<typename type>
	tvec4<type> mix(const tvec4<type> &a, const tvec4<type> &b, float amnt) {
		return tvec4<type>(mix(a.x, b.x, amnt), mix(a.y, b.y, amnt), mix(a.z, b.z, amnt), mix(a.w, b.w, amnt));
	}

	template<typename type>
	tvec2<type> mix(const tvec2<type> &a, const tvec2<type> &b, const tvec2<float> &amnt) {
		return tvec2<type>(mix(a.x, b.x, amnt.x), mix(a.y, b.y, amnt.y));
	}
	template<typename type>
	tvec3<type> mix(const tvec3<type> &a, const tvec3<type> &b, const tvec3<float> &amnt) {
		return tvec3<type>(mix(a.x, b.x, amnt.x), mix(a.y, b.y, amnt.y), mix(a.z, b.z, amnt.z));
	}
	template<typename type>
	tvec4<type> mix(const tvec4<type> &a, const tvec4<type> &b, const tvec4<float> &amnt) {
		return tvec4<type>(mix(a.x, b.x, amnt.x), mix(a.y, b.y, amnt.y), mix(a.z, b.z, amnt.z), mix(a.w, b.w, amnt.w));
	}

	template<typename type>
	tvec2<type> smoothstep(const tvec2<type> &v) {
		return tvec2<type>(smoothstep(v.x), smoothstep(v.y));
	}
	template<typename type>
	tvec3<type> smoothstep(const tvec3<type> &v) {
		return tvec3<type>(smoothstep(v.x), smoothstep(v.y), smoothstep(v.z));
	}
	template<typename type>
	tvec4<type> smoothstep(const tvec4<type> &v) {
		return tvec4<type>(smoothstep(v.x), smoothstep(v.y), smoothstep(v.z), smoothstep(v.w));
	}

	template<typename type>
	tvec2<type> sigmoid(const tvec2<type> &v) {
		return tvec2<type>(sigmoid(v.x), sigmoid(v.y));
	}
	template<typename type>
	tvec3<type> sigmoid(const tvec3<type> &v) {
		return tvec3<type>(sigmoid(v.x), sigmoid(v.y), sigmoid(v.z));
	}
	template<typename type>
	tvec4<type> sigmoid(const tvec4<type> &v) {
		return tvec4<type>(sigmoid(v.x), sigmoid(v.y), sigmoid(v.z), sigmoid(v.w));
	}

	template<typename type>
	tvec2<type> lerp(const tvec2<type> &start, const tvec2<type> &end, float t) {
		return start * (1.0f - t) + end * t;
	}

	template<typename type>
	tvec3<type> lerp(const tvec3<type> &start, const tvec3<type> &end, float t) {
		return start * (1.0f - t) + end * t;
	}

	template<typename type>
	tvec4<type> lerp(const tvec4<type> &start, const tvec4<type> &end, float t) {
		return start * (1.0f - t) + end * t;
	}

	template<typename type>
	tvec2<type> slerp(const tvec2<type> &start, const tvec2<type> &end, float t) {
		float d = clamp(dot(start, end), -1.0f, 1.0f);
		float tetha = acos(d) * t;
		tvec4<type> rel = normalize(end - start * d);
		return start * cos(tetha) + rel * sin(tetha);
	}

	template<typename type>
	tvec3<type> slerp(const tvec3<type> &start, const tvec3<type> &end, float t) {
		float d = clamp(dot(start, end), -1.0f, 1.0f);
		float tetha = acos(d) * t;
		tvec4<type> rel = normalize(end - start * d);
		return start * cos(tetha) + rel * sin(tetha);
	}

	template<typename type>
	tvec4<type> slerp(const tvec4<type> &start, const tvec4<type> &end, float t) {
		float d = clamp(dot(start, end), -1.0f, 1.0f);
		float tetha = acos(d) * t;
		tvec4<type> rel = normalize(end.xyz - start.xyz * d);
		return start * cos(tetha) + rel * sin(tetha);
	}

	typedef tvec2<int> ivec2;
	typedef tvec3<int> ivec3;
	typedef tvec4<int> ivec4;
	typedef tvec2<unsigned> uvec2;
	typedef tvec3<unsigned> uvec3;
	typedef tvec4<unsigned> uvec4;

	typedef tvec2<uint8_t> bvec2;
	typedef tvec3<uint8_t> bvec3;
	typedef tvec4<uint8_t> bvec4;

	typedef tvec2<int16_t> svec2;
	typedef tvec3<int16_t> svec3;
	typedef tvec4<int16_t> svec4;

	typedef tvec2<int64_t> lvec2;
	typedef tvec3<int64_t> lvec3;
	typedef tvec4<int64_t> lvec4;

#ifdef __SIZEOF_INT128__
	typedef tvec2<__int128> llvec2;
	typedef tvec3<__int128> llvec3;
	typedef tvec4<__int128> llvec4;
#endif

	typedef tvec2<float> vec2;
	typedef tvec3<float> vec3;
	typedef tvec4<float> vec4;
	typedef tvec2<double> dvec2;
	typedef tvec3<double> dvec3;
	typedef tvec4<double> dvec4;



	template<typename type, std::size_t size>
	class tvecn {
	public:
		tvecn(type value = 0) : m_data{} {
			m_data.fill(value);
		}
		template<typename ...Args>
		tvecn(Args ...args) : m_data{args...} {}
		tvecn(const tvecn<type, size> &other) = default;

		tvecn<type, size>& operator=(const tvecn<type, size> &other) = default;
		type& at(std::size_t index) { return m_data[index]; }
		type& operator[](std::size_t index) { return m_data[index]; }

		const type& at(std::size_t index) const { return m_data[index]; }
		const type& operator[](std::size_t index) const { return m_data[index]; }

		tvecn<type, size> operator-() const {
			tvecn<type, size> result;
			for(std::size_t i = 0; i < size; i++) {
				result[i] = -this->at(i);
			}
			return result;
		}

		tvecn<type, size> operator+(const tvecn<type, size> &other) const {
			tvecn<type, size> result;
			for(std::size_t i = 0; i < size; i++) {
				result[i] = this->at(i) + other.at(i);
			}
			return result;
		}
		tvecn<type, size>& operator+=(const tvecn<type, size> &other) {
			for(std::size_t i = 0; i < size; i++) {
				this->at(i) += other.at(i);
			}
			return *this;
		}

		tvecn<type, size> operator-(const tvecn<type, size> &other) const {
			tvecn<type, size> result;
			for(std::size_t i = 0; i < size; i++) {
				result[i] = this->at(i) - other.at(i);
			}
			return result;
		}
		tvecn<type, size>& operator-=(const tvecn<type, size> &other) {
			for(std::size_t i = 0; i < size; i++) {
				this->at(i) -= other.at(i);
			}
			return *this;
		}

		tvecn<type, size> operator*(const tvecn<type, size> &other) const {
			tvecn<type, size> result;
			for(std::size_t i = 0; i < size; i++) {
				result[i] = this->at(i) * other.at(i);
			}
			return result;
		}
		tvecn<type, size> operator*(type scalar) const {
			tvecn<type, size> result;
			for(std::size_t i = 0; i < size; i++) {
				result[i] = this->at(i) * scalar;
			}
			return result;
		}
		tvecn<type, size>& operator*=(const tvecn<type, size> &other) {
			for(std::size_t i = 0; i < size; i++) {
				this->at(i) *= other.at(i);
			}
			return *this;
		}
		tvecn<type, size>& operator*=(type scalar) {
			for(std::size_t i = 0; i < size; i++) {
				this->at(i) *= scalar;
			}
			return *this;
		}

		tvecn<type, size> operator/(const tvecn<type, size> &other) const {
			tvecn<type, size> result;
			for(std::size_t i = 0; i < size; i++) {
				result[i] = this->at(i) / other.at(i);
			}
			return result;
		}
		tvecn<type, size> operator/(type scalar) const {
			tvecn<type, size> result;
			for(std::size_t i = 0; i < size; i++) {
				result[i] = this->at(i) / scalar;
			}
			return result;
		}
		tvecn<type, size>& operator/=(const tvecn<type, size> &other) {
			for(std::size_t i = 0; i < size; i++) {
				this->at(i) /= other.at(i);
			}
			return *this;
		}
		tvecn<type, size>& operator/=(type scalar) {
			for(std::size_t i = 0; i < size; i++) {
				this->at(i) /= scalar;
			}
			return *this;
		}

		tvecn<type, size> operator%(const tvecn<type, size> &other) const {
			tvecn<type, size> result;
			for(std::size_t i = 0; i < size; i++) {
				if constexpr(std::is_floating_point<type>()) {
					result[i] = std::fmod(this->at(i), other.at(i));
				}
				else {
					result[i] = this->at(i) % other.at(i);
				}
			}
			return result;
		}
		tvecn<type, size>& operator%=(const tvecn<type, size> &other) {
			for(std::size_t i = 0; i < size; i++) {
				if constexpr(std::is_floating_point<type>()) {
					this->at(i) = std::fmod(this->at(i), other.at(i));
				}
				else {
					this->at(i) %= other.at(i);
				}
			}
			return *this;
		}

		auto operator<=>(const tvec3<type> &other) const {
			return length(*this) <=> length(other);
		}

		typename std::array<type, size>::iterator begin() {
			return m_data.begin();
		}

		typename std::array<type, size>::iterator end() {
			return m_data.end();
		}

	private:
		std::array<type, size> m_data;
	};

	template<typename type, std::size_t size>
	type length(const tvecn<type, size> &v){
		type sum = 0;
		for(type t : v) {
			sum += v*v;
		}
		return sqrt(sum);
	}

	template<typename type, std::size_t size>
	type dist(const tvecn<type, size> &a, const tvecn<type, size> &b){
		return length(a-b);
	}

	template<typename type, std::size_t size>
	tvecn<type, size> normalize(const tvecn<type, size> &v){
		return v / length(v);
	}

	template<typename type, std::size_t size>
	type dot(const tvecn<type, size> &a, const tvecn<type, size> &b){
		type sum = 0;
		for(std::size_t i = 0; i < size; i++) {
			sum += a[i] * b[i];
		}
		return sum;
	}

	template<typename type, std::size_t size>
	tvecn<type, size> floor(tvecn<type, size> vec) {
		for(type &value : vec) {
			value = std::floor(value);
		}
		return vec;
	}

	template<typename type, std::size_t size>
	tvecn<type, size> ceil(tvecn<type, size> vec) {
		for(type &value : vec) {
			value = std::ceil(value);
		}
		return vec;
	}

	template<typename type, std::size_t size>
	tvecn<type, size> round(tvecn<type, size> vec) {
		for(type &value : vec) {
			value = std::round(value);
		}
		return vec;
	}

	template<typename type, std::size_t size>
	tvecn<type, size> fract(tvecn<type, size> vec) {
		for(type &value : vec) {
			value = fract(value);
		}
		return vec;
	}

	template<typename type, std::size_t size>
	tvecn<type, size> min(const tvecn<type, size> &a, const tvecn<type, size> &b) {
		tvecn<type, size> tmp;
		for(std::size_t i = 0; i < size; i++) {
			tmp[i] = std::min(a[i], b[i]);
		}
		return tmp;
	}

	template<typename type, std::size_t size>
	tvecn<type, size> max(const tvecn<type, size> &a, const tvecn<type, size> &b) {
		tvecn<type, size> tmp;
		for(std::size_t i = 0; i < size; i++) {
			tmp[i] = std::max(a[i], b[i]);
		}
		return tmp;
	}

	template<typename type, std::size_t size>
	tvecn<type, size> clamp(const tvecn<type, size> &vec, type min, type max) {
		tvecn<type, size> tmp;
		for(std::size_t i = 0; i < size; i++) {
			tmp[i] = std::clamp(vec[i], min, max);
		}
		return tmp;
	}

	template<typename type, std::size_t size>
	tvecn<type, size> clamp(const tvecn<type, size> &vec, const tvecn<type, size> &min, const tvecn<type, size> &max) {
		tvecn<type, size> tmp;
		for(std::size_t i = 0; i < size; i++) {
			tmp[i] = std::clamp(vec[i], min[i], max[i]);
		}
		return tmp;
	}

	template<typename type, std::size_t size>
	tvecn<type, size> mix(const tvecn<type, size> &a, const tvecn<type, size> &b, float amnt) {
		tvecn<type, size> tmp;
		for(std::size_t i = 0; i < size; i++) {
			tmp[i] = mix(a[i], b[i], amnt);
		}
		return tmp;
	}

	template<typename type, std::size_t size>
	tvecn<type, size> mix(const tvecn<type, size> &a, const tvecn<type, size> &b, const tvecn<float, size> &amnt) {
		tvecn<type, size> tmp;
		for(std::size_t i = 0; i < size; i++) {
			tmp[i] = mix(a[i], b[i], amnt[i]);
		}
		return tmp;
	}

	template<typename type, std::size_t size>
	tvecn<type, size> smoothstep(tvecn<type, size> vec) {
		for(type &value : vec) {
			value = smoothstep(value);
		}
		return vec;
	}

	template<typename type, std::size_t size>
	tvecn<type, size> sigmoid(tvecn<type, size> vec) {
		for(type &value : vec) {
			value = sigmoid(value);
		}
		return vec;
	}

	template<typename type, std::size_t size>
	tvecn<type, size> lerp(const tvecn<type, size> &start, const tvecn<type, size> &end, float t) {
		return start * (1.0f - t) + end * t;
	}
}

template<typename type>
std::ostream& operator<<(std::ostream &os, const math::tvec2<type> &vec){
	return os<<"("<<vec.x<<" | "<<vec.y<<")";
}
template<typename type>
std::ostream& operator<<(std::ostream &os, const math::tvec3<type> &vec){
	return os<<"("<<vec.x<<" | "<<vec.y<<" | "<<vec.z<<")";
}
template<typename type>
std::ostream& operator<<(std::ostream &os, const math::tvec4<type> &vec){
	return os<<"("<<vec.x<<" | "<<vec.y<<" | "<<vec.z<<" | "<<vec.w<<")";
}

template<typename type, std::size_t size>
std::ostream& operator<<(std::ostream &os, const math::tvecn<type, size> &vec){
	os<<"(";
	for(std::size_t i = 0; i < size-1; i++) {
		os<<vec[i]<<" | ";
	}
	return os<<vec[size-1]<<")";
}