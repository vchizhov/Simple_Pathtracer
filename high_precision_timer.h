#ifndef HIGH_PRECISION_TIMER_H
#define HIGH_PRECISION_TIMER_H

#define NOMINMAX	 //suppres windows min max
#include <Windows.h> //precise timing
#include <iostream>

//! a minimal implementation of a high precision time calss
class HighPrecisionTimer
{
private:
	double PCFreq = 0.0;
	__int64 CounterStart = 0;
public:

	void StartCounter()
	{
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
			std::cout << "QueryPerformanceFrequency failed!\n";

		PCFreq = double(li.QuadPart);

		QueryPerformanceCounter(&li);
		CounterStart = li.QuadPart;
	}

	double GetCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart - CounterStart) / PCFreq;
	}
};

#endif