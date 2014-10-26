#pragma once

#ifndef _TILED_LEVEL_H_
#define _TILED_LEVEL_H_

#include <vector>
#include <cassert>
#include <string>
#include <map>

#include "Geometry.h"
#include "Sprite.h"
#include "TileType.h"
#include "Vector2.h"
#include "NamedProperties.h"
#include "Entity.h"
#include "Tile.h"

namespace Solidus
{	
	enum MapItemGenerationMode { REGULAR, RANDOM, RANDOM_WEIGHTED, MAX_MAP_GENERATION_MODE };
	class BluePrintBase;
	class Agent;
	class Map : public Sprite
	{				
		friend class Renderer2D;
	public:		
		/* =============================================================
		* CTOR
		* ============================================================= */

		explicit Map( std::string map );
		virtual ~Map();

		/* =============================================================
		* BASE CLASS FUNCTIONS
		* ============================================================= */

		void									Draw( const Renderer2D& painter );		
		void									LoadContent( void );
		void									UpdateSimulation( double deltaTime );
		void									UpdateInput( void );

		/* =============================================================
		* FUNCTIONS
		* ============================================================= */	

		void									AddEntity( Entity* entity );
		bool									InteractWithEntityAtTileLocation( Agent* gameObject, int x, int y );
		void									Create( int width, int height, float pixelsPerTile, float tileWidth, std::map< int, std::shared_ptr< TileType >> cumulativeTileTypes );
		const Vector2&							FromWorldPositionToTileLocation( const Vector2& position );
		Rectangle								FromTileLocationToWorldPosition( int x, int y ) const;
		std::shared_ptr< Tile >&				GetTileAtLocation( int x, int y ); 
		const std::shared_ptr< Tile >&			GetTileAtLocation( int x, int y ) const;
		std::shared_ptr< Tile >&				GetTileAtLocation( const Vector2& location ); 
		const std::shared_ptr< Tile >&			GetTileAtLocation( const Vector2& location ) const;
		bool									IsInBounds( int tileX, int tileY ) const;
		bool									IsInBounds( float tileX, float tileY ) const;
		void									OnMapChange( NamedProperties& eventArgs );	
		std::vector< std::shared_ptr< Tile >>	RayTraceToTiles( int x0, int y0, int x1, int y1 );
		std::vector< std::shared_ptr< Tile >>	RayTraceToTiles( Vector2 startPosition, Vector2 endPosition );
		std::vector< Vector2 >					RayTraceToPositions( int x0, int y0, int x1, int y1 );
		std::vector< Vector2 >					RayTraceToPositions( Vector2 startPosition, Vector2 endPosition );
		Entity*									RemoveEntity( Entity* entity );
		void									ReConstruct( const std::vector< Tile* > tiles );
		int 									GenerateWeightedContent( std::map< int, std::shared_ptr< BluePrintBase >> cumulativeEntities, const std::shared_ptr< Map >& owner );

		/* =============================================================
		* INLINE
		* ============================================================= */

		inline std::string						Name() const { return m_mapName; }
		inline int								Width() const { return m_width; }
		inline int								Height() const { return m_height; }
		inline float							WorldUnitsPerTile() const { return m_worldUnitsPerTile; } // TODO: Remove this
		inline float							PixelsPerTile() const { return m_pixelsPerTile; }		
		inline void								SkipUpdate(){ m_skipUpdate = true; }
		inline const MapItemGenerationMode		GenerationMode() const { return m_generationMode; }
		inline void								SetGenerationMode( MapItemGenerationMode mode ) { m_generationMode = mode; }
		inline const bool						HasHero() const { return m_hasHero; }
		inline Agent*							GetHero() const
		{
			if( m_hasHero )
			{
				return m_hero;
			}
			return nullptr;
		}

		inline void							    SetHeroe( Agent* hero )
		{
			m_hero = hero;
		}
	private:
		/* =============================================================
		* MEMBERS
		* ============================================================= */
		std::string								m_mapName;
		int										m_levelId;
		int										m_height;
		int										m_width;
		float									m_worldWidth;
		float									m_worldHeight;
		float									m_worldUnitsPerTile;
		float									m_pixelsPerTile;		
		std::vector< std::shared_ptr< Tile >>   m_tiles;
		bool									m_skipUpdate;
		MapItemGenerationMode					m_generationMode;

		// This is in order to limit the amount of heroes to 1 for now.
		// It's also a reference to the hero object in order to avoid having
		// to go through all the tiles to locate the actual hero.
		bool									m_hasHero;
		Agent*									m_hero;

		static int								s_nextID;
	};
}

#endif