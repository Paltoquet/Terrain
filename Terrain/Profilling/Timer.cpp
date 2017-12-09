#include "stdafx.h"
#include "Timer.h"


Timer::Timer()
{
}


Timer::Timer(const Timer& other)
{
}

bool Timer::Initialize()
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_Frequency);
	if (m_Frequency.QuadPart == 0)
	{
		return false;
	}

	// Find out how many times the frequency counter ticks every millisecond.
	m_TickPerMs = (float)(m_Frequency.QuadPart / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_StartTime);

	return true;	
}

void Timer::Frame()
{
	INT64 currentTime;
	float timeDifference;
	double elapsed;

	QueryPerformanceCounter(&m_CurrentTime);
	elapsed = (float)(m_CurrentTime.QuadPart - m_StartTime.QuadPart) / m_TickPerMs;
	m_FrameTime = elapsed;

	QueryPerformanceCounter(&m_StartTime);

	return;
}

float Timer::GetTime()
{
	return m_FrameTime;
}

Timer::~Timer()
{
}
