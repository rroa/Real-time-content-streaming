#include "stdafx.h"
#include "Clock.h"
#include "Utilities.h"
#include "ReportErrors.h"
#include "Alarm.h"

namespace Solidus
{
	Clock::Clock()
		: m_clockId( NewUUID() )
		, m_level( 0 )
		, m_timeScale( 1.0f )
		, m_elapsedTime( 0.0 )
		, m_childCount( 0 )
		, m_maxDeltaTime( 1.0 )
		, m_parent( nullptr )
		, m_child( nullptr )
		, m_paused( false )
		, m_elapsedTimeInt( 0 )
	{}

	Clock::Clock( const float& timeScale, Clock* parent )
		: m_clockId( NewUUID() ) 
		, m_level( 0 )
		, m_timeScale( timeScale )		
		, m_elapsedTime( 0.0 )
		, m_maxDeltaTime( 1.0 )
		, m_childCount( 0 )
		, m_parent( parent )
		, m_child( nullptr )		
		, m_paused( false )
		, m_elapsedTimeInt( 0 )
	{
		if( parent )
		{
			m_level = m_parent->m_level + 1;
			parent->AddChild( this );
		}
	}

	Clock::~Clock()
	{
		std::string id = m_clockId;
		if( m_childCount > 0 )
		{
			if( m_child )
			{		
				m_child->m_parent->m_childCount = 0;
				delete m_child;
			}
		}		

		// Removing the alarms
		//
		for( auto alarmIterator = m_alarms.begin(); alarmIterator != m_alarms.end(); ++alarmIterator )
		{
			delete *alarmIterator;			
		}
		m_alarms.clear();

		trace( "Deleting clock: " << id );
	}

	void Clock::AddChild( Clock* child )
	{
		m_childCount++;
		if( !m_child )
		{
			m_child = child;
		}
		else
		{
			delete m_child;
			m_child = child;
		}
	}

	void Clock::AdvanceTime( double deltaSeconds )
	{
		// If clock is paused the do nothing.
		//
		if( m_paused ) return;
		
		m_lastDeltaTime = deltaSeconds;

		double timeIncrement = m_timeScale * deltaSeconds;
		if( timeIncrement > m_maxDeltaTime )
			timeIncrement = m_maxDeltaTime;
		
		m_elapsedTimeInt += static_cast< int64_t >( timeIncrement * 1000 );
		m_elapsedTime    += timeIncrement;

		// Update the alarms
		//
		for( auto alarmIterator = m_alarms.begin(); alarmIterator != m_alarms.end(); ++alarmIterator )
		{
			( *alarmIterator )->UpdateAlarm();
			
		}

		if( m_child ) m_child->AdvanceTime( timeIncrement );		
	}	

	Clock* Clock::FindNextInTree()
	{		
		return ( m_child ) ? m_child : nullptr;		
	}

	void Clock::CreateAlarm( const unsigned int& seconds, const std::string& eventName, const NamedProperties& eventArgs, const bool& repeats )
	{
		Alarm* alarmToAdd = new Alarm( seconds, eventName, eventArgs, repeats, this );
		m_alarms.push_back( alarmToAdd );
	}
}