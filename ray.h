#ifndef RAYTR_CORE_RAY_H
#define RAYTR_CORE_RAY_H

#include "mat3.h"

namespace Raytr_Core
{
	//! A basic ray class
	class ray
	{
	public:
		ray(const BasicMath::vec3& ro, const BasicMath::vec3& rd) : origin(ro), direction(rd) {}

		//! returns a point on the ray based on parameter t
		BasicMath::vec3 operator()(float t) const { return origin + direction*t; }

		//! tranforms the ray direction with a matrix
		ray& transform(const BasicMath::mat3& m)
		{
			direction = m*direction;
			return *this;
		}

		BasicMath::vec3 origin; //!< Ray origin
		BasicMath::vec3 direction; //!< Ray direction
	};
}

#endif
