#ifndef BASICMATH_VEC4_H
#define BASICMATH_VEC4_H
#include "vec3.h"

namespace BasicMath
{

	//! A simple 4d vector class
	class vec4 {


	public:
		vec4() {}
		explicit vec4(float s) { e[0] = s; e[1] = s; e[2] = s; e[3] = s; }
		vec4(float e0, float e1, float e2, float e3) { e[0] = e0; e[1] = e1; e[2] = e2; e[3] = e3; }
		vec4(const vec4& other) { e[0] = other[0]; e[1] = other[1]; e[2] = other[2]; e[3] = other[3]; }
		vec4(const vec3& other, float w) { e[0] = other[0]; e[1] = other[1]; e[2] = other[2]; e[3] = w; }


		//swizzle operations
		inline vec2 xy() const { return vec2(e[0], e[1]); }
		inline vec2 xz() const { return vec2(e[0], e[2]); }
		inline vec2 xw() const { return vec2(e[0], e[3]); }
		inline vec2 yz() const { return vec2(e[1], e[2]); }
		inline vec2 yw() const { return vec2(e[1], e[3]); }
		inline vec2 zw() const { return vec2(e[2], e[3]); }

		inline vec3 xyz() const { return vec3(e[0], e[1], e[2]); }
		inline vec3 yzw() const { return vec3(e[1], e[2], e[3]); }
		inline vec3 xzw() const { return vec3(e[0], e[2], e[3]); }
		inline vec3 xyw() const { return vec3(e[0], e[1], e[3]); }

		inline const vec4& operator+() const { return *this; }
		inline vec4 operator-() const { return vec4(-e[0], -e[1], -e[2], -e[3]); }
		inline float operator[](int i) const { return e[i]; }
		inline float& operator[](int i) { return e[i]; };

		inline vec4& operator+=(const vec4& v2);
		inline vec4& operator-=(const vec4& v2);
		//! componentwise multiplication
		inline vec4& operator*=(const vec4& v2);
		//! componentwise division
		inline vec4& operator/=(const vec4& v2);
		inline vec4& operator*=(const float t);
		inline vec4& operator/=(const float t);

		inline float length() const { return sqrtf(e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3]); }
		inline float squared_length() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3]; }
		//! vector normalization
		inline vec4& make_unit_vector();

		static const vec4 zero;
		static const vec4 one;

		union {
			float e[4];
			struct {
				float x, y, z, w;
			};
			struct
			{
				float r, g, b, a;
			};
		};
	};

	const vec4 vec4::zero(0);
	const vec4 vec4::one(1);

	inline vec4& vec4::operator+=(const vec4 &v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		e[3] += v.e[3];
		return *this;
	}

	inline vec4& vec4::operator*=(const vec4 &v) {
		e[0] *= v.e[0];
		e[1] *= v.e[1];
		e[2] *= v.e[2];
		e[3] *= v.e[3];
		return *this;
	}

	inline vec4& vec4::operator/=(const vec4 &v) {
		e[0] /= v.e[0];
		e[1] /= v.e[1];
		e[2] /= v.e[2];
		e[3] /= v.e[3];
		return *this;
	}

	inline vec4& vec4::operator-=(const vec4& v) {
		e[0] -= v.e[0];
		e[1] -= v.e[1];
		e[2] -= v.e[2];
		e[3] -= v.e[3];
		return *this;
	}

	inline vec4& vec4::operator*=(const float t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		e[3] *= t;
		return *this;
	}

	inline vec4& vec4::operator/=(const float t) {
		float k = float(1.0) / t;

		e[0] *= k;
		e[1] *= k;
		e[2] *= k;
		e[3] *= k;
		return *this;
	}

	inline vec4& vec4::make_unit_vector() {
		float k = float(1.0 / sqrtf(e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3]));
		e[0] *= k; e[1] *= k; e[2] *= k; e[3] *= k;
		return *this;
	}

	/**
	* \defgroup vec4_functions vec4 functions
	* functions for 4d vectors
	* @{
	*/

	inline std::istream& operator >> (std::istream &is, vec4 &t) {
		is >> t.e[0] >> t.e[1] >> t.e[2] >> t.e[3];
		return is;
	}

	inline std::ostream& operator<<(std::ostream &os, const vec4 &t) {
		os << t.e[0] << " " << t.e[1] << " " << t.e[2] << " " << t.e[3];
		return os;
	}

	inline vec4 operator+(const vec4 &v1, const vec4 &v2) {
		return vec4(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2], v1.e[3] + v2.e[3]);
	}

	inline vec4 operator-(const vec4 &v1, const vec4 &v2) {
		return vec4(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2], v1.e[3] - v2.e[3]);
	}

	//! componentwise multiplication
	inline vec4 operator*(const vec4 &v1, const vec4 &v2) {
		return vec4(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2], v1.e[3] * v2.e[3]);
	}

	//! componentwise division
	inline vec4 operator/(const vec4 &v1, const vec4 &v2) {
		return vec4(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2], v1.e[3] / v2.e[3]);
	}

	inline vec4 operator*(float t, const vec4 &v) {
		return vec4(t*v.e[0], t*v.e[1], t*v.e[2], t*v.e[3]);
	}

	inline vec4 operator/(const vec4& v, float t) {
		return vec4(v.e[0] / t, v.e[1] / t, v.e[2] / t, v.e[3] / t);
	}

	inline vec4 operator*(const vec4 &v, float t) {
		return vec4(t*v.e[0], t*v.e[1], t*v.e[2], t*v.e[3]);
	}

	//! dotProduct/scalar product
	inline float dotProduct(const vec4 &v1, const vec4 &v2) {
		return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2] + v1.e[3] * v2.e[3];
	}

	//! cross/vector product for the first 3 components, the 4th is taken from the first vector
	inline vec4 crossProduct(const vec4 &v1, const vec4 &v2) {
		return vec4((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
			(-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
			(v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]),v1.e[3]);
	}

	inline bool operator==(const vec4& v1, const vec4& v2)
	{
		return (v1[0] == v2[0] && v1[1] == v2[1] && v1[2] == v2[2] && v1[3] == v2[3]);
	}

	//! returns the normalized vector
	inline vec4 normalize(const vec4& v) {
		return v / v.length();
	}

	//! removes NaN values from vector
	inline vec4 de_nan(const vec4& c) {
		vec4 temp = c;
		if (!(temp[0] == temp[0])) temp[0] = 0;
		if (!(temp[1] == temp[1])) temp[1] = 0;
		if (!(temp[2] == temp[2])) temp[2] = 0;
		if (!(temp[3] == temp[3])) temp[3] = 0;
		return temp;
	}

	/**@}*/

}

#endif