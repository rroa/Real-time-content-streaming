#pragma once

#ifndef _STATE_MANAGER_H_
#define _STATE_MANAGER_H_

#include <deque>
#include <utility>
#include <vector>

namespace Solidus
{	
	class Entity;
	class Agent;
	class Item;
	class Tile;

	struct FrameInfo
	{
		FrameInfo( int update, Entity* parent )
			: Update( update )
			, Parent( parent )
		{}

		int		Update;
		Entity* Parent;
	};

	class StateManager
	{
	public:
		/* =============================================================
		 * STATIC FUNCTIONS
		 * ============================================================= */

		static void							Create();
		static void							Destroy();
		static StateManager&				Instance();

		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */
		void					SetCurrentUpdate( int currentUpdate );
		void					SaveEntityState( Entity* entity );
		void					MoveNext();
		void					MovePrev();		
		void					UpdateEntitiesToCurrentFrame();		

		/* =============================================================
		 * INLINE
		 * ============================================================= */
		inline void		  StartRecording() { m_record = true; }
		inline void		  StopRecording() { m_record = false; }
		inline void		  Pause() { m_paused = !m_paused; }
		inline const bool IsPaused() const { return m_paused; }
		inline const int  CurrentFrame() const { return m_currentFrameIndex; }
		inline const int  TotalRecordedFrames() const { return static_cast< int >( m_recordedTiles.size() ); }
	private:
		/* =============================================================
		 * CTOR
		 * ============================================================= */

		StateManager();		
		StateManager( StateManager const& ) {};
		StateManager& operator=( StateManager const& ) {};

		/* =============================================================
		 * DATA
		 * ============================================================= */

		static StateManager*										 s_instance;		
		std::deque< std::pair< FrameInfo, std::vector< Tile* >*>>    m_recordedTiles;
		bool														 m_paused;
		bool														 m_record;
		int															 m_maxFrames;
		int															 m_currentFrameIndex;
		int															 m_currentUpdate;
		int															 m_lastUpdatedFrameIndex;
	};
}

#endif