#ifndef BASICMATH_MAT4_H
#define BASICMATH_MAT4_H
#include "vec4.h"
#include "mat3.h"

namespace BasicMath
{


	//! A simple 4x4 matrix class
	/*!
	Internally it is represented as a 4 elements array of type vec4 for each of the rows 0,1,2
	*/
	class mat4 {


	public:
		mat4() {}
		mat4(float s)
		{
			v[0] = s; v[1] = s; v[2] = s; v[3] = s;
		}
		mat4(float e0, float e1, float e2,float e3,
			float e4, float e5,float e6, float e7,
			float e8,float e9, float e10, float e11,
			float e12, float e13, float e14, float e15)
		{
			v[0][0] = e0; v[0][1] = e1; v[0][2] = e2; v[0][3] = e3;
			v[1][0] = e4; v[1][1] = e5; v[1][2] = e6; v[1][3] = e7;
			v[2][0] = e8; v[2][1] = e9; v[2][2] = e10; v[2][3] = e11;
			v[3][0] = e12; v[3][1] = e13; v[3][2] = e14; v[3][3] = e15;
		}

		mat4(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
		{
			v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3;
		}

		mat4(const mat4& other)
		{
			v[0] = other[0];
			v[1] = other[1];
			v[2] = other[2];
			v[3] = other[3];
		}

		inline const mat4& operator+() const { return *this; }
		inline mat4 operator-() const { return mat4(-v[0], -v[1], -v[2], -v[3]); }
		//! returns the i-th row as a vec4
		inline vec4 operator[](int i) const { return v[i]; }
		//! returns the i-th row as a vec4&
		inline vec4& operator[](int i) { return v[i]; };


		inline mat4& operator+=(const mat4& m2);
		inline mat4& operator-=(const mat4& m2);
		//! matrix multiplication
		inline mat4& operator*=(const mat4& m2);
		//! matrix divion (through the inverse)
		inline mat4& operator/=(const mat4& m2);
		inline mat4& operator*=(const float t);
		inline mat4& operator/=(const float t);

		//! returns the determinant
		inline float det() const;

		//! returns the identity matrix
		static mat4 identity;

		//! returns a scaling matrix
		static mat4 scaling(const vec4& scale);

		//! returns a homogeneous coordinates rotation matrix for rotation around the x axis
		static mat4 rotationX(float rx);

		//! returns a homogeneous coordinates rotation matrix for rotation around the y axis
		static mat4 rotationY(float ry);

		//! returns a homogeneous coordinates rotation matrix for rotation around the z axis
		static mat4 rotationZ(float rz);

		//! returns a homogeneous coordinates rotation matrix for rotation around the x-y-z axes
		static mat4 rotationXYZ(vec4 angles);

		vec4 v[4];
	};

	inline mat4& mat4::operator+=(const mat4 &m) {
		v[0] += m.v[0];
		v[1] += m.v[1];
		v[2] += m.v[2];
		v[3] += m.v[3];
		return *this;
	}

	inline mat4& mat4::operator*=(const mat4 &m) {
		v[0][0] = v[0][0] * m.v[0][0] + v[0][1] * m.v[1][0] + v[0][2] * m.v[2][0] + v[0][3] * m.v[3][0];
		v[0][1] = v[0][0] * m.v[0][1] + v[0][1] * m.v[1][1] + v[0][2] * m.v[2][1] + v[0][3] * m.v[3][1];
		v[0][2] = v[0][0] * m.v[0][2] + v[0][1] * m.v[1][2] + v[0][2] * m.v[2][2] + v[0][3] * m.v[3][2];
		v[0][3] = v[0][0] * m.v[0][3] + v[0][1] * m.v[1][3] + v[0][2] * m.v[2][3] + v[0][3] * m.v[3][3];
		v[1][0] = v[1][0] * m.v[0][0] + v[1][1] * m.v[1][0] + v[1][2] * m.v[2][0] + v[1][3] * m.v[3][0];
		v[1][1] = v[1][0] * m.v[0][1] + v[1][1] * m.v[1][1] + v[1][2] * m.v[2][1] + v[1][3] * m.v[3][1];
		v[1][2] = v[1][0] * m.v[0][2] + v[1][1] * m.v[1][2] + v[1][2] * m.v[2][2] + v[1][3] * m.v[3][2];
		v[1][3] = v[1][0] * m.v[0][3] + v[1][1] * m.v[1][3] + v[1][2] * m.v[2][3] + v[1][3] * m.v[3][3];
		v[2][0] = v[2][0] * m.v[0][0] + v[2][1] * m.v[1][0] + v[2][2] * m.v[2][0] + v[2][3] * m.v[3][0];
		v[2][1] = v[2][0] * m.v[0][1] + v[2][1] * m.v[1][1] + v[2][2] * m.v[2][1] + v[2][3] * m.v[3][1];
		v[2][2] = v[2][0] * m.v[0][2] + v[2][1] * m.v[1][2] + v[2][2] * m.v[2][2] + v[2][3] * m.v[3][2];
		v[2][3] = v[2][0] * m.v[0][3] + v[2][1] * m.v[1][3] + v[2][2] * m.v[2][3] + v[2][3] * m.v[3][3];
		v[3][0] = v[3][0] * m.v[0][0] + v[3][1] * m.v[1][0] + v[3][2] * m.v[2][0] + v[3][3] * m.v[3][0];
		v[3][1] = v[3][0] * m.v[0][1] + v[3][1] * m.v[1][1] + v[3][2] * m.v[2][1] + v[3][3] * m.v[3][1];
		v[3][2] = v[3][0] * m.v[0][2] + v[3][1] * m.v[1][2] + v[3][2] * m.v[2][2] + v[3][3] * m.v[3][2];
		v[3][3] = v[3][0] * m.v[0][3] + v[3][1] * m.v[1][3] + v[3][2] * m.v[2][3] + v[3][3] * m.v[3][3];
		return *this;
	}

	inline mat4& mat4::operator/=(const mat4 &m) {
		*this *= inverse(m);
		return *this;
	}

	inline mat4& mat4::operator-=(const mat4& m) {
		v[0] -= m.v[0];
		v[1] -= m.v[1];
		v[2] -= m.v[2];
		v[3] -= m.v[3];
		return *this;
	}

	inline mat4& mat4::operator*=(const float t) {
		v[0] *= t;
		v[1] *= t;
		v[2] *= t;
		v[3] *= t;
		return *this;
	}

	inline mat4& mat4::operator/=(const float t) {
		float k = 1.0 / t;

		v[0] *= k;
		v[1] *= k;
		v[2] *= k;
		v[2] *= k;
		return *this;
	}


	inline float mat4::det() const {
		mat3 A00(v[1].yzw, v[2].yzw, v[3].yzw);
		mat3 A01(v[1].xzw, v[2].xzw, v[3].xzw);
		mat3 A02(v[1].xyw, v[2].xyw, v[3].xyw);
		mat3 A03(v[1].xyz, v[2].xyz, v[3].xyz);
		return v[0][0] * A00.det() - v[0][1] * A01.det() + v[0][2] * A02.det() - v[0][3] * A03.det();
	}


	mat4 mat4::identity(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	mat4 mat4::scaling(const vec4& scale)
	{
		return mat4(scale[0], 0, 0, 0,
			0, scale[1], 0, 0,
			0, 0, scale[2], 0,
			0, 0, 0, scale[3]);
	}

	mat4 mat4::rotationX(float theta)
	{
		float c = cos(theta); float s = sin(theta);
		return mat4(1, 0, 0, 0,
			0, c, -s, 0,
			0, s, c, 0,
			0, 0, 0, 1);
	}

	mat4 mat4::rotationY(float theta)
	{
		float c = cos(theta); float s = sin(theta);
		return mat4(c, 0, s, 0,
			0, 1, 0, 0,
			-s, 0, c, 0,
			0, 0, 0, 1);
	}

	mat4 mat4::rotationZ(float theta)
	{
		float c = cos(theta); float s = sin(theta);
		return mat4(c, -s, 0, 0,
			s, c, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	mat4 mat4::rotationXYZ(vec4 angles)
	{
		float c1 = cos(angles[0]); float s1 = sin(angles[0]);
		float c2 = cos(angles[1]); float s2 = sin(angles[1]);
		float c3 = cos(angles[2]); float s3 = sin(angles[2]);
		float s2c3 = s2*c3; float s2s3 = s2*s3;
		return mat4(c2*c3, s1*s2c3 - c1*s3, c1*s2c3 + s1*s3, 0,
			c2*s3, c1*c3 + s1*s2s3, c1*s2s3 - s1*c3, 0,
			-s2, s1*c2, c1*c2, 0,
			0, 0, 0, 1);
	}

	/**
	* \defgroup mat4_functions mat4 functions
	* functions for 3x3 matrices
	* @{
	*/

	inline std::istream& operator >> (std::istream &is, mat4 &t) {
		is >> t.v[0] >> t.v[1] >> t.v[2] >> t.v[3];
		return is;
	}

	inline std::ostream& operator<<(std::ostream &os, const mat4 &t) {
		os << t.v[0] << "\n" << t.v[1] << "\n" << t.v[2] << "\n" << t.v[3];
		return os;
	}

	inline mat4 operator+(const mat4 &m1, const mat4 &m2) {
		return mat4(m1.v[0] + m2.v[0], m1.v[1] + m2.v[1], m1.v[2] + m2.v[2], m1.v[3] + m2.v[3]);
	}

	inline mat4 operator-(const mat4 &m1, const mat4 &m2) {
		return mat4(m1.v[0] - m2.v[0], m1.v[1] - m2.v[1], m1.v[2] - m2.v[2], m1.v[3] - m2.v[3]);
	}

	//! matrix multiplication
	inline mat4 operator*(const mat4 &m1, const mat4 &m2) {
		mat4 res(m1);
		return (res *= m2);
	}

	//! matrix division (through the inverse)
	inline mat4 operator/(const mat4 &m1, const mat4 &m2) {
		mat4 res(m1);
		return (res /= m2);
	}

	inline mat4 operator*(float t, const mat4 &m) {
		return mat4(t*m.v[0], t*m.v[1], t*m.v[2], t*m.v[3]);
	}

	inline mat4 operator/(const mat4& m, float t) {
		return mat4(m.v[0] / t, m.v[1] / t, m.v[2] / t, m.v[3] / t);
	}

	inline mat4 operator*(const mat4 &m, float t) {
		return mat4(t*m.v[0], t*m.v[1], t*m.v[2], t*m.v[3]);
	}

	//! returns a matrix from column vectors
	inline mat4 matrixFromColumns(const vec4& v0, const vec4& v1, const vec4& v2, const vec4& v3)
	{
		return mat4(v0[0], v1[0], v2[0], v3[0],
			v0[1], v1[1], v2[1], v3[1],
			v0[2], v1[2], v2[2], v3[2], 
			v0[3], v1[3], v2[3], v3[3] );
	}

	//! returns the transposed matrix
	inline mat4 transpose(const mat4& m)
	{
		return matrixFromColumns(m[0], m[1], m[2], m[3]);
	}

	//! returns the i,j-th adjugate matrix
	inline mat3 adjugate(const mat4& m, int i, int j)
	{
		mat3 res;
		int i1=1, i2=2, i3=3;
		int j1 = 1, j2 = 2, j3 = 3;
		switch (i)
		{
		case 0:

			break;
		case 1:
			--i1;
			break;

		case 2:
			--i1; --i2;
			break;
		case 3:
			--i1; --i2; --i3;
			break;
		}
		switch (j)
		{
		case 0:

			break;
		case 1:
			--j1;
			break;

		case 2:
			--j1; --j2;
			break;
		case 3:
			--j1; --j2; --j3;
			break;
		}
		return mat3(m[i1][j1], m[i1][j2], m[i1][j3],
			m[i2][j1], m[i2][j2], m[i2][j3],
			m[i3][j1], m[i3][j2], m[i3][j3]);
	}

	//! returns the inverse matrix

	//! doesn't check for det==0
	inline mat4 inverse(const mat4& m)
	{
		float A00 = adjugate(m, 0, 0).det(), A01 = -adjugate(m, 0, 1).det(), A02 = adjugate(m, 0, 2).det(), A03 = -adjugate(m, 0, 3).det(),
			A10 = -adjugate(m, 1, 0).det(), A11 = adjugate(m, 1, 1).det(), A12 = -adjugate(m, 1, 2).det(), A13 = adjugate(m, 1, 3).det(),
			A20 = adjugate(m, 2, 0).det(), A21 = -adjugate(m, 2, 1).det(), A22 = adjugate(m, 2, 2).det(), A23 = -adjugate(m, 2, 3).det(),
			A30 = -adjugate(m, 3, 0).det(), A31 = adjugate(m, 3, 1).det(), A32 = -adjugate(m, 3, 2).det(), A33 = adjugate(m, 3, 3).det();
		float det = m[0][0] * A00 + m[0][1] * A01 + m[0][2] * A02 + m[0][3] * A03;
		return 1 / det * mat4(A00, A10, A20, A30,
			A01, A11, A21, A31,
			A02, A12, A22, A32,
			A03, A13, A23, A33);
	}

	//! removes NaN values from matrix
	inline mat4 de_nan(const mat4& c) {
		mat4 temp = c;
		if (!(temp[0] == temp[0])) temp[0] = de_nan(temp[0]);
		if (!(temp[1] == temp[1])) temp[1] = de_nan(temp[1]);
		if (!(temp[2] == temp[2])) temp[2] = de_nan(temp[2]);
		if (!(temp[3] == temp[3])) temp[3] = de_nan(temp[3]);
		return temp;
	}

	/**@}*/

}

#endif
