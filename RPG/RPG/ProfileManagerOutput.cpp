#include "stdafx.h"
#include "ProfileManagerOutput.h"

#include <stdio.h>

#if PROFILE_COMPILER == PROFILE_COMPILER_MSVC
#	pragma warning(disable: 4996)
#	define snprintf		_snprintf
#	define TRAILING		0

#else
#	define TRAILING		1
#endif

#if PROFILER == TRUE
namespace Solidus {


//-----------------------------------------------------------------------------

	void _printHeader(char *dest, const char *a_title) {
		snprintf(dest, OUTPUT_WIDTH_SUM + TRAILING,
			"%-*s %*s %*s %*s",
			OUTPUT_WIDTH_NAME, a_title,
			OUTPUT_WIDTH_HIT, "hits",
			OUTPUT_WIDTH_TIME+4+OUTPUT_WIDTH_PERC+1, "self time",
			OUTPUT_WIDTH_TIME+4+OUTPUT_WIDTH_PERC+1, "total time");
	}


//-----------------------------------------------------------------------------

	void _printData( char *dest, const ProfileData &a_data, float a_topercent ) 
	{
		float totalTicksAvg = a_data.TotalTicksAvg();
		const TimeUnit *selfUnit = GetTimeUnit(a_data.SelfTicks.Average);
		const TimeUnit *totalUnit = GetTimeUnit(totalTicksAvg);

		snprintf(dest, OUTPUT_WIDTH_DATA + TRAILING,
			" %*.1f %*.0f %-2s %*.0f%% %*.0f %-2s %*.0f%%",
			OUTPUT_WIDTH_HIT, a_data.EntryCount.Average,
			OUTPUT_WIDTH_TIME, a_data.SelfTicks.Average * selfUnit->invTickFreq, selfUnit->suffix,
			OUTPUT_WIDTH_PERC, a_data.SelfTicks.Average * a_topercent,
			OUTPUT_WIDTH_TIME, totalTicksAvg * totalUnit->invTickFreq, totalUnit->suffix,
			OUTPUT_WIDTH_PERC, totalTicksAvg * a_topercent);
	}

//-----------------------------------------------------------------------------

	std::string OutputNodesAsString( const ProfileEntry *a_root, uint32_t a_count ) 
	{
		float fTicksToPc = 100.0f / a_root->Data.ChildTicks.Average;
		std::string str;

		str.resize((1 + a_count) * (OUTPUT_WIDTH_SUM + 1) - 1);
		char *s = &str[0];

		_printHeader(s, "call tree");
		s += OUTPUT_WIDTH_SUM;
		(*s++) = '\n';

		const ProfileEntry *node = a_root;

		do {
			int offset = node->EntryLevel * 2;
			snprintf(s, OUTPUT_WIDTH_NAME + TRAILING, "%*s%-*s",
				offset, "", OUTPUT_WIDTH_NAME - offset, node->Zone->name);

			s += OUTPUT_WIDTH_NAME;

			_printData(s, node->Data, fTicksToPc);

			s += OUTPUT_WIDTH_DATA;
			(*s++) = '\n';

			node = node->FindNextInTree();
		} while (node);

		*(--s) = '\0';
		return str;
	}


//-----------------------------------------------------------------------------

	std::string OutputZonesAsString( const ProfileZone *a_root, uint32_t a_count ) 
	{
		float fTicksToPc = 100.0f / a_root->data.ChildTicks.Average;
		std::string str;

		str.resize((1 + a_count) * (OUTPUT_WIDTH_SUM + 1) - 1);
		char *s = &str[0];

		_printHeader(s, "flat profile");
		s += OUTPUT_WIDTH_SUM;
		(*s++) = '\n';

		const ProfileZone *zone = a_root;

		do {
			snprintf(s, OUTPUT_WIDTH_NAME + TRAILING, "%-*s",
				OUTPUT_WIDTH_NAME, zone->name);

			s += OUTPUT_WIDTH_NAME;

			_printData(s, zone->data, fTicksToPc);

			s += OUTPUT_WIDTH_DATA;
			(*s++) = '\n';

			zone = zone->next;
		} while (zone);

		*(--s) = '\0';
		return str;
	}

} // namespace Solidus
#endif
