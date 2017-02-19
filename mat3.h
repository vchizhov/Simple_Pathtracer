#ifndef BASICMATH_MAT3_H
#define BASICMATH_MAT3_H
#include "vec3.h"

namespace BasicMath
{


	//! A simple 3x3 matrix class
	/*!
		Internally it is represented as a 3 elements array of type vec3 for each of the rows 0,1,2
	*/
	class mat3 {


	public:
		mat3() {}
		explicit mat3(float s)
			:v0(s),v1(s),v2(s)
		{
		}
		mat3(float e0, float e1, float e2,
			float e3, float e4, float e5,
			float e6, float e7, float e8)
		{ 
			v[0].x = e0; v[0].y = e1; v[0].z = e2;
			v[1].x = e3; v[1].y = e4; v[1].z = e5;
			v[2].x = e6; v[2].y = e7; v[2].z = e8;
		}

		mat3(vec3 v0, vec3 v1, vec3 v2)
		{
			v[0] = v0; v[1] = v1; v[2] = v2;
		}

		mat3(const mat3& other) 
		{ 
			v[0] = other[0];
			v[1] = other[1];
			v[2] = other[2];
		}

		inline const mat3& operator+() const { return *this; }
		inline mat3 operator-() const { return mat3(-v[0], -v[1],-v[2]); }
		//! returns the i-th row as a vec3
		inline vec3 operator[](int i) const { return v[i];}
		//! returns the i-th row as a vec3&
		inline vec3& operator[](int i) { return v[i]; };

		//! returns the i-th column vector
		inline vec3 column(int i) { return vec3(v[0][i], v[1][i], v[2][i]); }

		inline mat3& operator+=(const mat3& m2);
		inline mat3& operator-=(const mat3& m2);
		//! matrix multiplication
		inline mat3& operator*=(const mat3& m2);
		//! matrix-vector multiplication
		inline vec3 operator*=(const vec3& vc);
		//! matrix division(through inverse)
		inline mat3& operator/=(const mat3& m2);

		inline mat3& operator*=(const float t);
		inline mat3& operator/=(const float t);

		//! returns the determinant
		inline float det() const;

		//! returns the identity matrix
		static mat3 identity;
		
		//! returns a scaling matrix
		static mat3 scaling(const vec3& scale);

		//! returns a rotation matrix for rotation around the x axis
		static mat3 rotationX(float rx);

		//! returns a rotation matrix for rotation around the y axis
		static mat3 rotationY(float ry);

		//! returns a rotation matrix for rotation around the z axis
		static mat3 rotationZ(float rz);

		//! returns a rotation matrix for rotation around the x-y-z axes
		static mat3 rotationXYZ(vec3 angles);

		union
		{
			vec3 v[3];
			struct
			{
				vec3 v0, v1, v2;
			};
		};
	};

	inline mat3& mat3::operator+=(const mat3 &m) {
		v[0] += m.v[0];
		v[1] += m.v[1];
		v[2] += m.v[2];
		return *this;
	}

	inline mat3& mat3::operator*=(const mat3 &m) {
		mat3 res;
		res[0][0] = v[0][0] * m.v[0][0] + v[0][1] * m.v[1][0] + v[0][2] * m.v[2][0];
		res[0][1] = v[0][0] * m.v[0][1] + v[0][1] * m.v[1][1] + v[0][2] * m.v[2][1];
		res[0][2] = v[0][0] * m.v[0][2] + v[0][1] * m.v[1][2] + v[0][2] * m.v[2][2];
		res[1][0] = v[1][0] * m.v[0][0] + v[1][1] * m.v[1][0] + v[1][2] * m.v[2][0];
		res[1][1] = v[1][0] * m.v[0][1] + v[1][1] * m.v[1][1] + v[1][2] * m.v[2][1];
		res[1][2] = v[1][0] * m.v[0][2] + v[1][1] * m.v[1][2] + v[1][2] * m.v[2][2];
		res[2][0] = v[2][0] * m.v[0][0] + v[2][1] * m.v[1][0] + v[2][2] * m.v[2][0];
		res[2][1] = v[2][0] * m.v[0][1] + v[2][1] * m.v[1][1] + v[2][2] * m.v[2][1];
		res[2][2] = v[2][0] * m.v[0][2] + v[2][1] * m.v[1][2] + v[2][2] * m.v[2][2];
		v[0] = res[0];
		v[1] = res[1];
		v[2] = res[2];

		return *this;
	}

	inline vec3 mat3::operator*=(const vec3& vc)
	{
		return vec3(dotProduct(v[0], vc), dotProduct(v[1], vc), dotProduct(v[2], vc));
	}

	//declare it here so I can use it in /=
	inline mat3 inverse(const mat3& m);

	inline mat3& mat3::operator/=(const mat3 &m) {
		mat3 res = inverse(m);
		*this *= res;
		return *this;
	}

	inline mat3& mat3::operator-=(const mat3& m) {
		v[0] -= m.v[0];
		v[1] -= m.v[1];
		v[2] -= m.v[2];
		return *this;
	}

	inline mat3& mat3::operator*=(const float t) {
		v[0] *= t;
		v[1] *= t;
		v[2] *= t;
		return *this;
	}

	inline mat3& mat3::operator/=(const float t) {
		float k = float(1.0) / t;

		v[0] *= k;
		v[1] *= k;
		v[2] *= k;
		return *this;
	}

	inline float mat3::det() const {
		vec3 res = crossProduct(v[1], v[2]);
		res *= v[0];
		return res[0] + res[1] + res[2];
	}


	mat3 mat3::identity( 1, 0, 0, 0, 1, 0, 0, 0, 1);

	mat3 mat3::scaling(const vec3& scale)
	{
		return mat3(scale[0], 0, 0,
			0, scale[1], 0,
			0, 0, scale[2]);
	}

	mat3 mat3::rotationX(float theta)
	{
		float c = cosf(theta); float s = sinf(theta);
		return mat3(1, 0, 0,
			0, c, -s,
			0, s, c);
	}

	mat3 mat3::rotationY(float theta)
	{
		float c = cosf(theta); float s = sinf(theta);
		return mat3(c, 0, s,
			0, 1, 0,
			-s, 0, c);
	}

	mat3 mat3::rotationZ(float theta)
	{
		float c = cosf(theta); float s = sinf(theta);
		return mat3(c, -s, 0,
			s, c, 0,
			0, 0, 1);
	}
	
	mat3 mat3::rotationXYZ(vec3 angles)
	{
		float c1 = cosf(angles[0]); float s1 = sinf(angles[0]);
		float c2 = cosf(angles[1]); float s2 = sinf(angles[1]);
		float c3 = cosf(angles[2]); float s3 = sinf(angles[2]);
		float s2c3 = s2*c3; float s2s3 = s2*s3;
		return mat3(c2*c3, s1*s2c3 - c1*s3, c1*s2c3 + s1*s3,
			c2*s3, c1*c3 + s1*s2s3, c1*s2s3 - s1*c3,
			-s2, s1*c2, c1*c2);
	}

	/**
	* \defgroup mat3_functions mat3 functions
	* functions for 3x3 matrices
	* @{
	*/

	inline std::istream& operator >> (std::istream &is, mat3 &t) {
		is >> t.v[0] >> t.v[1] >> t.v[2];
		return is;
	}

	inline std::ostream& operator<<(std::ostream &os, const mat3 &t) {
		os << t.v[0] << "\n" << t.v[1] << "\n" << t.v[2];
		return os;
	}

	inline mat3 operator+(const mat3 &m1, const mat3 &m2) {
		return mat3(m1.v[0]+m2.v[0], m1.v[1] + m2.v[1], m1.v[2] + m2.v[2]);
	}

	inline mat3 operator-(const mat3 &m1, const mat3 &m2) {
		return mat3(m1.v[0] - m2.v[0], m1.v[1] - m2.v[1], m1.v[2] - m2.v[2]);
	}

	//! matrix multiplication
	inline mat3 operator*(const mat3 &m1, const mat3 &m2) {
		mat3 res;
		res.v[0][0] = m1.v[0][0] * m2.v[0][0] + m1.v[0][1] * m2.v[1][0] + m1.v[0][2] * m2.v[2][0];
		res.v[0][1] = m1.v[0][0] * m2.v[0][1] + m1.v[0][1] * m2.v[1][1] + m1.v[0][2] * m2.v[2][1];
		res.v[0][2] = m1.v[0][0] * m2.v[0][2] + m1.v[0][1] * m2.v[1][2] + m1.v[0][2] * m2.v[2][2];
		res.v[1][0] = m1.v[1][0] * m2.v[0][0] + m1.v[1][1] * m2.v[1][0] + m1.v[1][2] * m2.v[2][0];
		res.v[1][1] = m1.v[1][0] * m2.v[0][1] + m1.v[1][1] * m2.v[1][1] + m1.v[1][2] * m2.v[2][1];
		res.v[1][2] = m1.v[1][0] * m2.v[0][2] + m1.v[1][1] * m2.v[1][2] + m1.v[1][2] * m2.v[2][2];
		res.v[2][0] = m1.v[2][0] * m2.v[0][0] + m1.v[2][1] * m2.v[1][0] + m1.v[2][2] * m2.v[2][0];
		res.v[2][1] = m1.v[2][0] * m2.v[0][1] + m1.v[2][1] * m2.v[1][1] + m1.v[2][2] * m2.v[2][1];
		res.v[2][2] = m1.v[2][0] * m2.v[0][2] + m1.v[2][1] * m2.v[1][2] + m1.v[2][2] * m2.v[2][2];
		return res;
	}

	//! matrix-vector multiplication
	inline vec3 operator*(const mat3 &m, const vec3& v) {
		return vec3(dotProduct(m.v[0], v), dotProduct(m.v[1], v), dotProduct(m.v[2], v));
	}

	//! matrix division (through division)
	inline mat3 operator/(const mat3 &m1, const mat3 &m2) {
		return m1*inverse(m2);
	}

	inline mat3 operator*(float t, const mat3 &m) {
		return mat3(t*m.v[0], t*m.v[1], t*m.v[2]);
	}

	inline mat3 operator/(const mat3& m, float t) {
		return mat3(m.v[0] / t, m.v[1] / t, m.v[2] / t);
	}

	inline mat3 operator*(const mat3 &m, float t) {
		return mat3(t*m.v[0], t*m.v[1], t*m.v[2]);
	}

	//! returns a matrix from column vectors
	inline mat3 matrixFromColumns(const vec3& v0, const vec3& v1, const vec3& v2)
	{
		return mat3(v0.x, v1.x, v2.x,
			v0.y, v1.y, v2.y,
			v0.z, v1.z, v2.z);
	}

	//! returns the transposed matrix
	inline mat3 transpose(const mat3& m)
	{
		return matrixFromColumns(m[0], m[1], m[2]);
	}


	//! returns the inverse matrix

	//! doesn't check for det==0
	inline mat3 inverse(const mat3& m)
	{
		vec3 v12 = crossProduct(m.v[1], m.v[2]);
		vec3 v01 = crossProduct(m.v[0], m.v[1]);
		vec3 v20 = crossProduct(m.v[2], m.v[0]);
		vec3 det = m.v[0] * v12;
		det[0] += det[1] + det[2];
		return 1 / det[0] * matrixFromColumns(v12, v20, v01);
	}

	//! removes NaN values from matrix
	inline mat3 de_nan(const mat3& c) {
		mat3 temp = c;
		if (!(temp[0] == temp[0])) temp[0] = de_nan(temp[0]);
		if (!(temp[1] == temp[1])) temp[1] = de_nan(temp[1]);
		if (!(temp[2] == temp[2])) temp[2] = de_nan(temp[2]);
		return temp;
	}

	//! creates a coordinate system from a single vector (up vector)
	mat3 createCoordinateSystem(const vec3& v)
	{
		vec3 w;
		if (std::fabsf(v.x) > std::fabsf(v.y))
			w = vec3(v.z, 0, -v.x) / sqrt(v.x * v.x + v.z * v.z);
		else
			w = vec3(0, -v.z, v.y) / sqrt(v.y * v.y + v.z * v.z);
		vec3 u = crossProduct(v, w);
		return matrixFromColumns(u, v, w);
	}


	/**@}*/



}

#endif