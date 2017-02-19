#ifndef RAYTR_CORE_OBJECT_H
#define RAYTR_CORE_OBJECT_H

#include "vec2.h"
#include "ray.h"

namespace Raytr_Core
{
	class material;
	class object;

	//! A structure to hold intersection information
	struct intersection_info
	{
		bool intersect;
		float t; //!< the parameter t at which the ray intersects the surface
		BasicMath::vec3 position; //!< coordinates of the intersection point
		BasicMath::vec3 normal; //!< the normal at the intersection point
		BasicMath::vec2 uv; //!< uv texture coordinates
		const object* pObject; //!< a pointer to the object intersected

		intersection_info()
			: intersect(false), t(BasicMath::cINFINITY),
			position(BasicMath::vec3(BasicMath::cINFINITY)), normal(BasicMath::vec3(0)), uv(BasicMath::vec2(0)), pObject(nullptr)
		{

		}

	};
	
	
	//! An abstract object class
	class object
	{
	public:
		object() : pMaterial(nullptr), center(BasicMath::vec3::zero) {}

		bool virtual intersect(const ray& r, float tmin, float tmax, intersection_info& temp) const = 0;
		virtual float pdf_value(const BasicMath::vec3& o, const BasicMath::vec3& v) const { return 0.0f; }
		virtual BasicMath::vec3 random(const BasicMath::vec3& o) const { return BasicMath::vec3(0, 1, 0); }

		virtual float pdf_value_area() const { return 0.0f; }
		virtual BasicMath::vec3 random_area() const { return BasicMath::vec3(0, 1, 0); }

		material* pMaterial;
		BasicMath::vec3 center;
	};

	
}

#endif