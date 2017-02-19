#ifndef RAYTR_CORE_AABB_H
#define RAYTR_CORE_AABB_H
#include "vec3.h"
#include "bounding_volume.h"

namespace Raytr_Core
{
	//! An axis aligned bounding box class
	class bounding_volume_aabb : public bounding_volume
	{
	private:
		BasicMath::vec3 _min;//!< box corner with _min coordinates
		BasicMath::vec3 _max;//!< box corner with _max coordinates
		BasicMath::vec3 _center;//!< center of the box
		BasicMath::vec3 _halfSize;//!< half size of the box
	public:
		bounding_volume_aabb(const BasicMath::vec3& _min = BasicMath::vec3::infinity, const BasicMath::vec3& _max = -BasicMath::vec3::infinity) 
			: _min(_min), _max(_max)
		{
			updateCenterAndHalfSize();
		}

		//! get the _min,_max of the bounding box
		const BasicMath::vec3& min() const
		{
			return _min;
		}

		const BasicMath::vec3& max() const
		{
			return _max;
		}

		const BasicMath::vec3& center() const
		{
			return _center;
		}

		const BasicMath::vec3& halfSize() const
		{
			return _halfSize;
		}

		//! adds a point to the aabb and modifies the bounds if necessary
		void addPoint(const BasicMath::vec3& position)
		{
			if (_min.x > position.x)
				_min.x = position.x;
			if (_min.y > position.y)
				_min.y = position.y;
			if (_min.z > position.z)
				_min.z = position.z;

			if (_max.x < position.x)
				_max.x = position.x;
			if (_max.y < position.y)
				_max.y = position.y;
			if (_max.z < position.z)
				_max.z = position.z;
		}

		//! recalculates center and half widths
		void updateCenterAndHalfSize()
		{
			_center = (_min + _max) / 2;
			_halfSize = (_max-_min) / 2;
		}

		//! returns true if the ray intersects the bounding box, returns the intersection points too - t_min, t_max
		bool virtual intersect(const ray& r, float& t_min, float& t_max) const
		{
#define SWAP(x,y) {const float a = x; x = y; y = a;}
			//bounding box defined by _min x,y,z and _max x,y,z
			//if the vector component in that direction is close to 0 
			//X
			if (r.direction.x < BasicMath::cEPSILON && r.direction.x > -BasicMath::cEPSILON)
			{	//if the origin of the ray is not in the region - no intersection
				if (_min.x > r.origin.x || r.origin.x > _max.x)
					return false;
			}
			else
			{
				float invD = 1.0f / r.direction.x;
				float t0 = (_min.x - r.origin.x)*invD;
				float t1 = (_max.x - r.origin.x)*invD;
				if (r.direction.x < 0.0f)
					SWAP(t0, t1);
				t_min = t0 > t_min ? t0 : t_min;
				t_max = t1 < t_max ? t1 : t_max;
				if (t_max < t_min)
					return false;
			}
			//Y
			if (r.direction.y < BasicMath::cEPSILON && r.direction.y > -BasicMath::cEPSILON)
			{	//if the origin of the ray is not in the region - no intersection
				if (_min.y > r.origin.y || r.origin.y > _max.y)
					return false;
			}
			else
			{
				float invD = 1.0f / r.direction.y;
				float t0 = (_min.y - r.origin.y)*invD;
				float t1 = (_max.y - r.origin.y)*invD;
				if (r.direction.y < 0.0f)
					SWAP(t0, t1);
				t_min = t0 > t_min ? t0 : t_min;
				t_max = t1 < t_max ? t1 : t_max;
				if (t_max < t_min)
					return false;
			}

			//Z
			if (r.direction.z < BasicMath::cEPSILON && r.direction.z > -BasicMath::cEPSILON)
			{	//if the origin of the ray is not in the region - no intersection
				if (_min.z > r.origin.z || r.origin.z > _max.z)
					return false;
			}
			else
			{
				float invD = 1.0f / r.direction.z;
				float t0 = (_min.z - r.origin.z)*invD;
				float t1 = (_max.z - r.origin.z)*invD;
				if (r.direction.z < 0.0f)
					SWAP(t0, t1);
				t_min = t0 > t_min ? t0 : t_min;
				t_max = t1 < t_max ? t1 : t_max;
				if (t_max < t_min)
					return false;
			}
			return true;
#undef SWAP
		}

		//! returns true if p is inside the aabb
		inline bool isPointInside(const BasicMath::vec3& p) const
		{
			return (_min <= p && p <= _max);
		}

		//! returns true if the edge (p0,p1) intersects the aabb
		inline bool isEdgeInside(const BasicMath::vec3& p0, const BasicMath::vec3& p1) const
		{
			float t_min = 0;
			BasicMath::vec3 dir = p1 - p0;
			float t_max = 1;
			//bounding box defined by _min x,y,z and _max x,y,z
			for (int a = 0; a < 3; a++)
			{
				//if the vector component in that direction is close to 0
				if (fabs(dir[a]) < BasicMath::cEPSILON)
					//if the origin of the ray is not in the region - no intersection
					if (_min[a] > p0[a] || p0[a] > _max[a])
						return false;

				float invD = 1.0f / dir[a];
				float t0 = (_min[a] - p0[a]) * invD;
				float t1 = (_max[a] - p0[a]) * invD;
				if (invD < 0.0f)
					std::swap(t0, t1);
				t_min = t0 > t_min ? t0 : t_min;
				t_max = t1 < t_max ? t1 : t_max;
				if (t_max < t_min)
					return false;
			}
			return true;
		}

		//! intersection between two bounding boxes
		inline bool intersectsAABB(const bounding_volume_aabb& other) const
		{
			return (_max >= other.min() && _min <= other.max());
		}
	};
}

#endif