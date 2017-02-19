#ifndef RAYTR_CORE_CAMERA_H
#define RAYTR_CORE_CAMERA_H
#include "ray.h"

namespace Raytr_Core
{
	//! A simple camera class
	class camera
	{
	public:

		//! build a camera from origin, a target lcation and an up vector
		camera(const BasicMath::vec3& pos = BasicMath::vec3(0), const BasicMath::vec3& target=BasicMath::vec3(0,0,1), BasicMath::vec3& up= BasicMath::vec3(0,1,0),
			float ar=1, float fov=90)
			: position(pos), aspectRatio(ar)
		{
			BasicMath::vec3 z = normalize(target - pos);
			BasicMath::vec3 x = normalize(crossProduct(up, z));
			BasicMath::vec3 y = crossProduct(z, x);
			orientation = BasicMath::matrixFromColumns(x, y, z);
			u = aspectRatio*x;
			v = y;
			w = aspectRatio / std::atan(BasicMath::deg2rad(fov/2))*z;
		}

		//! returns a ray based on xy coordinates along the local x and y axes
		ray getRay(float x, float y) const
		{
			return ray(position, normalize(x*u + y*v + w));
		}

		BasicMath::vec3 position;
		BasicMath::mat3 orientation;
		float aspectRatio;

	private:
		BasicMath::vec3 u;
		BasicMath::vec3 v;
		BasicMath::vec3 w;
		
	};
}

#endif