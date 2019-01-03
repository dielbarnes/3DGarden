//
// Timer.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#include "Timer.h"

Timer::Timer()
{
	__int64 countsPerSecond = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);

	m_fSecondsPerCount = 1.0f / (float)countsPerSecond;
	m_fTicksPerMs = (float)(countsPerSecond / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_previousTime);
}

Timer::~Timer()
{
}

void Timer::Update()
{
	__int64 currentTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	m_fDeltaTime = (currentTime - m_previousTime) * m_fSecondsPerCount;
	m_fFrameTime = (currentTime - m_previousTime) / m_fTicksPerMs;
	m_previousTime = currentTime;
}

float Timer::GetDeltaTime()
{
	return m_fDeltaTime;
}

float Timer::GetFrameTime()
{
	return m_fFrameTime;
}
