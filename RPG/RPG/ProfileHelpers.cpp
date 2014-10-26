#pragma once

#include "stdafx.h"
#include "ProfileHelpers.h"

#if PROFILE_PLATFORM == PROFILE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif PROFILE_PLATFORM == PROFILE_PLATFORM_POSIX
#include <sys/time.h>
#endif

namespace Solidus {


//-----------------------------------------------------------------------------

	const TimeUnit* GetTimeUnit(float ticks) {
		static TimeUnit unit[] = {
			{ GetTickFreq() / 1.0f			, GetTickInvFreq() * 1.0f			, "s" },
			{ GetTickFreq() / 1000.0f		, GetTickInvFreq() * 1000.0f		, "ms" },
			{ GetTickFreq() / 1000000.0f	, GetTickInvFreq() * 1000000.0f		, "us" },
			{ GetTickFreq() / 1000000000.0f	, GetTickInvFreq() * 1000000000.0f	, "ns" }
		};

		if (unit[0].tickFreq < ticks) return &unit[0];
		else if (unit[1].tickFreq < ticks) return &unit[1];
		else if (unit[2].tickFreq < ticks) return &unit[2];
		else return &unit[3];
	}


//-----------------------------------------------------------------------------

#if PROFILE_PLATFORM == PROFILE_PLATFORM_WIN32

	tick_t _InitTickFreq(void) {
		tick_t freq;

		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&freq));
		return freq;
	}

	void GetTicks(tick_t *p) {
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(p));
	}

	tick_t GetTickFreq(void) {
		static tick_t freq = _InitTickFreq();
		return freq;
	}

	float GetTickInvFreq(void) {
		static float invfreq = 1.0f / GetTickFreq();
		return invfreq;
	}


//-----------------------------------------------------------------------------

#elif PROFILE_PLATFORM == PROFILE_PLATFORM_POSIX

	void GetTicks(tick_t *p) {
		timeval time;
		gettimeofday(&time, NULL);

		*p = time.tv_sec * 1000000 + time.tv_usec;
	}

	tick_t GetTickFreq(void) {
		return 1000000;
	}

	float GetTickInvFreq(void) {
		return 1.0f / 1000000.0f;
	}

#endif
} // namespace Solidus
