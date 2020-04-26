#ifndef _FPS_H_
#define _FPS_H_
#include <string>
#include <sstream>

class FpsHelper
{
public:
	FpsHelper(double frameStarTime)
		:lastFrameTime(frameStarTime), frames(0) {}
	double getMilliSecondsPerFrame()
	{
		return 1000.0 / frames;
	}
	int getFramePerSeconds()
	{
		return frames;
	}
	std::wstring getFramePerSecondsAsString()
	{
		std::wstringstream ss;
		ss << "FPS: " << frames;
		return ss.str();
	}
public:
	void resetCounter(double frameTime, int framesCount = 0)
	{
		this->lastFrameTime = frameTime;
		this->frames = framesCount;
	}
public:
	double lastFrameTime;
	int frames;

};
#endif // !_FPS_H_

