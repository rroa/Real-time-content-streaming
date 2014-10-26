#include "stdafx.h"
#include "ClockManager.h"
#include "Clock.h"

namespace Solidus
{
	ClockManager* ClockManager::s_instance = nullptr;

	Clock* ClockManager::AddClock( float timeScale )
	{		
		m_hierarchyLength++;

		if( !m_root )
		{
			m_root			 = new Clock( timeScale );
			m_lastClockEntry = s_instance->m_root;
			return m_root;
		}

		m_lastClockEntry	 = new Clock( timeScale, m_lastClockEntry );
		return m_lastClockEntry;
	}

	void ClockManager::Create()
	{
		if( !s_instance )
		{
			s_instance = new ClockManager;			
		}
	}	

	bool ClockManager::RemoveClock( Clock* clockToDelete )
	{
		bool removed = false;

		int clockCounter = 0;
		Clock* currentClock = m_root;
		while( currentClock )
		{
			clockCounter++;
			if( currentClock->Id() == clockToDelete->Id() )
			{
				removed = true;
				delete currentClock;
				break;
			}

			currentClock = currentClock->FindNextInTree();
		}

		if( removed )
			s_instance->m_hierarchyLength = clockCounter - 1;

		return removed;
	}

	void ClockManager::Destroy()
	{
		if( s_instance )
		{
			// Clearing the hierarchy
			if( s_instance->m_root )
				delete s_instance->m_root;

			// Resetting the instance
			delete s_instance;
			s_instance = nullptr;
		}		
	}

	ClockManager& ClockManager::Instance()
	{
		if( !s_instance )
		{
			Create();
		}

		return *s_instance;
	}	

	void ClockManager::PauseClocks()
	{
		m_root->Pause();
	}

	void ClockManager::UnPauseClocks()
	{
		m_root->UnPause();
	}

	void ClockManager::AdvanceTime( double deltaSeconds )
	{
		m_root->AdvanceTime( deltaSeconds );
	}

	const double ClockManager::ElapsedTime() const
	{
		return m_root->ElapsedTime();
	}
}