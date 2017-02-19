#ifndef RAYTR_CORE_SPHERE_H
#define RAYTR_CORE_SPHERE_H
#include "rds.h"
#include "material.h"

namespace Raytr_Core
{
	

	//! A sphere primitive class
	class sphere : public object
	{
	public:
		sphere(const BasicMath::vec3& o, float r, material* pMat) : radius(r), radius2(r*r)
		{
			this->pMaterial = pMat;
			center = o;
			area = 4 * float(M_PI)*radius2;
		}

		bool intersect(const ray& r, float tmin, float tmax, intersection_info& info) const
		{
			// analytic solution
			// center to origin vector
			BasicMath::vec3 co = r.origin - center;
			float a = dotProduct(r.direction,r.direction);
			float b = dotProduct(r.direction,co);
			float c = dotProduct(co,co) - radius2;
			float discriminant = b*b - a*c;

			if (discriminant < 0) return false;


			float solution = (-b - sqrtf(discriminant)) / a;
			if (solution > tmax || solution < tmin)
				solution = (-b + sqrtf(discriminant)) / a;
			if (solution > tmax || solution < tmin)
				return false;


			//fill out the intersect_info structure
			info.intersect = true;
			info.t = solution;
			info.position = r(solution);
			info.normal = (info.position-center)/radius;
			info.uv = get_sphere_uv(info.normal);
			info.pObject = this;

			return true;
		}

		//! solid angle subtended by sphere
		virtual float pdf_value(const BasicMath::vec3& o, const BasicMath::vec3& v) const
		{ 
			float cosThetaMax = sqrtf(1 - radius2 / (center - o).squared_length());
			return BasicMath::uniformCosineLobePdf(v, cosThetaMax);
		}

		//! generate a uniformly random distributed vector towards the sphere from point o
		virtual BasicMath::vec3 random(const BasicMath::vec3& o) const
		{ 
			BasicMath::vec3 direction = center - o;
			float distance_squared = direction.squared_length();
			float r1 = BasicMath::uniform_distribution(BasicMath::generator);
			float r2 = BasicMath::uniform_distribution(BasicMath::generator);
			float cosThetaMax = sqrtf(std::max(0.f,1 - radius2 / distance_squared));
			return BasicMath::uniformCosineLobeSample(r1, r2, cosThetaMax);
		}

		virtual float pdf_value_area() const
		{
			//not like this, needs a G term too
			return 1 / area;
		}
		virtual BasicMath::vec3 random_area() const
		{
			return center+radius*BasicMath::uniformSphereSample(
				BasicMath::uniform_distribution(BasicMath::generator), 
				BasicMath::uniform_distribution(BasicMath::generator));
		}

		float radius; //<! sphere radius
		float radius2; //<! sphere radius^2
		float area;
	};


}

#endif