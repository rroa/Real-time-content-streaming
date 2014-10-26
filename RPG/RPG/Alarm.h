#pragma once

#ifndef _ALARM_H_
#define _ALARM_H_

#include "NamedProperties.h"
#include <string>

namespace Solidus
{
	class Clock;
	class Alarm
	{
	public:
		/* =============================================================
		 * CTOR
		 * ============================================================= */

		explicit Alarm( const unsigned int& seconds, const std::string& eventName, const NamedProperties& eventArgs, const bool& repeats, Clock* parent );

		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */

		void  UpdateAlarm();
		float GetPercentElapsed();

		/* =============================================================
		 * INLINE FUNCTIONS
		 * ============================================================= */

		inline void Activate() { m_active = true; }
		inline void DeActivate() { m_active = false; }
		inline bool IsActive() const { return m_active; }		
	private:
		
		// General Info
		std::string		m_alarmId;
		bool			m_repeats;
		unsigned int	m_seconds;
		Clock*			m_clock;
		int64_t			m_previousElapsedTime;
		// Event
		std::string		m_eventName;
		NamedProperties m_eventArgs;

		// State
		bool			m_active;
	};
}

#endif