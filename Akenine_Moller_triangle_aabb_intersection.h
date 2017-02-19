/********************************************************/

/* AABB-triangle overlap test code                      */

/* by Tomas Akenine-Möller                              */

/* Function: int triBoxOverlap(float boxcenter[3],      */

/*          float boxhalfsize[3],float triverts[3][3]); */

/* History:                                             */

/*   2001-03-05: released the code in its first version */

/*   2001-06-18: changed the order of the tests, faster */

/*                                                      */

/* Acknowledgement: Many thanks to Pierre Terdiman for  */

/* suggestions and discussions on how to optimize code. */

/* Thanks to David Hunt for finding a ">="-bug!         */

/********************************************************/

//modified version of Tomas Akenine-Möller's routine, to work with my library
#ifndef BASIC_MATH_AKENINE_MOLLER_TRIANGLE_AABB_INTERSECTION_H
#define BASIC_MATH_AKENINE_MOLLER_TRIANGLE_AABB_INTERSECTION_H

#include <math.h>

#include <stdio.h>

#include "vec3.h"

namespace BasicMath
{

	bool planeBoxOverlap(const vec3& normal, const vec3& vert, const vec3& maxbox)	// -NJMP-
	{
		int q;

		vec3 vmin, vmax;

		//iterate over vector components
		for (q = 0; q <= 2; q++)
		{

			float v = vert[q];					// -NJMP-

			if (normal[q] > 0.0f)

			{

				vmin[q] = -maxbox[q] - v;	// -NJMP-

				vmax[q] = maxbox[q] - v;	// -NJMP-

			}

			else

			{

				vmin[q] = maxbox[q] - v;	// -NJMP-

				vmax[q] = -maxbox[q] - v;	// -NJMP-

			}

		}

		if (dotProduct(normal, vmin) > 0.0f) return false;	// -NJMP-

		if (dotProduct(normal, vmax) >= 0.0f) return true;	// -NJMP-



		return false;

	}


	/*======================== X-tests ========================*/

#define AXISTEST_X01(a, b, fa, fb)\
		p0 = a*v0[1] - b*v0[2];\
		p2 = a*v2[1] - b*v2[2];\
		if (p0 < p2) { min = p0; max = p2; }\
		else { min = p2; max = p0; }\
			rad = fa * boxhalfsize[1] + fb * boxhalfsize[2];\
			if (min > rad || max < -rad) return false;



#define AXISTEST_X2(a, b, fa, fb)			   \
	p0 = a*v0[1] - b*v0[2];			           \
		p1 = a*v1[1] - b*v1[2];			       	   \
		if (p0 < p1) { min = p0; max = p1; }\
		else { min = p1; max = p0; } \
			rad = fa * boxhalfsize[1] + fb * boxhalfsize[2];   \
			if (min > rad || max < -rad) return false;



	/*======================== Y-tests ========================*/

#define AXISTEST_Y02(a, b, fa, fb)			   \
	p0 = -a*v0[0] + b*v0[2];		      	   \
		p2 = -a*v2[0] + b*v2[2];	       	       	   \
		if (p0 < p2) { min = p0; max = p2; }\
		else { min = p2; max = p0; } \
			rad = fa * boxhalfsize[0] + fb * boxhalfsize[2];   \
			if (min > rad || max < -rad) return false;



#define AXISTEST_Y1(a, b, fa, fb)			   \
	p0 = -a*v0[0] + b*v0[2];		      	   \
		p1 = -a*v1[0] + b*v1[2];	     	       	   \
		if (p0 < p1) { min = p0; max = p1; }\
		else { min = p1; max = p0; } \
			rad = fa * boxhalfsize[0] + fb * boxhalfsize[2];   \
			if (min > rad || max < -rad) return false;



	/*======================== Z-tests ========================*/



#define AXISTEST_Z12(a, b, fa, fb)			   \
	p1 = a*v1[0] - b*v1[1];			           \
		p2 = a*v2[0] - b*v2[1];			       	   \
		if (p2 < p1) { min = p2; max = p1; }\
		else { min = p1; max = p2; } \
			rad = fa * boxhalfsize[0] + fb * boxhalfsize[1];   \
			if (min > rad || max < -rad) return false;



#define AXISTEST_Z0(a, b, fa, fb)			   \
	p0 = a*v0[0] - b*v0[1];				   \
		p1 = a*v1[0] - b*v1[1];			           \
		if (p0 < p1) { min = p0; max = p1; }\
		else { min = p1; max = p0; } \
			rad = fa * boxhalfsize[0] + fb * boxhalfsize[1];   \
			if (min > rad || max < -rad) return false;


	bool triBoxOverlap(const vec3& boxcenter, const vec3& boxhalfsize, const vec3& trivert0, const vec3& trivert1, const vec3& trivert2, const vec3& trinormal,
		const vec3& aabbMin, const vec3& aabbMax, const vec3& triaabbMin, const vec3& triaabbMax)
	{

		/*    use separating axis theorem to test overlap between triangle and box */

		/*    need to test for overlap in these directions: */

		/*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */

		/*       we do not even need to test these) */

		/*    2) normal of the triangle */

		/*    3) crossproduct(edge from tri, {x,y,z}-directin) */

		/*       this gives 3x3=9 more tests */

		/* move everything so that the boxcenter is in (0,0,0) */
		vec3 v0(trivert0 - boxcenter), v1(trivert1 - boxcenter), v2(trivert2 - boxcenter);

		float min, max, p0, p1, p2, rad, fex, fey, fez;		// -NJMP- "d" local variable removed

		vec3 e0(v1 - v0), e1(v2 - v1), e2(v0 - v2);

		/* Bullet 3:  */

		/*  test the 9 tests first (this was faster) */

		fex = fabsf(e0[0]);

		fey = fabsf(e0[1]);

		fez = fabsf(e0[2]);

		AXISTEST_X01(e0[2], e0[1], fez, fey);

		AXISTEST_Y02(e0[2], e0[0], fez, fex);

		AXISTEST_Z12(e0[1], e0[0], fey, fex);



		fex = fabsf(e1[0]);

		fey = fabsf(e1[1]);

		fez = fabsf(e1[2]);

		AXISTEST_X01(e1[2], e1[1], fez, fey);

		AXISTEST_Y02(e1[2], e1[0], fez, fex);

		AXISTEST_Z0(e1[1], e1[0], fey, fex);



		fex = fabsf(e2[0]);

		fey = fabsf(e2[1]);

		fez = fabsf(e2[2]);

		AXISTEST_X2(e2[2], e2[1], fez, fey);

		AXISTEST_Y1(e2[2], e2[0], fez, fex);

		AXISTEST_Z12(e2[1], e2[0], fey, fex);



		/* Bullet 1: */

		/*  first test overlap in the {x,y,z}-directions */

		/*  find min, max of the triangle each direction, and test for overlap in */

		/*  that direction -- this is equivalent to testing a minimal AABB around */

		/*  the triangle against the AABB */


		//test AABB overlapping with triangle AABB
		if (!(triaabbMin <= aabbMax && triaabbMax >= aabbMin)) return false;


		/* Bullet 2: */

		/*  test if the box intersects the plane of the triangle */

		/*  compute plane equation of triangle: normal*x+d=0 */

		// -NJMP- (line removed here)

		if (!planeBoxOverlap(trinormal, v0, boxhalfsize)) return false;	// -NJMP-

		return true;   /* box and triangle overlaps */

	}

}

#endif

