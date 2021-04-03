#pragma once

#include "math.hpp"
#include "vector.hpp"
#include "quaternion.hpp"

#include <sstream>

#pragma push_macro("minor")
#undef minor

namespace math{
	template <typename type>
	class tmat2;
	template <typename type>
	class tmat3;
	template <typename type>
	class tmat4;

	template <typename type>
	class tmat2 {
	public:
		using col_t = math::tvec2<type>;
		using row_t = math::tvec2<type>;

		tmat2(const tmat2<type> &other) = default;
		template<typename type2>
		tmat2(const tmat2<type2> &other) : m_data(other.dataVec()) {}

		tmat2() : m_data(col_t(1,0), col_t(0,1)) {}
		tmat2(type val) : m_data(col_t(val), col_t(val)) {}
		tmat2(tvec2<col_t> m_data) : m_data(m_data[0], m_data[1]) {}
		tmat2(
			type v00, type v01,
			type v10, type v11) : m_data(
			col_t(v00, v10),
			col_t(v01, v11)
		) {}

		type& at(unsigned col, unsigned row) { return m_data[col][row]; }
		const type& at(unsigned col, unsigned row) const { return m_data[col][row]; }

		tmat3<type> transpose() const {
			return tmat3<type>(	m_data[0][0], m_data[0][1],
								m_data[1][0], m_data[1][1]);
		}

		type determinant() const {
			return at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0);
		}
		type submatrix(unsigned col, unsigned row) const {
			return at((col + 1) % 2, (row + 1) % 2);
		}
		type minor(unsigned col, unsigned row) const {
			return submatrix(col, row);
		}
		type cofactor(unsigned col, unsigned row) const {
			return minor(col, row) * (((col + row) % 2) ? -1 : 1);
		}
		bool invertible() const {
			return determinant() != 0;
		}
		tmat2<type> inverse() const {
			tmat2<type> m;
			type det = determinant();
			if(invertible()) {
				for(unsigned col = 0; col < 2; col++) {
					for(unsigned row = 0; row < 2; row++) {
						type c = cofactor(col, row);
						m[col][row] = c / det;
					}
				}
			}
			return m.transpose();
		}

		tmat2<type> operator*(const tmat2<type>& mat) const {
			tmat2<type> m;
			for(unsigned col = 0; col < 2; col++) {
				for(unsigned row = 0; row < 2; row++) {
					m[col][row] = 	at(0, row) * mat.at(col, 0) +
									at(1, row) * mat.at(col, 1);
				}
			}
			return m;
		}
		tvec2<type> operator*(const tvec2<type>& vec) const {
			tvec2<type> v;
			for(unsigned row = 0; row < 2; row++) {
				v[row] = at(0, row) * vec[0] +
						 at(1, row) * vec[1];
			}
			return v;
		}
		tmat2<type> operator/(const tmat2<type>& mat) const { return inverse() * mat; }
		tmat2<type> operator/(const tvec2<type>& vec) const { return inverse() * vec; }
		type& operator()(unsigned col, unsigned row) { return m_data[col][row]; }
		const type& operator()(unsigned col, unsigned row) const { return m_data[col][row]; }
		col_t& operator[](unsigned col) { return m_data[col]; }
		const col_t& operator[](unsigned col) const { return m_data[col]; }
		tmat2& operator=(const tmat2<type> &other) = default;

		type* data() {
			return &m_data[0][0];
		}
		const type* data() const {
			return &m_data[0][0];
		}

		const tvec2<col_t>& dataVec() const {
			return m_data;
		}

	private:
		tvec2<col_t> m_data;
	};

	template <typename type>
	class tmat3 {
	public:
		using col_t = math::tvec3<type>;
		using row_t = math::tvec3<type>;

		tmat3(const tmat3<type> &other) = default;
		template<typename type2>
		tmat3(const tmat3<type2> &other) : m_data(other.dataVec()) {}

		tmat3() : m_data(	col_t(1,0,0),
							col_t(0,1,0),
							col_t(0,0,1)) {}
		tmat3(type val) : m_data(col_t(val), col_t(val), col_t(val)) {}
		tmat3(tvec3<col_t> m_data) : m_data(m_data) {}
		tmat3(
			type v00, type v01, type v02,
			type v10, type v11, type v12,
			type v20, type v21, type v22) : m_data{
			col_t(v00, v10, v20),
			col_t(v01, v11, v21),
			col_t(v02, v12, v22)
		} {}

		type& at(unsigned col, unsigned row) { return m_data[col][row]; }
		const type& at(unsigned col, unsigned row) const { return m_data[col][row]; }

		tmat3<type> transpose() const {
			return tmat3<type>(	m_data[0][0], m_data[0][1], m_data[0][2],
								m_data[1][0], m_data[1][1], m_data[1][2],
								m_data[2][0], m_data[2][1], m_data[2][2]);
		}

		type determinant() const {
			type det = 0;
			for(unsigned col = 0; col < 3; col++) {
				det += at(col, 0) * cofactor(col, 0);
			}
			return det;
		}
		tmat2<type> submatrix(unsigned col, unsigned row) const {
			tmat2<type> m;
			for(unsigned c = 0; c < 3; c++) {
				for(unsigned r = 0; r < 3; r++) {
					if(c != col && r != row) {
						m[c - (c > col)][r - (r > row)] = at(c, r);
					}
				}
			}
			return m;
		}
		type minor(unsigned col, unsigned row) const {
			return submatrix(col, row).determinant();
		}
		type cofactor(unsigned col, unsigned row) const {
			return minor(col, row) * (((col + row) % 2) ? -1 : 1);
		}
		bool invertible() const {
			return determinant() != 0;
		}
		tmat3<type> inverse() const {
			tmat3<type> m;
			type det = determinant();
			if(invertible()) {
				for(unsigned col = 0; col < 3; col++) {
					for(unsigned row = 0; row < 3; row++) {
						type c = cofactor(col, row);
						m[col][row] = c / det;
					}
				}
			}
			return m.transpose();
		}

		tmat3<type> operator*(const tmat3<type>& mat) const {
			tmat3<type> m;
			for(unsigned col = 0; col < 3; col++) {
				for(unsigned row = 0; row < 3; row++) {
					m[col][row] = 	at(0, row) * mat.at(col, 0) +
									at(1, row) * mat.at(col, 1) +
									at(2, row) * mat.at(col, 2);
				}
			}
			return m;
		}
		tvec3<type> operator*(const tvec3<type>& vec) const {
			tvec3<type> v;
			for(unsigned row = 0; row < 3; row++) {
				v[row] = at(0, row) * vec[0] +
						 at(1, row) * vec[1] +
						 at(2, row) * vec[2];
			}
			return v;
		}
		tmat3<type> operator/(const tmat3<type>& mat) const { return inverse() * mat; }
		tmat3<type> operator/(const tvec3<type>& vec) const { return inverse() * vec; }
		type& operator()(unsigned col, unsigned row) { return m_data[col][row]; }
		const type& operator()(unsigned col, unsigned row) const { return m_data[col][row]; }
		col_t& operator[](unsigned col) { return m_data[col]; }
		const col_t& operator[](unsigned col) const { return m_data[col]; }
		tmat3& operator=(const tmat3<type> &other) = default;

		type* data() {
			return &m_data[0][0];
		}
		const type* data() const {
			return &m_data[0][0];
		}

		const tvec3<col_t>& dataVec() const {
			return m_data;
		}

	private:
		tvec3<col_t> m_data;
	};

	template <typename type>
	class tmat4 {
	public:
		using col_t = math::tvec4<type>;
		using row_t = math::tvec4<type>;

		tmat4(const tmat4<type> &other) = default;
		template<typename type2>
		tmat4(const tmat4<type2> &other) : m_data(other.dataVec()) {}

		tmat4() : m_data(	col_t(1,0,0,0),
							col_t(0,1,0,0),
							col_t(0,0,1,0),
							col_t(0,0,0,1)) {}
		tmat4(type val) : m_data(col_t(val), col_t(val), col_t(val), col_t(val)) {}
		tmat4(tvec4<col_t> m_data) : m_data(m_data) {}
		tmat4(
			type v00, type v01, type v02, type v03,
			type v10, type v11, type v12, type v13,
			type v20, type v21, type v22, type v23,
			type v30, type v31, type v32, type v33) : m_data{
			col_t(v00, v10, v20, v30),
			col_t(v01, v11, v21, v31),
			col_t(v02, v12, v22, v32),
			col_t(v03, v13, v23, v33)
		} {}
		// create matrix from quaternion
		//     1-2y²-2z² | 2xy-2wz   | 2xz+2wy   | 0
		//     2xy+2wz   | 1-2x²-2z² | 2yz+2wx   | 0
		//     2xz-2wy   | 2yz-2*w*x | 1-2x²-2y² | 0
		//     0         | 0         | 0         | 1
		tmat4(const tquaternion<type> &q) : m_data{
			col_t(1 - 2*q.y*q.y - 2*q.z*q.z, 2*q.x*q.y - 2*q.w*q.z, 2*q.x*q.z + 2*q.w*q.y, 0),
			col_t(2*q.x*q.y + 2*q.w*q.z, 1 - 2*q.x*q.x - 2*q.z*q.z, 2*q.y*q.z + 2*q.w*q.x, 0),
			col_t(2*q.x*q.z - 2*q.w*q.y, 2*q.y*q.z - 2*q.w*q.x, 1 - 2*q.x*q.x - 2*q.y*q.y, 0),
			col_t(0, 0, 0, 1)
		} {}

		type& at(unsigned col, unsigned row) { return m_data[col][row]; }
		const type& at(unsigned col, unsigned row) const { return m_data[col][row]; }

		tmat4<type> transpose() {
			return tmat4<type>(	m_data[0][0], m_data[0][1], m_data[0][2], m_data[0][3],
								m_data[1][0], m_data[1][1], m_data[1][2], m_data[1][3],
								m_data[2][0], m_data[2][1], m_data[2][2], m_data[2][3],
								m_data[3][0], m_data[3][1], m_data[3][2], m_data[3][3]);
		}

		type determinant() const {
			type det = 0;
			for(unsigned col = 0; col < 4; col++) {
				det += at(col, 0) * cofactor(col, 0);
			}
			return det;
		}
		tmat3<type> submatrix(unsigned col, unsigned row) const {
			tmat3<type> m;
			for(unsigned c = 0; c < 4; c++) {
				for(unsigned r = 0; r < 4; r++) {
					if(c != col && r != row) {
						m[c - (c > col)][r - (r > row)] = at(c, r);
					}
				}
			}
			return m;
		}
		type minor(unsigned col, unsigned row) const {
			return submatrix(col, row).determinant();
		}
		type cofactor(unsigned col, unsigned row) const {
			return minor(col, row) * (((col + row) % 2) ? -1 : 1);
		}
		bool invertible() const {
			return determinant() != 0;
		}
		tmat4<type> inverse() const {
			tmat4<type> m;
			type det = determinant();
			if(invertible()) {
				for(unsigned col = 0; col < 4; col++) {
					for(unsigned row = 0; row < 4; row++) {
						type c = cofactor(col, row);
						m[col][row] = c / det;
					}
				}
			}
			return m.transpose();
		}

		tmat4<type> operator*(const tmat4<type>& mat) const {
			tmat4<type> m;
			for(unsigned col = 0; col < 4; col++) {
				for(unsigned row = 0; row < 4; row++) {
					m[col][row] = 	at(0, row) * mat.at(col, 0) +
									at(1, row) * mat.at(col, 1) +
									at(2, row) * mat.at(col, 2) +
									at(3, row) * mat.at(col, 3);
				}
			}
			return m;
		}
		tvec4<type> operator*(const tvec4<type>& vec) const {
			tvec4<type> v;
			for(unsigned row = 0; row < 4; row++) {
				v[row] = at(0, row) * vec[0] +
						 at(1, row) * vec[1] +
						 at(2, row) * vec[2] +
						 at(3, row) * vec[3];
			}
			return v;
		}
		tmat4<type> operator/(const tmat4<type>& mat) const { return inverse() * mat; }
		tvec4<type> operator/(const tvec4<type>& vec) const { return inverse() * vec; }

		tmat4<type>& operator*=(const tmat4<type>& other) {
			return operator=(operator*(other));
		}
		tmat4<type>& operator/=(const tmat4<type>& other) {
			return operator=(operator/(other));
		}

		type& operator()(unsigned col, unsigned row) { return m_data[col][row]; }
		const type& operator() (unsigned col, unsigned row) const { return m_data[col][row]; }

		col_t& operator[](unsigned col) { return m_data[col]; }
		const col_t& operator[] (unsigned col) const { return m_data[col]; }

		tmat4& operator=(const tmat4<type> &other) = default;

		static tmat4<type> translation(const tvec3<type> &vec) {
			return tmat4<type>(
				1,0,0,vec.x,
				0,1,0,vec.y,
				0,0,1,vec.z,
				0,0,0,1
			);
		}
		static tmat4<type> rotation(const tvec3<type> &vec) {
			tmat4<type> x(
				1,0,0,0,
				0,cos(vec.x),-sin(vec.x),0,
				0,sin(vec.x), cos(vec.x),0,
				0,0,0,1
			);
			tmat4<type> y(
				cos(vec.y),0,sin(vec.y),0,
				0,1,0,0,
				-sin(vec.y),0,cos(vec.y),0,
				0,0,0,1
			);
			tmat4<type> z(
				cos(vec.z),-sin(vec.z),0,0,
				sin(vec.z), cos(vec.z),0,0,
				0,0,1,0,
				0,0,0,1
			);

			return z*y*x;
		}
		static tmat4<type> scaling(const tvec3<type> &vec) {
			return tmat4<type>(
				vec.x,0,0,0,
				0,vec.y,0,0,
				0,0,vec.z,0,
				0,0,0,1
			);
		}
		static tmat4<type> shearing(type xy, type xz, type yx, type yz, type zx, type zy) {
			return tmat4<type>(
				1 ,xy,xz,0,
				yx,1 ,yz,0,
				zx,zy,1 ,0,
				0 ,0 ,0 ,1
			);
		}
		static tmat4<type> projection(type fov, type aspect, type znear, type zfar) {
			if(fov >= 0 && aspect != 0) {
				type uh = 1.0 / tan(0.5 * fov);
				type uw = uh / aspect;
				return tmat4<type>(
					uw,0,0,0,
					0,uh,0,0,
					0,0,zfar / (zfar - znear),-zfar * znear / (zfar - znear),
					0,0,1,0
				);
			}
			return tmat4<type>();
		}

		tmat4<type> translate(const tvec3<type> &vec) const { return (*this) * translation(vec); }
		tmat4<type> rotate(const tvec3<type> &vec) const { return (*this) * rotation(vec); }
		tmat4<type> scale(const tvec3<type> &vec) const { return (*this) * scaling(vec); }
		tmat4<type> shear(type xy, type xz, type yx, type yz, type zx, type zy) const { return (*this) * shearing(xy, xz, yx, yz, zx, zy); }

		type* data() {
			return &m_data[0][0];
		}
		const type* data() const {
			return &m_data[0][0];
		}

		const tvec4<col_t>& dataVec() const {
			return m_data;
		}

	private:
		tvec4<col_t> m_data;
	};

	typedef tmat2<float> mat2;
	typedef tmat3<float> mat3;
	typedef tmat4<float> mat4;



	template<typename type, std::size_t size>
	class tmatn {
	public:
		using col_t = math::tvecn<type, size>;
		using row_t = math::tvecn<type, size>;

		tmatn() : m_data(tvecn<type, size>(type(0))) {
			for(std::size_t i = 0; i < size; i++) {
				this->at(i, i) = type(1);
			}
		}
		tmatn(const tmatn<type, size> &other) = default;
		template<typename type2>
		tmatn(const tmatn<type2, size> &other) : m_data(other.dataVec()) {}
		tmatn(type val) : m_data(col_t(val), col_t(val), col_t(val), col_t(val)) {}
		tmatn(tvecn<col_t, size> m_data) : m_data(m_data) {}

		tmatn<type, size>& operator=(const tmatn<type, size> &other) = default;

		type& at(unsigned col, unsigned row) { return m_data[col][row]; }
		const type& at(unsigned col, unsigned row) const { return m_data[col][row]; }

		tmatn<type, size> transpose() const {
			tmatn<type, size> tmp;
			for(std::size_t col = 0; col < size; col++) {
				for(std::size_t row = 0; row < size; row++) {
					tmp.at(row, col) = at(col, row);
				}
			}
			return tmp;
		}

		type determinant() const {
			type det = 0;
			for(unsigned col = 0; col < size; col++) {
				det += at(col, 0) * cofactor(col, 0);
			}
			return det;
		}

		// if size > 2 returns [tmatn<type, size-1>] else if size == 2 returns [type]
		auto submatrix(unsigned col, unsigned row) const {
			static_assert(size >= 2, "can't create submatrix of 1x1 matrix or smaller!");

			if constexpr(size == 2) {
				return type(at((col + 1) % 2, (row + 1) % 2));
			}
			else {
				tmatn<type, size - 1> tmp;
				for(unsigned c = 0; c < size; c++) {
					for(unsigned r = 0; r < size; r++) {
						if(c != col && r != row) {
							tmp[c - (c > col)][r - (r > row)] = at(c, r);
						}
					}
				}
				return tmp;
			}
		}

		inline type minor(unsigned col, unsigned row) const {
			if constexpr(size == 2) {
				return submatrix(col, row);
			}
			else {
				return submatrix(col, row).determinant();
			}
		}

		inline type cofactor(unsigned col, unsigned row) const {
			return minor(col, row) * (((col + row) % 2) ? -1 : 1);
		}

		inline bool invertible() const {
			return determinant() != 0;
		}

		tmatn<type, size> inverse() const {
			tmatn<type, size> tmp;
			type det = determinant();
			if(det != 0) {
				for(unsigned col = 0; col < size; col++) {
					for(unsigned row = 0; row < size; row++) {
						type c = cofactor(col, row);
						tmp[col][row] = c / det;
					}
				}
			}
			return tmp.transpose();
		}

		tmatn<type, size> operator*(const tmatn<type, size> &other) const {
			tmatn<type, size> tmp;
			for(unsigned col = 0; col < size; col++) {
				for(unsigned row = 0; row < size; row++) {
					type sum = 0;
					for(unsigned i = 0; i < size; i++) {
						sum += at(i, row) * other.at(col, i);
					}
					tmp[col][row] = sum;
				}
			}
			return tmp;
		}

		tvecn<type, size> operator*(const tvecn<type, size>& vec) const {
			tvecn<type, size> tmp;
			for(unsigned row = 0; row < 4; row++) {
				type sum = 0;
				for(unsigned i = 0; i < size; i++) {
					sum += at(i, row) * vec.at(i);
				}
				tmp[row] = sum;
			}
			return tmp;
		}

		tmatn<type, size> operator/(const tmatn<type, size>& mat) const { return inverse() * mat; }
		tvecn<type, size> operator/(const tvecn<type, size>& vec) const { return inverse() * vec; }

		tmatn<type, size>& operator*=(const tmatn<type, size>& other) {
			return operator=(operator*(other));
		}
		tmatn<type, size>& operator/=(const tmatn<type, size>& other) {
			return operator=(operator/(other));
		}

		type& operator()(unsigned col, unsigned row) { return m_data[col][row]; }
		const type& operator() (unsigned col, unsigned row) const { return m_data[col][row]; }

		col_t& operator[](unsigned col) { return m_data[col]; }
		const col_t& operator[] (unsigned col) const { return m_data[col]; }

		type* data() {
			return &m_data[0][0];
		}
		const type* data() const {
			return &m_data[0][0];
		}

		const tvecn<col_t, size>& dataVec() const {
			return m_data;
		}

	private:
		tvecn<col_t, size> m_data;
	};
}

template<typename type>
std::ostream& operator<<(std::ostream &stream, math::tmat2<type> mat) {
	stream<<"[("<<mat[0][0]<<" | "<<mat[1][0]<<")\n";
	stream<<" ("<<mat[0][1]<<" | "<<mat[1][1]<<")]\n";;
	return stream;
}

template<typename type>
std::ostream& operator<<(std::ostream &stream, math::tmat3<type> mat) {
	stream<<"[("<<mat[0][0]<<" | "<<mat[1][0]<<" | "<<mat[2][0]<<")\n";
	stream<<" ("<<mat[0][1]<<" | "<<mat[1][1]<<" | "<<mat[2][1]<<")\n";
	stream<<" ("<<mat[0][2]<<" | "<<mat[1][2]<<" | "<<mat[2][2]<<")]\n";;
	return stream;
}

template<typename type>
std::ostream& operator<<(std::ostream &stream, math::tmat4<type> mat) {
	stream<<"[("<<mat[0][0]<<" | "<<mat[1][0]<<" | "<<mat[2][0]<<" | "<<mat[3][0]<<")\n";
	stream<<" ("<<mat[0][1]<<" | "<<mat[1][1]<<" | "<<mat[2][1]<<" | "<<mat[3][1]<<")\n";
	stream<<" ("<<mat[0][2]<<" | "<<mat[1][2]<<" | "<<mat[2][2]<<" | "<<mat[3][2]<<")\n";
	stream<<" ("<<mat[0][3]<<" | "<<mat[1][3]<<" | "<<mat[2][3]<<" | "<<mat[3][3]<<")]\n";
	return stream;
}


#pragma pop_macro("minor")
