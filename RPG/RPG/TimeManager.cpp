#include "stdafx.h"
#include "TimeManager.h"
#include "ClockManager.h"

namespace Solidus
{	
	static LARGE_INTEGER startTime = { 0 };
	static double frequency		   = 0;
	static double invfreq		   = 0;

	SystemClock GetSystemClocks()
	{		
		_InitPerformanceCounter();

		LARGE_INTEGER time;
		QueryPerformanceCounter( &time );

		return time.QuadPart - startTime.QuadPart;
	}

	double GetAbsoluteTimeSeconds()
	{
		return GetSystemClocks() / GetSystemFrequencyDouble();
	}

	double GetSystemFrequencyDouble()
	{		
		_InitFrequency();

		return frequency;
	}

	double GetSystemTimeSeconds( SystemClock clocks )
	{		
		return clocks / GetSystemFrequencyDouble();
	}

	double ClocksToSeconds( SystemClock clocks )
	{
		return clocks / GetSystemFrequencyDouble();
	}

	void _InitFrequency()
	{
		if( frequency == 0 )
		{
			LARGE_INTEGER freq;
			QueryPerformanceFrequency( &freq );
			frequency = static_cast< double >( freq.QuadPart );
		}
	}

	void _InitPerformanceCounter()
	{
		if( startTime.QuadPart == 0 )
		{
			QueryPerformanceCounter( &startTime );
		}
	}

	void InitTimeSystem()
	{		
		_InitPerformanceCounter();
		_InitFrequency();
	}

	SystemClock SecondsToClocks( double seconds )
	{
		return static_cast< SystemClock >( seconds * GetSystemFrequencyDouble() );
	}

	double GetSystemInvFreq()
	{
		invfreq = 1.0 / GetSystemFrequencyDouble();
		return invfreq;
	}

	/*const TimeUnit* GetTimeUnit( double ticks )
	{
		static TimeUnit unit[] = {
			{ GetSystemFrequencyDouble() / 1.0			, GetSystemInvFreq() * 1.0			, "s" },
			{ GetSystemFrequencyDouble() / 1000.0		, GetSystemInvFreq() * 1000.0		, "ms" },
			{ GetSystemFrequencyDouble() / 1000000.0	, GetSystemInvFreq() * 1000000.0	, "us" },
			{ GetSystemFrequencyDouble() / 1000000000.0	, GetSystemInvFreq() * 1000000000.0	, "ns" }
		};

		if (unit[ 0 ].tickFreq < ticks) 
			return &unit[ 0 ];
		else if ( unit[ 1 ].tickFreq < ticks ) 
			return &unit[ 1 ];
		else if ( unit[ 2 ].tickFreq < ticks ) 
			return &unit[ 2 ];
		else 
			return &unit[ 3 ];
	}*/

	Clock* CreateClock( float timeScale /*= 1.0f */ )
	{
		return ClockManager::Instance().AddClock( timeScale );
	}

	bool RemoveClock( Clock* clockToDelete )
	{
		return ClockManager::Instance().RemoveClock( clockToDelete );
	}

	void PauseAllClocks()
	{
		ClockManager::Instance().PauseClocks();
	}

	void UnPauseAllClocks()
	{
		ClockManager::Instance().UnPauseClocks();
	}

	void ShutdownTimeSystem()
	{
		ClockManager::Instance().Destroy();
	}

	void UpdateTimeSystem( double deltaSeconds )
	{
		ClockManager::Instance().AdvanceTime( deltaSeconds );
	}
}