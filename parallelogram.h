#ifndef RAYTR_CORE_PARALLELOGRAM_H
#define RAYTR_CORE_PARALLELOGRAM_H
#include "rds.h"
#include "material.h"

namespace Raytr_Core
{

	//! A parallelogram primitive class
	class parallelogram : public object
	{
	public:
		parallelogram(const BasicMath::vec3& o, const BasicMath::vec3& e1, const BasicMath::vec3& e2, material* pMat)
			: e1(e1), e2(e2), normal(BasicMath::crossProduct(e1, e2))
		{
			this->pMaterial = pMat;
			center = o;
			area = normal.length();
			normal /= area;
			//find the components to use when calculating uv
			if (abs(e1.x*e2.y - e1.y*e2.x) > BasicMath::cEPSILON)
			{
				coordsToUse = 1;
			}
			else if(abs(e1.x*e2.z - e1.z*e2.x) > BasicMath::cEPSILON)
			{
				coordsToUse = 2;
			}
			else
			{
				coordsToUse = 3;
			}
		}

		bool intersect(const ray& r, float tmin, float tmax, intersection_info& info) const
		{
			float t;
			// analytic solution
			BasicMath::vec3 oc = center - r.origin;
			float denominator = dotProduct(normal, r.direction);

			if (denominator < BasicMath::cEPSILON && -BasicMath::cEPSILON<denominator)
				return false;

			float numerator = dotProduct(normal, oc);

			t = numerator / denominator;
			if (t < tmin || t>tmax)
			{
				return false;
			}

			BasicMath::vec3 pos = r(t) - center;
			float u,v;
			switch (coordsToUse)
			{
				float det;
			case 1:
				det = e1.x*e2.y - e2.x*e1.y;
				u = (pos.x*e2.y - pos.y*e2.x) / det;
				v = (e1.x*pos.y - pos.x*e1.y) / det;
				break;
			case 2:
				det = e1.x*e2.z - e2.x*e1.z;
				u = (pos.x*e2.z - pos.z*e2.x) / det;
				v = (e1.x*pos.z - pos.x*e1.z) / det;
				break;
			case 3:
				det = e1.y*e2.z - e2.y*e1.z;
				u = (pos.y*e2.z - pos.z*e2.y) / det;
				v = (e1.y*pos.z - pos.y*e1.z) / det;
				break;
			}

			if (u < 0 || u > 1 || 0 > v || v > 1)
				return false;


			//fill out the intersect_info structure
			info.intersect = true;
			info.t = t;
			info.position = r(t);
			info.normal = normal;
			//calculate uv
			info.uv = BasicMath::vec2(u, v);

			info.pObject = this;

			return true;
		}

		//! solid angle subtended by parallelogram - don't feel like integrating this shit
		virtual float pdf_value(const BasicMath::vec3& o, const BasicMath::vec3& v) const
		{
			return 0;
		}

		//! generate a uniformly random distributed vector towards the parallelogram - don't feel like integrating this shit
		virtual BasicMath::vec3 random(const BasicMath::vec3& o) const
		{
			return BasicMath::vec3(0);
		}

		virtual float pdf_value_area() const
		{
			//not like this needs a G term too
			return 1 / area;
		}
		virtual BasicMath::vec3 random_area() const
		{
			return center + e1*BasicMath::uniform_distribution(BasicMath::generator)
				+ e2*BasicMath::uniform_distribution(BasicMath::generator);
		}

		BasicMath::vec3 e1, e2;
		BasicMath::vec3 normal;
		float area;
		int coordsToUse;//1-xy,2-xz,3-yz
	};


}

#endif
