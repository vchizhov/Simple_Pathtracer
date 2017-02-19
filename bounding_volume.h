#ifndef RAYTR_CORE_BOUNDING_VOLUME_H
#define RAYTR_CORE_BOUNDING_VOLUME_H
#include "ray.h"
namespace Raytr_Core
{
	//! an abstract bounding volume class
	class bounding_volume
	{
	public:
		bool virtual intersect(const ray& r, float& tmin, float& tmax) const = 0;
	};

	//! a dummy bounding volume class
	class bounding_volume_none : public bounding_volume
	{
	public:
		bool virtual intersect(const ray& r, float& tmin, float& tmax) const
		{
			return true;
		}
	};

	const bounding_volume_none cDefaultboundingVolume;
}

#endif
