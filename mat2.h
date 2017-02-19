#ifndef BASICMATH_MAT2_H
#define BASICMATH_MAT2_H
#include "vec2.h"

namespace BasicMath
{


	//! A simple 3x3 matrix class
	/*!
	Internally it is represented as a 3 elements array of type vec2 for each of the rows 0,1,2
	*/
	class mat2 {


	public:
		mat2() {}
		mat2(float s)
		{
			v[0] = s; v[1] = s;
		}
		mat2(float e0, float e1, 
			float e2,float e3)
		{
			v[0].e[0] = e0; v[0].e[1] = e1;
			v[1].e[0] = e2; v[1].e[1] = e3;
		}

		mat2(vec2 v0, vec2 v1)
		{
			v[0] = v0; v[1] = v1;
		}

		mat2(const mat2& other)
		{
			v[0]= other[0];
			v[1]= other[1];
		}

		inline const mat2& operator+() const { return *this; }
		inline mat2 operator-() const { return mat2(-v[0], -v[1]); }
		//! returns the i-th row as a vec2
		inline vec2 operator[](int i) const { return v[i]; }
		//! returns the i-th row as a vec2&
		inline vec2& operator[](int i) { return v[i]; };

		//! returns the i-th column vector
		inline vec2 column(int i) { return vec2(v[0][i], v[1][i]); }

		inline mat2& operator+=(const mat2& m2);
		inline mat2& operator-=(const mat2& m2);
		//! matrix multiplication
		inline mat2& operator*=(const mat2& m2);
		//! matrix-vector multiplication
		inline vec2 operator*=(const vec2& vc);
		//! matrix division(through inverse)
		inline mat2& operator/=(const mat2& m2);

		inline mat2& operator*=(const float t);
		inline mat2& operator/=(const float t);

		//! returns the determinant
		inline float det() const;

		//! returns the identity matrix
		static mat2 identity;

		//! returns a scaling matrix
		static mat2 scaling(const vec2& scale);

		//! returns a rotation matrix
		static mat2 rotationX(float r);

		vec2 v[2];
	};

	inline mat2& mat2::operator+=(const mat2 &m) {
		v[0] += m.v[0];
		v[1] += m.v[1];
		return *this;
	}

	//declare it here so I can use it in *=
	inline mat2 operator*(const mat2&, const mat2&);

	inline mat2& mat2::operator*=(const mat2 &m) {
		mat2 res;

		res = (*this) * m;
		std::swap((*this), res);

		return *this;
	}

	inline vec2 mat2::operator*=(const vec2& vc)
	{
		return vec2(dotProduct(v[0], vc), dotProduct(v[1], vc));
	}

	//declare it here so I can use it in /=
	inline mat2 inverse(const mat2& m);

	inline mat2& mat2::operator/=(const mat2 &m) {
		mat2 res = inverse(m);
		*this *= res;
		return *this;
	}

	inline mat2& mat2::operator-=(const mat2& m) {
		v[0] -= m.v[0];
		v[1] -= m.v[1];
		return *this;
	}

	inline mat2& mat2::operator*=(const float t) {
		v[0] *= t;
		v[1] *= t;
		return *this;
	}

	inline mat2& mat2::operator/=(const float t) {
		float k = float(1.0) / t;

		v[0] *= k;
		v[1] *= k;
		return *this;
	}

	inline float mat2::det() const {
		return areaProduct(v[1], v[2]);
	}

	mat2 mat2::identity(1, 0, 0, 1);

	mat2 mat2::scaling(const vec2& scale)
	{
		return mat2(scale[0], 0,
			0, scale[1]);
	}

	mat2 mat2::rotationX(float theta)
	{
		float c = cosf(theta); float s = sinf(theta);
		return mat2(c, -s,
			s, c);
	}

	/**
	* \defgroup mat2_functions mat2 functions
	* functions for 3x3 matrices
	* @{
	*/

	inline std::istream& operator >> (std::istream &is, mat2 &t) {
		is >> t.v[0] >> t.v[1];
		return is;
	}

	inline std::ostream& operator<<(std::ostream &os, const mat2 &t) {
		os << t.v[0] << "\n" << t.v[1];
		return os;
	}

	inline mat2 operator+(const mat2 &m1, const mat2 &m2) {
		return mat2(m1.v[0] + m2.v[0], m1.v[1] + m2.v[1]);
	}

	inline mat2 operator-(const mat2 &m1, const mat2 &m2) {
		return mat2(m1.v[0] - m2.v[0], m1.v[1] - m2.v[1]);
	}

	//! matrix multiplication
	inline mat2 operator*(const mat2 &m1, const mat2 &m2) {
		mat2 res;
		res[0][0] = m1[0][0] * m2[0][0] + m1[0][1] * m2[1][0];
		res[0][1] = m1[0][0] * m2[0][1] + m1[0][1] * m2[1][1];
		res[1][0] = m1[1][0] * m2[0][0] + m1[1][1] * m2[1][0];
		res[1][1] = m1[1][0] * m2[0][1] + m1[1][1] * m2[1][1];
		return res;
	}

	//! matrix-vector multiplication
	inline vec2 operator*(const mat2 &m, const vec2& v) {
		return vec2(dotProduct(m[0], v), dotProduct(m[1], v));
	}

	//! matrix division (through division)
	inline mat2 operator/(const mat2 &m1, const mat2 &m2) {
		return m1*inverse(m2);
	}

	inline mat2 operator*(float t, const mat2 &m) {
		return mat2(t*m.v[0], t*m.v[1]);
	}

	inline mat2 operator/(const mat2& m, float t) {
		return mat2(m.v[0] / t, m.v[1] / t);
	}

	inline mat2 operator*(const mat2 &m, float t) {
		return mat2(t*m.v[0], t*m.v[1]);
	}

	//! returns a matrix from column vectors
	inline mat2 matrixFromColumns(const vec2& v0, const vec2& v1)
	{
		return mat2(v0[0], v1[0],
			v0[1], v1[1]);
	}

	//! returns the transposed matrix
	inline mat2 transpose(const mat2& m)
	{
		return matrixFromColumns(m[0], m[1]);
	}


	//! returns the inverse matrix

	//! doesn't check for det==0
	inline mat2 inverse(const mat2& m)
	{
		return 1/m.det()*mat2(m[1][1], -m[0][1],
			-m[1][0], m[0][0]);
	}

	//! removes NaN values from matrix
	inline mat2 de_nan(const mat2& c) {
		mat2 temp = c;
		if (!(temp[0] == temp[0])) temp[0] = de_nan(temp[0]);
		if (!(temp[1] == temp[1])) temp[1] = de_nan(temp[1]);
		return temp;
	}

	//! creates a coordinate system from a single vector (up vector)
	mat2 createCoordinateSystem(const vec2 v)
	{
		vec2 u(v.y, -v.x);
		return matrixFromColumns(u, v);
	}

	

	/**@}*/

}

#endif
