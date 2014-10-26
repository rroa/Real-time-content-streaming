#pragma once

#ifndef _PROFILEMANAGEROUTPUT_H_
#define _PROFILEMANAGEROUTPUT_H_

#include "ProfileEntry.h"
#include "ProfileZone.h"

#include <string>

#if PROFILER
namespace Solidus 
{
	enum OUTPUT_WIDTH 
	{
		OUTPUT_WIDTH_HIT  = 6,
		OUTPUT_WIDTH_TIME = 6,
		OUTPUT_WIDTH_PERC = 4,
		OUTPUT_WIDTH_SUM  = 79,

		OUTPUT_WIDTH_DATA = 1 + OUTPUT_WIDTH_HIT + 1 + 2 * ( OUTPUT_WIDTH_TIME + 4 + OUTPUT_WIDTH_PERC + 1 ) + 1,
		OUTPUT_WIDTH_NAME = OUTPUT_WIDTH_SUM - OUTPUT_WIDTH_DATA
	};


//-----------------------------------------------------------------------------

	std::string OutputNodesAsString( const ProfileEntry *a_root, uint32_t a_count );
	std::string OutputZonesAsString( const ProfileZone *a_root, uint32_t a_count );

}
#endif

#endif
