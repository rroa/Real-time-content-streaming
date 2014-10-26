#include "stdafx.h"
#include "StateManager.h"
#include "Entity.h"
#include "Agent.h"
#include "Item.h"
#include "Renderer2D.h"
#include "Tile.h"

namespace Solidus
{
	StateManager* StateManager::s_instance = nullptr;

	void StateManager::Create()
	{
		if( !s_instance )
		{
			s_instance = new StateManager;
		}
	}

	void StateManager::Destroy()
	{
		if( s_instance )
		{
			// Clearing the map
			for( auto iter = s_instance->m_recordedTiles.begin(); iter != s_instance->m_recordedTiles.end(); ++iter )
			{
				delete iter->second;
			}

			s_instance->m_recordedTiles.clear();

			// Resetting the instance
			delete s_instance;
			s_instance = nullptr;
		}		
	}

	StateManager& StateManager::Instance()
	{
		if( !s_instance )
		{
			Create();
		}

		return *s_instance;
	}

	StateManager::StateManager()
		: m_paused( false )
		, m_maxFrames( 1800 )
		, m_currentFrameIndex( -1 )
		, m_currentUpdate( 0 )
		, m_lastUpdatedFrameIndex( -1 )
		, m_record( false )
	{}

	void StateManager::SetCurrentUpdate( int currentUpdate )
	{
		if( !m_record ) return;
		m_currentUpdate = currentUpdate;

		// Create a new entry to the state recorder

		++m_currentFrameIndex;		

		if( m_maxFrames == m_currentFrameIndex )
		{
			// Remove the first entry
			m_recordedTiles.pop_back();

			// Stay at the top
			m_currentFrameIndex -= 1;
		}				

		m_recordedTiles.push_front( std::make_pair< FrameInfo, std::vector< Tile* >*>( FrameInfo(m_currentUpdate, nullptr), new std::vector< Tile* >() ));
	}

	void StateManager::SaveEntityState( Entity* entity )
	{
		if( !m_record ) return;

		Tile* tile = dynamic_cast< Tile* >( entity );
		if( tile )
		{			
			if( m_recordedTiles.size() > 0 )
			{
				m_recordedTiles.front().first.Parent = tile;
				m_recordedTiles.front().second->push_back( tile->Clone() );
			}			
		}
	}

	void StateManager::MoveNext()
	{
		m_paused = true;
		m_record = false;
		++m_currentFrameIndex;
		if( static_cast< int >( m_recordedTiles.size() ) == m_currentFrameIndex )
		{
			m_currentFrameIndex = 0;
		}
	}

	void StateManager::MovePrev()
	{
		m_paused = true;
		m_record = false;
		m_currentFrameIndex--;
		if( m_currentFrameIndex < 0 )
		{
			m_currentFrameIndex = m_recordedTiles.size() - 1;
		}
	}

	void StateManager::UpdateEntitiesToCurrentFrame()
	{		
		// If there's no recorded frames exit
		//
		if( m_recordedTiles.size() == 0 ) return;
		
		// If there hasn't been any updates in the frame Index then exit
		//
		if( m_lastUpdatedFrameIndex == m_currentFrameIndex ) return;
		
		// Updated the current updated Index
		//
		m_lastUpdatedFrameIndex = m_currentFrameIndex;

		// Rebuild the map with the new tile states
		//
		MapManager::Instance().CurrentMap()->ReConstruct( *m_recordedTiles[ m_currentFrameIndex ].second );
	}
}