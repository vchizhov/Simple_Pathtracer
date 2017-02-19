#ifndef RAYTR_CORE_PLANE_H
#define RAYTR_CORE_PLANE_H
#include "material.h"

namespace Raytr_Core
{


	//! A plane primitive class
	class plane : public object
	{
	public:
		plane(const BasicMath::vec3& o, BasicMath::vec3 t, BasicMath::vec3 b, material* pMat) 
			: center(o), normal(normalize(crossProduct(t,b))), tangent(normalize(t)), bitangent(normalize(b)), 
			tLength(t.length()), bLength(b.length())
		{
			this->pMaterial = pMat;
		}

		bool intersect(const ray& r, float tmin, float tmax, intersection_info& info) const
		{
			float t;
			// analytic solution
			BasicMath::vec3 oc = center-r.origin;
			float denominator = dotProduct(normal, r.direction);

			if (fabs(denominator) < BasicMath::cEPSILON)
				return false;

			float numerator = dotProduct(normal, oc);

			t = numerator / denominator;
			if (t < tmin || t>tmax)
			{
				return false;
			}
			//fill out the intersect_info structure
			info.intersect = true;
			info.t = t;
			info.position = r(t);
			info.normal = normal;
			//calculate uv
			info.uv = BasicMath::vec2(dotProduct(bitangent, info.position-center)/bLength,dotProduct(tangent, info.position - center)/tLength);
			info.uv -= floor(info.uv);
			if (info.uv.u < 0)
				info.uv.u = 1 - info.uv.u;
			else if (info.uv.v < 0)
				info.uv.v = 1 - info.uv.v;

			info.pObject = this;

			return true;
		}

		BasicMath::vec3 center; //!< plane offset
		BasicMath::vec3 normal; //!< normal vector of the plane
		BasicMath::vec3 tangent; //!< tangent vector of the plane - used to determine uv coords
		float tLength; //!< original tangent length
		BasicMath::vec3 bitangent;//!< bitangent vector
		float bLength; //!< original bitangent length
	};
}

#endif