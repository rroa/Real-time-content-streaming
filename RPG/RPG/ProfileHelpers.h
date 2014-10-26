#pragma once

#ifndef _PROFILEHELPERS_H_
#define _PROFILEHELPERS_H_

#include "ProfilerPrereqs.h"

namespace Solidus 
{
	/* =============================================================
	 * HELPER STRUCTS
	 * ============================================================= */

	struct TimeUnit 
	{
		float tickFreq;
		float invTickFreq;
		const char* suffix;
	};

	/* =============================================================
	 * FUNCTIONS
	 * ============================================================= */

	const TimeUnit* GetTimeUnit(float ticks);
	void			GetTicks(tick_t *p);
	tick_t			GetTickFreq(void);
	float			GetTickInvFreq(void);


#if PROFILE_COMPILER == PROFILE_COMPILER_MSVC
#	pragma warning(disable: 4311)
#endif

	inline uint32_t ptr32(const void *a_Ptr) 
	{
		return reinterpret_cast<uint32_t>(a_Ptr);
	}

#if PROFILE_COMPILER == PROFILE_COMPILER_MSVC
#	pragma warning(default: 4311)
#endif


}

#endif
