//random distribution sampling
#ifndef BASICMATH_RDS_H
#define BASICMATH_RDS_H
#include "vec3.h"



namespace BasicMath
{
	//! returns a uniformly distributed random point on a sphere(r1,r2 are from U[0,1])
	vec3 uniformSphereSample(float r1, float r2)
	{
		vec3 res;
		res.y = 1 - 2 * r2;
		float phi = 2 * float(M_PI)*r1;
		float sinTheta = 2 * sqrtf(r2*(1 - r2));
		res.x = cosf(phi)*sinTheta;
		res.z = sinf(phi)*sinTheta;
		return res;
	}

	//! returns a uniformly distributed random point on a hempisphere (around (0,1,0))
	vec3 uniformHemisphereSample(float r1, float r2)
	{
		vec3 res;
		res.y = r2;
		float phi = 2 * float(M_PI)*r1;
		float sinTheta = sqrtf(1 - r2*r2);
		res.x = cosf(phi)*sinTheta;
		res.z = sinf(phi)*sinTheta;
		return res;
	}

	//! returns a uniformly distributed random point on a cosine lobe angle thetaMax(r1,r2 are from U[0,1])
	vec3 uniformCosineLobeSample(float r1, float r2, float cosThetaMax)
	{
		vec3 res;
		res.y = 1 + r2*(cosThetaMax - 1);
		float phi = 2 * float(M_PI) * r1;
		float sinTheta = sqrtf(1 - res.y*res.y);
		res.x = cosf(phi)*sinTheta;
		res.z = sinf(phi)*sinTheta;
		return res;
	}

	//! probability distribution function for uniformCosineLobeSample
	float uniformCosineLobePdf(const vec3& direction, float cosThetaMax)
	{
		return 1 / (2 * float(M_PI)*(1 - cosThetaMax));
	}

	//! returns a cosine weighted distributed random point on a hempisphere (around (0,1,0))
	vec3 cosineWeightedHemisphereSample(float r1, float r2)
	{
		vec3 res;
		res.y = r2;
		float phi = 2 * float(M_PI)*r1;
		float sinTheta = sqrtf(1 - r2*r2);
		res.x = cosf(phi)*sinTheta;
		res.z = sinf(phi)*sinTheta;
		return res;
	}

	//! probability distribution function for cosineWeightedDirectionHemisphere
	float cosineWeightedHemispherePdf(const vec3& direction)
	{
		//cos(direction,(0,1,0))/PI
		//return BasicMath::dotProduct(direction, BasicMath::vec3(0, 1, 0)) / float(M_PI);
		return direction.y / float(M_PI);
	}

}

#endif