#ifndef RAYTR_CORE_SCENE_H
#define RAYTR_CORE_SCENE_H
#include <vector>
#include "object.h"

namespace Raytr_Core
{

	//! A scene class
	class scene
	{
	public:
		scene& addObject(object* op)
		{
			objects.push_back(op);
			return *this;
		}


		bool intersect(const ray& r, float tmin, float tmax, intersection_info& info) const
		{
			info.intersect = false;
			float closest_so_far = tmax;
			for (object* iter : objects)
			{
				if (iter->intersect(r, tmin, closest_so_far, info))
				{
					closest_so_far = info.t;
				}
			}
			return info.intersect;
		}

		~scene()
		{
			for (size_t k = 0; k < objects.size(); ++k)
			{
				delete objects[k];
			}
		}

		std::vector<object*> objects;
	};
}

#endif
