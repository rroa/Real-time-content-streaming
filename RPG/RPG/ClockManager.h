#pragma once

#ifndef _CLOCK_MANAGER_H_
#define _CLOCK_MANAGER_H_

namespace Solidus
{
	class Clock;
	class ClockManager
	{
	public:
		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */

		Clock*								AddClock( float timeScale = 1.0f );
		void								AdvanceTime( double deltaSeconds );		
		bool								RemoveClock( Clock* clockToDelete );
		static void							Create();
		static void							Destroy();		
		const double						ElapsedTime() const;
		static ClockManager&				Instance();
		void								PauseClocks();
		void								UnPauseClocks();

		/* =============================================================
		 * INLINE
		 * ============================================================= */

		inline const int	ClocksCount() const { return m_hierarchyLength; }		
	private:
		/* =============================================================
		 * CTOR
		 * ============================================================= */

		ClockManager() 
			: m_root( nullptr )
			, m_lastClockEntry( nullptr )
			, m_hierarchyLength( 0 )
		{}		
		ClockManager( ClockManager const& ) {}
		ClockManager& operator=( ClockManager const& ) {}

		/* =============================================================
		 * MEMBERS
		 * ============================================================= */

		static ClockManager*	s_instance;
		Clock*					m_root;
		Clock*					m_lastClockEntry;
		int						m_hierarchyLength;
	};
}

#endif