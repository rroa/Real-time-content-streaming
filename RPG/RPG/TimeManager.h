#pragma once

#ifndef _TIME_MANAGER_H_
#define _TIME_MANAGER_H_

#include "Clock.h"
#include "StopWatch.h"

namespace Solidus
{
	/* =============================================================
	 * HELPER STRUCTS
	 * ============================================================= */

	//struct TimeUnit {
	//	double tickFreq;
	//	double invTickFreq;
	//	const char* suffix;
	//};

	/* =============================================================
	 * TYPE DEFINITIONS
	 * ============================================================= */

	typedef unsigned long long SystemClock;

	/* =============================================================
	 * FUNCTIONS
	 * ============================================================= */
	
	// Clock Management
	Clock*			CreateClock( float timeScale = 1.0f );
	bool			RemoveClock( Clock* clockToDelete );

	// Getters
	//
	double			GetAbsoluteTimeSeconds();
	SystemClock		GetSystemClocks();
	double			GetSystemFrequencyDouble();
	double			GetSystemTimeSeconds( SystemClock clocks );
	double			GetSystemInvFreq();
	//const TimeUnit* GetTimeUnit( double ticks );
	
	// Init
	//
	void			_InitFrequency();
	void			_InitPerformanceCounter();
	void			InitTimeSystem();
	void			PauseAllClocks();
	void			UnPauseAllClocks();
	void			UpdateTimeSystem( double deltaSeconds );
	void			ShutdownTimeSystem();
	
	// Unit transformations
	//
	double			ClocksToSeconds( SystemClock clocks );
	SystemClock		SecondsToClocks( double seconds );
}

#endif