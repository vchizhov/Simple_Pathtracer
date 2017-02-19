#ifndef BASICMATH_VEC2_H
#define BASICMATH_VEC2_H
#include "util.h"


namespace BasicMath
{
	//! A simple 2d vector class
	class vec2 {


	public:
		vec2() {}
		explicit vec2(float s) { e[0] = s; e[1] = s;}
		vec2(float e0, float e1) { e[0] = e0; e[1] = e1;}
		vec2(const vec2& other) { e[0] = other[0]; e[1] = other[1];}

		inline const vec2& operator+() const { return *this; }
		inline vec2 operator-() const { return vec2(-e[0], -e[1]); }
		inline float operator[](int i) const { return e[i]; }
		inline float& operator[](int i) { return e[i]; };

		inline vec2& operator+=(const vec2&);
		inline vec2& operator-=(const vec2&);
		//! componentwise multiplication
		inline vec2& operator*=(const vec2&);
		//! componentwise division
		inline vec2& operator/=(const vec2&);
		inline vec2& operator*=(const float);
		inline vec2& operator/=(const float);

		bool operator==(const vec2&) const;

		bool operator!=(const vec2&) const;

		inline float length() const { return sqrtf(e[0] * e[0] + e[1] * e[1]); }
		inline float squared_length() const { return e[0] * e[0] + e[1] * e[1]; }
		//! vector normalization
		inline vec2& make_unit_vector();

		union
		{
			float e[2];
			struct
			{
				float x, y;
			};
			struct
			{
				float u, v;
			};
		};
	};

	inline vec2& vec2::operator+=(const vec2 &v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		return *this;
	}

	inline vec2& vec2::operator*=(const vec2 &v) {
		e[0] *= v.e[0];
		e[1] *= v.e[1];
		return *this;
	}

	inline vec2& vec2::operator/=(const vec2 &v) {
		e[0] /= v.e[0];
		e[1] /= v.e[1];
		return *this;
	}

	inline vec2& vec2::operator-=(const vec2& v) {
		e[0] -= v.e[0];
		e[1] -= v.e[1];
		return *this;
	}

	inline vec2& vec2::operator*=(const float t) {
		e[0] *= t;
		e[1] *= t;
		return *this;
	}

	inline vec2& vec2::operator/=(const float t) {
		float k = float(1.0) / t;

		e[0] *= k;
		e[1] *= k;
		return *this;
	}

	bool vec2::operator==(const vec2& v2) const
	{
		return (e[0] == v2.e[0] && e[1] == v2.e[1]);
	}

	bool vec2::operator!=(const vec2& v2) const
	{
		return (e[0] != v2.e[0] || e[1] != v2.e[1]);
	}

	inline vec2& vec2::make_unit_vector()
	{
		float k = float(1.0 / sqrtf(e[0] * e[0] + e[1] * e[1]));
		e[0] *= k; e[1] *= k; e[2] *= k;
		return *this;
	}


	/**
	* \defgroup vec2_functions vec2 functions
	* functions for 3d vectors
	* @{
	*/

	inline std::istream& operator >> (std::istream &is, vec2 &t) {
		is >> t.e[0] >> t.e[1];
		return is;
	}

	inline std::ostream& operator<<(std::ostream &os, const vec2 &t) {
		os << t.e[0] << " " << t.e[1];
		return os;
	}

	inline vec2 operator+(const vec2 &v1, const vec2 &v2) {
		return vec2(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1]);
	}

	inline vec2 operator-(const vec2 &v1, const vec2 &v2) {
		return vec2(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1]);
	}

	//! componentwise multiplication
	inline vec2 operator*(const vec2 &v1, const vec2 &v2) {
		return vec2(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1]);
	}

	//! componentwise division
	inline vec2 operator/(const vec2 &v1, const vec2 &v2) {
		return vec2(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1]);
	}

	inline vec2 operator*(float t, const vec2 &v) {
		return vec2(t*v.e[0], t*v.e[1]);
	}

	inline vec2 operator/(const vec2& v, float t) {
		return vec2(v.e[0] / t, v.e[1] / t);
	}

	inline vec2 operator*(const vec2 &v, float t) {
		return vec2(t*v.e[0], t*v.e[1]);
	}

	//! dotProduct/scalar product
	inline float dotProduct(const vec2 &v1, const vec2 &v2) {
		return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1];
	}

	//! directed area product product
	inline float areaProduct(const vec2 &v1, const vec2 &v2) {
		return v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0];
	}

	//! returns the normalized vector
	inline vec2 normalize(const vec2& v) {
		return v / v.length();
	}

	inline vec2 pow(const vec2& v, float power)
	{
		return vec2(std::pow(v[0], power), std::pow(v[1], power));
	}

	inline vec2 max(const vec2& v1, const vec2& v2)
	{
		return vec2(std::max(v1[0], v2[0]), std::max(v1[1], v2[1]));
	}

	inline vec2 min(const vec2& v1, const vec2& v2)
	{
		return vec2(std::min(v1[0], v2[0]), std::min(v1[1], v2[1]));
	}

	inline vec2 floor(const vec2& v)
	{
		return vec2(std::floor(v[0]), std::floor(v[1]));
	}

	//! reflects vector v around n
	inline vec2 reflect(const vec2& v, const vec2& n)
	{
		return v - 2 * dotProduct(v, n)*n;
	}

	//! replaces NaN values with 0s
	inline vec2 de_nan(const vec2& c) {
		vec2 temp = c;
		if (!(temp[0] == temp[0])) temp[0] = float(0);
		if (!(temp[1] == temp[1])) temp[1] = float(0);
		return temp;
	}

	//! returns a random vector in the upper disk of the unit circle
	vec2 uniformSampleDisc(const float &r1, const float &r2)
	{

		return vec2(r1, sqrtf(1 - r1*r1));
	}

	/**@}*/
}

#endif
