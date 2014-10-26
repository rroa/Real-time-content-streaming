#pragma once

#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

#include <time.h>

namespace Solidus
{
	struct StopWatch 
	{	
		/* =============================================================
		 * CTOR
		 * ============================================================= */

		StopWatch()
			: m_tStart( 0 )
			, m_tEnd( 0 )
		{}

		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */

		void Start()
		{
			if( m_tStart == 0 ) 
			{
				m_tStart    = clock();
				m_tEnd      = 0;
				m_isRunning = true;
			}
		}

		void ReStart()
		{
			m_tStart	= clock();
			m_tEnd		= 0;
			m_isRunning = true;
		}

		double ElapsedTime()
		{
			if( !m_isRunning || m_tEnd == 0 )
			{
				return (double)(clock() - m_tStart) / CLOCKS_PER_SEC;
			}
			else
			{
				return (double)(m_tEnd - m_tStart) / CLOCKS_PER_SEC;
			}
		}

		void Stop()
		{
			m_tEnd		= clock();
			m_isRunning = false;
		}
	private:
		/* =============================================================
		 * MEMBERS
		 * ============================================================= */

		bool	m_isRunning;
		clock_t m_tStart;
		clock_t m_tEnd;
	};
}

#endif