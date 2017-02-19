#ifndef RAYTR_CORE_TRIANGLE_H
#define RAYTR_CORE_TRIANGLE_H
#include "object.h"
#include "vertex.h"
#include "aabb.h"
#include "Akenine_Moller_triangle_aabb_intersection.h"
namespace Raytr_Core
{
	//! A triangle class to be used in a mesh
	class triangle
	{
	public:
		bounding_volume_aabb boundingBox;
		const vertex* v0;
		const vertex* v1;
		const vertex* v2;
		BasicMath::vec3 e1, e2, normal, nnormal;
		float area;

		triangle() {}

		triangle(const vertex* v0, const vertex* v1, const vertex* v2)
			: v0(v0), v1(v1), v2(v2),
			e1(v1->position - v0->position), e2(v2->position - v0->position), normal(BasicMath::crossProduct(e1, e2))
		{
			boundingBox.addPoint(v0->position); boundingBox.addPoint(v1->position); boundingBox.addPoint(v2->position);
			boundingBox.updateCenterAndHalfSize();
			area = normal.length() / 2;
			nnormal = normal / (2*area);
		}

		bool intersect(const ray& r, float tmin, float tmax, intersection_info& info) const
		{

			float denominator = dotProduct(normal, r.direction);
			if (denominator < -BasicMath::cEPSILON)
			{
				// analytic solution
				BasicMath::vec3 oc = v0->position - r.origin;
				float t = dotProduct(normal, oc);
				if (t > tmin*denominator || t < tmax*denominator)
					return false;

				BasicMath::vec3 ocD = crossProduct(oc, r.direction);
				BasicMath::vec2 ks;
				ks.x = dotProduct(e2, ocD);
				if (ks.x > 0 || ks.x < denominator)
					return false;
				ks.y = -dotProduct(e1, ocD);
				if (ks.y > 0 || ks.x + ks.y<denominator)
					return false;
				float inv_denom = 1.0f / denominator;
				t *= inv_denom;
				ks *= inv_denom;

				//fill out the intersect_info structure
				info.intersect = true;
				info.t = t;
				info.position = r(t);
				//interpolate normals
				//info.normal = normalize((1 - ks.x - ks.y) * v0->normal + ks.x * v1->normal + ks.y * v2->normal);
				//if the interpolated normal is too close to zero, take the original normal
				//if (BasicMath::isnan(info.normal))
				info.normal = nnormal;
				//calculate uv
				info.uv = (1 - ks.x - ks.y) * v0->uv + ks.x * v1->uv + ks.y * v2->uv;
				//remember to fill out pObject in the parent
				return true;
			}
			//else if (denominator > BasicMath::cEPSILON)
			//{
			//	// analytic solution
			//	BasicMath::vec3 oc = v0->position - r.origin;
			//	float t = dotProduct(normal, oc);
			//	if (t < tmin*denominator || t > tmax*denominator)
			//		return false;

			//	BasicMath::vec3 ocD = crossProduct(oc, r.direction);
			//	BasicMath::vec2 ks;
			//	ks.x = dotProduct(e2, ocD);
			//	if (ks.x < 0 || ks.x > denominator)
			//		return false;
			//	ks.y = -dotProduct(e1, ocD);
			//	if (ks.y < 0 || ks.x + ks.y>denominator)
			//		return false;
			//	float inv_denom = 1.0f / denominator;
			//	t *= inv_denom;
			//	ks *= inv_denom;

			//	//fill out the intersect_info structure
			//	info.intersect = true;
			//	info.t = t;
			//	info.position = r(t);
			//	//interpolate normals
			//	//info.normal = normalize((1 - ks.x - ks.y) * v0->normal + ks.x * v1->normal + ks.y * v2->normal);
			//	//if the interpolated normal is too close to zero, take the original normal
			//	//if (BasicMath::isnan(info.normal))
			//	info.normal = nnormal;
			//	//calculate uv
			//	info.uv = (1 - ks.x - ks.y) * v0->uv + ks.x * v1->uv + ks.y * v2->uv;
			//	//remember to fill out pObject in the parent
			//	return true;

			//} //backface culling + 0 case
			else //if (denominator > -BasicMath::cEPSILON)
				return false;
		}

		bool intersectsAABB(const bounding_volume_aabb& aabb) const
		{
			return triBoxOverlap(aabb.center(), aabb.halfSize(), v0->position, v1->position, v2->position, nnormal,
				aabb.min(), aabb.max(), boundingBox.min(), boundingBox.max());
			//return aabb.intersectsAABB(boundingBox);
		}

		//! solid angle subtended by triangle - don't feel like integrating this shit
		virtual float pdf_value(const BasicMath::vec3& o, const BasicMath::vec3& v) const
		{
			return 0;
		}

		//! generate a uniformly random distributed vector towards the triangle - don't feel like integrating this shit
		virtual BasicMath::vec3 random(const BasicMath::vec3& o) const
		{
			return BasicMath::vec3(0);
		}

		virtual float pdf_value_area() const
		{
			//not like this, needs a G term too
			return 1 / area;
		}
		virtual BasicMath::vec3 random_area() const
		{
			float r1 = sqrtf(BasicMath::uniform_distribution(BasicMath::generator));
			float r2 = BasicMath::uniform_distribution(BasicMath::generator);
			float gamma = r1*r2;
			float alpha = r1 - gamma;
			float beta = 1 - r1;

			return alpha*v0->position + beta*v1->position + gamma*v2->position;
		}
	};
}
#endif
