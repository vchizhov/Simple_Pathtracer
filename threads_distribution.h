#ifndef THREADS_DISTRIBUTION_H
#define THREADS_DISTRIBUTION_H
#include <thread>
#include <vector>

namespace ThreadsDistribution
{
	struct point
	{
		int x,y;
		point() {}
		point(int x, int y): x(x), y(y) {}
	};

	struct rectangle
	{
		point pos;
		point size;
		rectangle()
		{}
		rectangle(float x, float y, float width, float height)
			:pos(x,y), size(width,height)
		{

		}
		rectangle(point pos, point size)
			:pos(pos), size(size)
		{

		}

	};

	class rectangles
	{
	private:
		size_t currentRect;
		point pos;
		bool done;
	public:
		std::vector<rectangle> rects;

		bool init()
		{
			done = false;
			if (rects.empty())
				return !(done = true);

			currentRect = 0;
			pos = rects[currentRect].pos;
			return done;
		}

		bool nextPoint(point& pin)
		{

			if (done)
				return false;

			pin = pos;

			//get next
			++pos.x;
			if (pos.x >= rects[currentRect].pos.x + rects[currentRect].size.x)
			{
				pos.x = rects[currentRect].pos.x;
				++pos.y;
			}
			if (pos.y >= rects[currentRect].pos.y + rects[currentRect].size.y)
			{
				++currentRect;
				if (currentRect == rects.size())
					return !(done = true);

				pos = rects[currentRect].pos;
			}

			return true;
		}

	};

	//! divides a bigger rectangle into smaller pieces
	std::vector<rectangles> populateWithRectangles(const rectangle& area, const point& rectSize, int numThreads)
	{
		std::vector<rectangles> result(numThreads);
		int currentThread = -1;
		point currentPos = area.pos;

		while (currentPos.y < area.pos.y + area.size.y)
		{
			//iterate over the threads
			++currentThread;
			currentThread = currentThread%numThreads;

			//at each iteration add a new rectangle moving along x
			result[currentThread].rects.push_back(rectangle(currentPos,rectSize));
			currentPos.x += rectSize.x;

			//fix y
			if (currentPos.y + rectSize.y >= area.pos.y + area.size.y)
			{
				result[currentThread].rects.back().size.y -= currentPos.y + rectSize.y - area.pos.x - area.size.x;
			}

			//if the new rectangle is out of bounds
			if (currentPos.x >= area.pos.x + area.size.x)
			{
				result[currentThread].rects.back().size.x -= currentPos.x - area.pos.x - area.size.x;

				//wrap x and increment y
				currentPos.x = area.pos.x;
				currentPos.y += rectSize.y;
			}
		}

		return result;
	}
}

#endif