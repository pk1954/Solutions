// win32_actionTimer.h 
//
// win32_utilities

#pragma once

#include <chrono>
#include "minwindef.h"
#include "MoreTypes.h"
#include "observable.h"
#include "win32_hiResTimer.h"

import Util;

using std::chrono::milliseconds;

class ActionTimer : public Observable
{
public:
	ActionTimer()
	{
		m_hrtimerOverall.Start();
	}

	void TimerStart()
	{
		m_hrtimerSingleAction.Start();
	};

	void TimerStop()
	{
		m_hrtimerSingleAction.Stop();
		m_usSingleActionTime = m_hrtimerSingleAction.GetDuration();
		++m_dwCounter;
		NotifyAll(false);
	};

	microseconds GetSingleActionTime() const
	{
		return m_usSingleActionTime;
	}

	Hertz CalcActionFrequency(microseconds us, DWORD dwCount = 1) const
	{
		static unsigned long long MICROSECONDS_TO_HERTZ_FACTOR = microseconds::period::den;
		if (us == microseconds::zero())
			return  0_Hertz;

		assert(dwCount < ULLONG_MAX / MICROSECONDS_TO_HERTZ_FACTOR);    // avoid ull overflow
		unsigned long long ullFrequency = (dwCount * MICROSECONDS_TO_HERTZ_FACTOR) / us.count();
		return Hertz(Cast2Long(ullFrequency));
	}

	Hertz GetMeasuredPerformance()
	{
		m_hrtimerOverall.Stop();
		microseconds usOverallTime = m_hrtimerOverall.GetDuration();
		Hertz        result        = CalcActionFrequency(usOverallTime, m_dwCounter);
		m_dwCounter = 0;
		m_hrtimerOverall.Restart();
		return result;
	}

private:

	HiResTimer   m_hrtimerSingleAction { };
	microseconds m_usSingleActionTime  { };
	HiResTimer   m_hrtimerOverall      { };
	DWORD        m_dwCounter           { 0 };   // nr of executions
};
