#pragma once

#ifndef _TILE_H_
#define _TILE_H_

#include <string>
#include <map>

#include "Entity.h"
#include "NamedProperties.h"

namespace Solidus
{	
	struct TileType;
	class  Map;
	class Tile : public Entity
	{
		friend class Renderer2D;
	public:			
		/* =============================================================
		 * CTOR
		 * ============================================================= */

		explicit Tile();
		
		Tile( const Tile& cSource );

		Tile( std::string name, std::shared_ptr< TileType > tileType, float worldUnits, float maxHealth, float health, bool canBeDamaged, Vector2 initialPosition = Vector2::ZeroVector, Vector4 color = Vector4::EmptyColor );

		/* =============================================================
		 * BASE CLASS FUNCTIONS
		 * ============================================================= */

		virtual void LoadContent( void );
		virtual void Draw( const Renderer2D& painter );
		virtual void UpdateSimulation( double deltaTime );
		virtual void UpdateInput( void );
		Tile*		 Clone();
		
		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */

		const bool	 IsSolid() const;
		const bool   CausesHarm() const;
		void MovedToEvent( NamedProperties& eventArgs );
		void MovedFromEvent( NamedProperties& eventArgs );

		/* =============================================================
		 * INLINE
		 * ============================================================= */		

		inline void		  SetType( std::shared_ptr< TileType > tileType ) { m_tileType = tileType;  }
		inline void		  SetKnown() { m_known = true; }
		inline const bool Known() const { return m_known; }
	private:	
		/* =============================================================
		 * PRIVATE FUNCTIONS
		 * ============================================================= */

		void CauseDamageToEntity( Entity* entity );
		void CheckForDeadElementsIOwn();	

		/* =============================================================
		 * MEMBERS
		 * ============================================================= */
		bool						m_known;
		int							m_nUpdates;
		int							m_lastUpdated;
		bool						m_isListDirty;		
		float						m_worldUnits;		
		std::shared_ptr< TileType > m_tileType;		
	};
}

#endif