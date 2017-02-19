#ifndef RAYTR_CORE_FILTER_H
#define RAYTR_CORE_FILTER_H
#include "intensity_array.h"
namespace Raytr_Core
{
	//! an abstract filter class
	class filter
	{
	public:
		virtual void filter_image(const intensity_array& src, intensity_array& dst) const = 0;
	};

	//! a linear noise filter preserving edges
	class noise_filter : public filter
	{
	public:
		void filter_point(const intensity_array& src, intensity_array& dst, int x, int y) const
		{
			int tx = x, ty = y;
			//4*(x,y)
			dst(x, y) += 4 * src(tx, ty);
			//2*(x,y-1)
			ty = std::max(0, y - 1);
			dst(x, y) += 2 * src(tx, ty);
			//2*(x,y+1)
			ty = std::min(src.height - 1, y + 1);
			dst(x, y) += 2 * src(tx, ty);

			//1*(x-1,y+1)
			tx = std::max(0,x - 1);
			dst(x,y)+= src(tx, ty);
			//2*(x-1,y)
			ty = y;
			dst(x, y) += 2*src(tx, ty);
			//1*(x-1,y-1)
			ty = std::max(0, y - 1);
			dst(x, y) += src(tx, ty);
			
			//1*(x+1,y-1)
			tx = std::min(src.width-1, x + 1);
			dst(x, y) += src(tx, ty);
			//2*(x+1,y)
			ty = y;
			dst(x, y) += 2 * src(tx, ty);
			//1*(x+1,y+1)
			ty = std::min(src.height - 1, y + 1);
			dst(x, y) += src(tx, ty);
			
			//divide by 20
			//or by 16?
			dst(x,y) *= 0.0625;
		}

		//! 3by3 low pass filter
		virtual void filter_image(const intensity_array& src, intensity_array& dst) const
		{
			for (int y = 0; y < src.height; ++y)
			{
				for (int x = 0; x < src.width; ++x)
				{
					filter_point(src, dst, x, y);
				}
			}
		}
	};

	//! a median filter
	class median_filter : public filter
	{
	public:
		void filter_point(const intensity_array& src, intensity_array& dst, int x, int y) const
		{
			float arr[3][9];
			int counter = 0;
			for (int i = -1; i <= 1; ++i)
			{
				int ty = BasicMath::clamp(y + i, 0, src.height-1);
				for (int j = -1; j <= 1; ++j)
				{
					int tx = BasicMath::clamp(x + j, 0, src.width-1);
					arr[0][counter] = src(tx, ty).r; arr[1][counter] = src(tx, ty).g; arr[2][counter] = src(tx, ty).b;
					++counter;
				}
			}
			std::sort(arr[0], arr[0] + 9);
			std::sort(arr[1], arr[1] + 9);
			std::sort(arr[2], arr[2] + 9);
			//take the median
			dst(x, y) = BasicMath::vec3(arr[0][4], arr[1][4], arr[2][4]);
		}

		//! 3by3 median filter
		virtual void filter_image(const intensity_array& src, intensity_array& dst) const
		{
			for (int y = 0; y < src.height; ++y)
			{
				for (int x = 0; x < src.width; ++x)
				{
					filter_point(src, dst, x, y);
				}
			}
		}
	};
}

#endif