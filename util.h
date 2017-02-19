#ifndef BASICMATH_UTIL_H
#define BASICMATH_UTIL_H

//so I can use M_PI and other constants from math.h
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <iostream>
#include <limits>
#include <random>

namespace BasicMath
{

	const float		cINFINITY = std::numeric_limits<float>::max();
	const float		cEPSILON = 0.000001f;
	const size_t	cFLOATBYTES = sizeof(float);
	const size_t	cBOOLBYTES = sizeof(bool);

	//! finds the roots of the equation: a*x^2+b*y+c = 0
	bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
	{
		float discr = b * b - 4 * a * c;
		if (discr < 0) return false;
		else if (discr == 0) {
			x0 = x1 = float(-0.5) * b / a;
		}
		else {
			float q = (b > 0) ?
				float(-0.5) * (b + sqrtf(discr)) :
				float(-0.5) * (b - sqrtf(discr));
			x0 = q / a;
			x1 = c / q;
		}

		return true;
	}

	//! converts degrees to radians
	inline float deg2rad(float deg)
	{
		return deg*float(M_PI / 180);
	}
	//! converts radians to degrees
	inline float rad2deg(float rad)
	{
		return rad * float(180 / M_PI);
	}

	//! if a<x<b returns x, if x<a returns a, if x>b returns b
	inline float clamp(float x, float a, float b)
	{
		return std::max(a,std::min(x, b));
	}

	std::default_random_engine generator;
	std::uniform_real_distribution<float> uniform_distribution(0, 1);
}

#endif
