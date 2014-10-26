#pragma once

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include "NamedProperties.h"
#include <string>
#include <list>

namespace Solidus
{
	class Alarm;
	class Clock
	{		
	public:
		/* =============================================================
		 * CTOR
		 * ============================================================= */

		Clock();
		Clock( const float& timeScale, Clock* parent = nullptr );
		~Clock();

		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */

		void		 AddChild( Clock* child );
		void		 AdvanceTime( double deltaSeconds );
		void		 CreateAlarm( const unsigned int& seconds, const std::string& eventName, const NamedProperties& eventArgs, const bool& repeats );
		Clock*		 FindNextInTree();	

		/* =============================================================
		 * INLINE FUNCTIONS
		 * ============================================================= */

		inline void			Pause() { m_paused = true; }
		inline void			UnPause() { m_paused = false; }
		inline void			SetMaxDeltaTime( double maxDeltaTime ) { m_maxDeltaTime = maxDeltaTime; }
		inline double		ElapsedTime() const { return m_elapsedTime; }
		inline int64_t		ElapsedTimeInt() const { return m_elapsedTimeInt; }
		inline bool			IsRoot() const { return ( m_level == 0 ); }
		inline std::string  Id() const { return m_clockId; }
	private:
		/* =============================================================
		 * MEMBERS
		 * ============================================================= */

		// General Info
		std::string m_clockId;

		// Hierarchy
		int			m_level;
		int			m_childCount;

		// Time
		double		m_maxDeltaTime;
		int64_t		m_elapsedTimeInt;
		double		m_elapsedTime;
		double		m_lastDeltaTime;		
		float		m_timeScale;

		// Hierarchy
		Clock*		m_parent;
		Clock*		m_child;

		// Alarms
		std::list< Alarm* > m_alarms;

		// State
		bool		m_paused;
	};
}

#endif