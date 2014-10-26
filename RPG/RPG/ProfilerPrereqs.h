#pragma once

#ifndef _PROFILERPREREQS_H_
#define _PROFILERPREREQS_H_

#include "ProfilerConfig.h"

namespace Solidus 
{	
#if PROFILER
	struct ProfileEntry;
	struct ProfileZone;

	typedef ProfileEntry* ProfileNodeCache;
	typedef ProfileEntry* ProfileProfileEntryTable;
#endif

#	define FORCE_INLINE		__forceinline
#	define PROFILE_UNUSED

	typedef int					int32_t;
	typedef unsigned int		uint32_t;
	typedef __int64				int64_t;
	typedef unsigned __int64	uint64_t;
	typedef uint64_t			tick_t;

}

#endif
