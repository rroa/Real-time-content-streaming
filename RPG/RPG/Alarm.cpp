#include "stdafx.h"
#include "Alarm.h"
#include "Clock.h"
#include "EventSystem.h"
#include "Utilities.h"
#include "ReportErrors.h"
#include <time.h>

#ifndef _WIN32
#include <sys/time.h>
#endif

namespace Solidus
{
	Alarm::Alarm( const unsigned int& seconds, const std::string& eventName, const NamedProperties& eventArgs, const bool& repeats, Clock* parent )
		: m_alarmId( NewUUID() ) 
		, m_seconds( seconds )
		, m_eventName( eventName )
		, m_eventArgs( eventArgs )
		, m_previousElapsedTime( 0 )
		, m_repeats( repeats )
		, m_clock( parent )
		, m_active( true )
	{

	}

	void Alarm::UpdateAlarm()
	{
		if( !m_active ) return;

		int64_t clockElapsedTime = m_clock->ElapsedTimeInt();
		int64_t diff			 = ( clockElapsedTime - m_previousElapsedTime );		

		//trace( GetPercentElapsed() );

		if( static_cast< float >( diff / 1000 ) >= static_cast< float >( m_seconds ))
		{			
#ifdef _WIN32
			SYSTEMTIME      beg;
#else
			struct timeval  beg;  
			struct tm      *t;        
#endif
			char buffer[50];

#ifdef _WIN32
			GetLocalTime( &beg );
			
			sprintf_s( buffer, "%02d:%02d:%02d:%06d", beg.wHour,
				beg.wMinute, beg.wSecond, beg.wMilliseconds * 1000 );
#else
			gettimeofday( &beg, NULL );
			t = localtime( &beg.tv_sec );

			printf( "%02d:%02d:%02d.%06d\n",
				t->tm_hour, t->tm_min, t->tm_sec, (int) beg.tv_usec );
#endif

			trace(buffer);
			trace("Fired Alarm: " << m_alarmId );
			g_EventManager.FireEvent( m_eventName, m_eventArgs );
			m_previousElapsedTime = clockElapsedTime;
			if( !m_repeats ) 
			{
				m_active = false;
			}
		}		
	}

	float Alarm::GetPercentElapsed()
	{
		int64_t clockElapsedTime = m_clock->ElapsedTimeInt();
		int64_t diff = ( clockElapsedTime - m_previousElapsedTime );
		float pctg = static_cast< float >( diff / m_seconds ) * 100.0f;

		return pctg;
	}

}