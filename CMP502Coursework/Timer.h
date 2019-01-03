//
// Timer.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#ifndef TIMERCLASS_H
#define TIMERCLASS_H

#include <windows.h>

class Timer
{
public:
	Timer();
	~Timer();

	void Update();
	float GetDeltaTime();
	float GetFrameTime();

private:
	float m_fSecondsPerCount;
	float m_fTicksPerMs;
	__int64 m_previousTime;
	float m_fDeltaTime;
	float m_fFrameTime;
};

#endif
