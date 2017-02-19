#ifndef BASICMATH_VEC3_H
#define BASICMATH_VEC3_H
#include "util.h"
#include "vec2.h"

namespace BasicMath
{
	//! A simple 3d vector class
	class vec3 {


	public:
		vec3() {}
		explicit vec3(float s) { x = s; y = s; z = s; }
		vec3(float e0, float e1, float e2) { x = e0; y = e1; z = e2; }
		vec3(const vec3& other) { x = other.x; y = other.y; z = other.z; }

		inline vec2 xy() const { return vec2(x, y); }
		inline vec2 xz() const { return vec2(x, z); }
		inline vec2 yz() const { return vec2(y, z); }

		inline const vec3& operator+() const { return *this; }
		inline vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
		inline float operator[](int i) const { return e[i]; }
		inline float& operator[](int i) { return e[i]; };

		inline vec3& operator+=(const vec3& v2);
		inline vec3& operator-=(const vec3& v2);
		//! componentwise multiplication
		inline vec3& operator*=(const vec3& v2);
		//! componentwise division
		inline vec3& operator/=(const vec3& v2);
		inline vec3& operator*=(const float t);
		inline vec3& operator/=(const float t);

		//componentwise less than
		inline bool operator<(const vec3& v2) const;
		//componentwise greater than
		inline bool operator>(const vec3& v2) const;
		//componentwise less than or equal 
		inline bool operator<=(const vec3& v2) const;
		//componentwise greater than or equal 
		inline bool operator>=(const vec3& v2) const;

		bool operator==(const vec3& v2) const;

		bool operator!=(const vec3& v2) const;

		inline float length() const { return sqrtf(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
		inline float squared_length() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
		//! vector normalization
		inline vec3& make_unit_vector();

		static const vec3 zero;
		static const vec3 one;
		static const vec3 epsilon;
		static const vec3 infinity;

		union {
			float e[3];
			struct {
				float x, y, z;
			};
			struct {
				float u, v, w;
			};
			struct {
				float r, g, b;
			};
		};
	};

	const vec3 vec3::zero(0);
	const vec3 vec3::one(1);
	const vec3 vec3::epsilon(cEPSILON);
	const vec3 vec3::infinity(cINFINITY);

	inline vec3& vec3::operator+=(const vec3 &v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	inline vec3& vec3::operator*=(const vec3 &v) {
		e[0] *= v.e[0];
		e[1] *= v.e[1];
		e[2] *= v.e[2];
		return *this;
	}

	inline vec3& vec3::operator/=(const vec3 &v) {
		e[0] /= v.e[0];
		e[1] /= v.e[1];
		e[2] /= v.e[2];
		return *this;
	}

	inline vec3& vec3::operator-=(const vec3& v) {
		e[0] -= v.e[0];
		e[1] -= v.e[1];
		e[2] -= v.e[2];
		return *this;
	}

	inline vec3& vec3::operator*=(const float t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	inline vec3& vec3::operator/=(const float t) {
		float k = float(1.0) / t;

		e[0] *= k;
		e[1] *= k;
		e[2] *= k;
		return *this;
	}

	inline bool vec3::operator<(const vec3& v2) const
	{
		return (x < v2.x && y < v2.y && z < v2.z);
	}

	inline bool vec3::operator>(const vec3& v2) const
	{
		return (x > v2.x && y > v2.y && z > v2.z);
	}

	inline bool vec3::operator<=(const vec3& v2) const
	{
		return (x <= v2.x && y <= v2.y && z <= v2.z);
	}

	inline bool vec3::operator>=(const vec3& v2) const
	{
		return (x >= v2.x && y >= v2.y && z >= v2.z);
	}

	bool vec3::operator==(const vec3& v2) const
	{
		return (e[0] == v2.e[0] && e[1] == v2.e[1] && e[2] == v2.e[2]);
	}

	bool vec3::operator!=(const vec3& v2) const
	{
		return (e[0] != v2.e[0] || e[1] != v2.e[1] || e[2] != v2.e[2]);
	}

	inline vec3& vec3::make_unit_vector()
	{
		float k = float(1.0 / sqrtf(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]));
		e[0] *= k; e[1] *= k; e[2] *= k;
		return *this;
	}


	/**
	* \defgroup vec3_functions vec3 functions
	* functions for 3d vectors
	* @{
	*/

	inline std::istream& operator >> (std::istream &is, vec3 &t) {
		is >> t.e[0] >> t.e[1] >> t.e[2];
		return is;
	}

	inline std::ostream& operator<<(std::ostream &os, const vec3 &t) {
		os << t.e[0] << " " << t.e[1] << " " << t.e[2];
		return os;
	}

	inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
		return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
	}

	inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
		return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
	}

	//! componentwise multiplication
	inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
		return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
	}

	//! componentwise division
	inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
		return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
	}

	inline vec3 operator*(float t, const vec3 &v) {
		return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
	}

	inline vec3 operator/(const vec3& v, float t) {
		return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
	}

	inline vec3 operator/(float t, const vec3& v) {
		return vec3(t/v.e[0], t/v.e[1], t/v.e[2]);
	}

	inline vec3 operator*(const vec3 &v, float t) {
		return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
	}

	//! dotProduct/scalar product
	inline float dotProduct(const vec3 &v1, const vec3 &v2) {
		return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
	}

	//! cross/vector product
	inline vec3 crossProduct(const vec3 &v1, const vec3 &v2) {
		return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
			(-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
			(v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
	}
	
	//! returns the normalized vector
	inline vec3 normalize(const vec3& v) {
		return v / v.length();
	}

	//! checks whether the vector contains nan values
	inline bool isnan(const vec3& v)
	{
		return (std::isnan(v[0]) || std::isnan(v[1]) || std::isnan(v[2]));
	}

	//! componentwise fabs
	inline vec3 fabs(const vec3& v)
	{
		return vec3(std::fabs(v[0]), std::fabs(v[1]), std::fabs(v[2]));
	}

	//! each component of the vector to the power of power
	inline vec3 pow(const vec3& v, float power)
	{
		return vec3(std::pow(v[0], power), std::pow(v[1], power), std::pow(v[2], power));
	}

	//! componentwise max between 2 vectors
	inline vec3 max(const vec3& v1, const vec3& v2)
	{
		return vec3(std::max(v1[0], v2[0]), std::max(v1[1], v2[1]), std::max(v1[2], v2[2]));
	}

	//! return the maximum component of a vector
	inline float max(const vec3& v)
	{
		return std::max(v.x, std::max(v.y, v.z));
	}

	//! componentwise min between 2 vectors
	inline vec3 min(const vec3& v1, const vec3& v2)
	{
		return vec3(std::min(v1[0], v2[0]), std::min(v1[1], v2[1]), std::min(v1[2], v2[2]));
	}

	//! return the minimum component of a vector
	inline float min(const vec3& v)
	{
		return std::min(v[0], std::min(v[1], v[2]));
	}

	//! componentwise clamp a vector between a and b
	inline vec3 clamp(const vec3& v, const vec3& a, const vec3& b)
	{
		return min(max(v,a),b);
	}

	//! reflects vector v around n
	inline vec3 reflect(const vec3& v, const vec3& n)
	{
		return v - 2 * dotProduct(v, n)*n;
	}

	//! removes NaN values from vector
	inline vec3 de_nan(const vec3& c) {
		vec3 temp = c;
		if (!(temp[0] == temp[0])) temp[0] = 0;
		if (!(temp[1] == temp[1])) temp[1] = 0;
		if (!(temp[2] == temp[2])) temp[2] = 0;
		return temp;
	}

	

	//returns the sphere uv coordinates given a point p on a centered at the origin sphere with radius 1
	vec2 get_sphere_uv(const BasicMath::vec3& p)
	{
		float phi = atan2f(p.z, p.x);
		float theta = asinf(p.y);
		return vec2(1 - (phi + M_PI) / (2 * M_PI), (theta + M_PI_2) / M_PI);
	}

	/**@}*/
}

#endif