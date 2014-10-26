#pragma once

#ifndef _PROFILEDATA_H_
#define _PROFILEDATA_H_

#include "ProfilerPrereqs.h"

namespace Solidus 
{
	struct ProfileLastData 
	{
		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */

		uint32_t EntryCount;
		tick_t	 SelfTicks;
	};	

	struct ProfileData 
	{
		/* =============================================================
		 * HELPER STRUCTS
		 * ============================================================= */

		template <typename T>
		struct Data 
		{
			/* =============================================================
			 * FUNCTIONS
			 * ============================================================= */

			void CalculateAverage( float a_damping ) { Average = a_damping * ( Average - Current ) + Current; }
			void Clear() { Current = 0; Average = 0; }

			/* =============================================================
			 * MEMBERS
			 * ============================================================= */

			T Current;
			float Average;			
		};				

		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */

		void CalculateAverage( float a_damping ) 
		{
			EntryCount.CalculateAverage( a_damping );
			SelfTicks.CalculateAverage( a_damping );
			ChildTicks.CalculateAverage( a_damping );
		}

		void ClearAll() 
		{
			EntryCount.Clear();
			SelfTicks.Clear();
			ChildTicks.Clear();
		}

		void ClearCurrent() 
		{
			EntryCount.Current = 0;
			SelfTicks.Current  = 0;
			ChildTicks.Current = 0;
		}

		/* =============================================================
		 * INLINE FUNCTIONS
		 * ============================================================= */

		inline tick_t TotalTicksCurrent() const { return SelfTicks.Current + ChildTicks.Current; }
		inline float  TotalTicksAvg() const { return SelfTicks.Average + ChildTicks.Average; }

		/* =============================================================
		 * MEMBERS
		 * ============================================================= */

		Data< uint32_t > EntryCount;
		Data< tick_t >   SelfTicks;
		Data< tick_t >   ChildTicks;
	};
}

#endif
