#pragma once

#include <windows.h>

class Timer
{
public:
	Timer();
	Timer(const Timer&);
	~Timer();

	bool Initialize();
	void Frame();

	float GetTime();

private:

	LARGE_INTEGER m_Frequency;
	LARGE_INTEGER m_StartTime;
	LARGE_INTEGER m_CurrentTime;

	float m_TickPerMs;
	float m_FrameTime;

};

