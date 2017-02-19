#ifndef RAYTR_CORE_INTENSITY_ARRAY_H
#define RAYTR_CORE_INTENSITY_ARRAY_H
#include "vec3.h"
#include <cstring> //memset
namespace Raytr_Core
{
	//! an array to hold color intensities with float precision for all 3 channels
	class intensity_array
	{
	public:

		BasicMath::vec3* arr;
		int width, height;

		intensity_array(int width, int height) : width(width), height(height)
		{
			arr = new BasicMath::vec3[width*height];
			memset(arr, 0, 3 * width * height * BasicMath::cFLOATBYTES);
		}

		//! adds an array to the current array, size must match
		intensity_array& operator+=(const intensity_array& other)
		{
			for (int i = 0; i < width*height; ++i)
			{
				arr[i]+=other.arr[i];
			}
			return *this;
		}

		//! easy access as a 2dimensional array
		const BasicMath::vec3& operator()(int i, int j) const
		{
			return arr[j*width + i];
		}

		//! easy access as a 2dimensional array
		BasicMath::vec3& operator()(int i, int j)
		{
			return arr[j*width + i];
		}


		~intensity_array()
		{
			delete[] arr;
		}
		
	};

}

#endif