#include "Engine/Utility/Timer.h"

rv::Timer::Timer()
	:
	last(now())
{
}

rv::Duration rv::Timer::Mark()
{
	auto temp = last;
	last = now();
	return Duration(last - temp);
}

rv::Duration rv::Timer::Peek() const
{
	return Duration(now() - last);
}

void rv::Timer::Reset()
{
	last = now();
}

std::chrono::high_resolution_clock::time_point rv::Timer::now()
{
	return std::chrono::high_resolution_clock::now();
}
