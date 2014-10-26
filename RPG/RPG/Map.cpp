#include "stdafx.h"
#include <cassert>
#include <iostream>
#include <utility>
#include <limits>
#include <algorithm>

#include "Map.h"
#include "Agent.h"
#include "AStar.h"
#include "Entity.h"
#include "EventSystem.h"
#include "FileManager.h"
#include "Item.h"
#include "ReportErrors.h"
#include "ResourceManager.h"
#include "StringExtensions.h"
#include "Tile.h"
#include "Utilities.h"
#include "BluePrintBase.h"
#include "BluePrint.h"
#include "BluePrintManager.h"

namespace Solidus
{
	int Map::s_nextID = 0;
	Map::Map( std::string mapName )
		: m_width( 0 )
		, m_height( 0 )
		, m_worldUnitsPerTile( 0 )	
		, m_hasHero( false )		
		, m_skipUpdate( false )
		, m_mapName( mapName )
		, m_generationMode( REGULAR )
	{
		m_levelId = ++s_nextID;		
		g_EventManager.Register( this, &Map::OnMapChange, "MapChange" );	
	}

	Map::~Map()
	{
		// Clearing out the tiles list
		m_tiles.clear();
	}	

	void Map::Create( int width, int height, float pixelsPerTile, float tileWidth, 
		std::map< int, std::shared_ptr< TileType >> cumulativeTileTypes )
	{
		assert( width > 0 );
		assert( height > 0 );
		assert( tileWidth > 0 );		
		assert( pixelsPerTile > 0);

		// Letting the black magic work by itself
		//
		std::shared_ptr< Map > currentMap = std::make_shared< Map >( *this );

		// Setting proportional values
		//
		m_height			= height;
		m_width				= width;
		m_worldUnitsPerTile = tileWidth;
		m_pixelsPerTile		= pixelsPerTile;

		// Clear tiles
		m_tiles.clear();

		// Re-dimension tile space based on width and height		
		m_tiles.resize( m_width * m_height );		

		for( int x = 0; x < m_width; ++x )
		{
			// Swap the elements of the row.
			//
			for( int y = 0; y < m_height; ++y )
			{
				if( cumulativeTileTypes.size() > 0 )
				{					
					auto tileType = WeightedRandom( cumulativeTileTypes );
					if( tileType )
					{
						Vector2 tileLocation( x, y );
						// Tile Construction
						//						
						GetTileAtLocation( x, y ).
							reset( new Tile( tileType->Name, tileType, tileWidth,
							tileType->MaxHealth, tileType->MaxHealth, true,
							Vector2( x, y) ));
					}				
				}
			}
		}

		// Load the map content
		//
		LoadContent();
	}

	bool Map::InteractWithEntityAtTileLocation( Agent* gameObject, int x, int y  )
	{
		Vector2 gameObjectNextLocation( x, y );
		bool canMoveToTile = true;				

		auto currentTile = GetTileAtLocation( static_cast< int > ( gameObjectNextLocation.x ), 
			static_cast< int >( gameObjectNextLocation.y ));

		//DEBUG("The type of the tile you want to reach to is " << currentTile->Name() );
		//DEBUG("The tile you want reach to has " << currentTile->HasItems() << " items");

		if( currentTile->IsSolid() )
		{			
			if(  currentTile->CanBeDamaged() )
			{
				// TODO: ADD code to handle tile damage
				// E.g. destroying rocks if a special item is equipped
				DEBUG( gameObject->Name() << " will try to destroy " << currentTile->Name() );
			}
			canMoveToTile = false;
		}		

		if( currentTile->HasItems() )
		{
			// Check for ITEM
			//
			Entity* lastItem = currentTile->GetLastItem();
			if( !lastItem || lastItem->Id() == gameObject->Id() )
			{
				lastItem = currentTile->GetPenultimateItem();
			}

			Item* itemObject = dynamic_cast< Item* >( lastItem );			

			if( itemObject )
			{
				if( itemObject->CanBeEquipped() )
				{
					if( StringExtensions::CompareStrings( gameObject->Name(), "hero" ) )
					{						
						if( !m_hero->IsInventoryFull() )
						{			

							if( StringExtensions::CompareStrings( itemObject->Name(), "torch" ) )
							{
								DEBUG( gameObject->Name() << " picked up a " << itemObject->Name() );
								gameObject->PickedTorch();
								
								currentTile->RemoveLastItem();								
								m_hero->AddItem( itemObject );
							}
							
							if( itemObject->InventorySlot() != NO_SLOT )
							{
								if( !m_hero->IsSlotInUse( itemObject->InventorySlot()) && 
									m_hero->CanLoadSlot( itemObject->InventorySlot() ))
								{
									DEBUG( gameObject->Name() << " picked up a " << itemObject->Name() );
									
									currentTile->RemoveLastItem();
									m_hero->AddItem( itemObject );
								}
							}
							else if( itemObject->InventorySlot() == CONSUMABLE )
							{
								DEBUG( gameObject->Name() << " picked up a " << itemObject->Name() );
								DEBUG( gameObject->Name() << " is about to use a " << itemObject->Name() 
									<< ", his current health is " << gameObject->Health() );

								gameObject->ReceiveHealing( itemObject->Health() );

								DEBUG( gameObject->Name() << " drank a potion and got a boost of " << itemObject->Health() << " points in health." );
								DEBUG( gameObject->Name() << " now has " << gameObject->Health() << " points of health." );
								
								currentTile->RemoveLastItem();
								delete itemObject;
							}
							else
							{
								currentTile->RemoveLastItem();								
								m_hero->AddItem( itemObject );
							}
						}
					}
				}
				else
				{
					currentTile->AddItem( itemObject );
				}				
			}

			// Check for Entity
			//
			Entity* entityObject = dynamic_cast< Entity* >( currentTile->GetLastItem() );
			if( entityObject )
			{
				if( entityObject->BlocksPosition() )
				{
					canMoveToTile = false;
					if( entityObject->CanBeUnlocked() &&
						StringExtensions::CompareStrings( gameObject->Name(), "hero" ))
					{
						DEBUG( entityObject->Name() << " can be unlocked with a " << entityObject->ItemThatUnlocks() );
						DEBUG( "Trying to unlock " << entityObject->Name() << "..." );
						bool unlocked = gameObject->UseItem( entityObject->ItemThatUnlocks() );
						if( unlocked )
						{
							DEBUG( gameObject->Name() << " used a " << entityObject->ItemThatUnlocks() << "!" );
							DEBUG( gameObject->Name() << " successfully unlocked " << entityObject->Name() << "!" );
							entityObject->UnBlock();							
						}
						else
						{							
							DEBUG( gameObject->Name() << " doesn't have a " << entityObject->ItemThatUnlocks() << "!" );
						}
					}
					else
					{						
						if( entityObject->CanBeDamaged() )
						{
							DEBUG( gameObject->Name() << " will now attack " << entityObject->Name() );						
							entityObject->TakeDamage( gameObject->Attack() );
						}
					}					
				}
				else
				{
					if( entityObject->CanWarp() )
					{
						if( StringExtensions::CompareStrings( gameObject->Name(), "hero" ) )
						{
							NamedProperties args;
							args.SetProperty( "Destination", entityObject->Destination() );
							args.SetProperty( "WarpTo", entityObject->WarpTo() );

							OnMapChange( args );
							canMoveToTile = false;
						}
					}
				}
			}

			// Check for Agent
			//
			Agent* agentObject = dynamic_cast< Agent* >( currentTile->GetLastItem() );
			if( agentObject )
			{
				canMoveToTile = false;
				if( agentObject->CanBeDamaged() && agentObject->Behavior() != INDIFFERENCE )
				{	
					DEBUG( gameObject->Name() << " will now attack " << agentObject->Name() );						
					agentObject->TakeDamage( gameObject->Attack() );

					DEBUG( agentObject->Name() << " will now attack " << gameObject->Name() );
					gameObject->TakeDamage( agentObject->Attack() );				
				}
				else
				{
					//agentObject->DropItemFromInventory();
					//agentObject->MoveNorth();
				}
			}
		}
		
		return canMoveToTile;
	}

	const Vector2& Map::FromWorldPositionToTileLocation( const Vector2& position )
	{
		return position;
	}

	void Map::OnMapChange( NamedProperties& eventArgs )
	{
		std::string newMapName;
		Vector2 newHeroLocation;
		eventArgs.GetProperty( "Destination", newMapName );
		eventArgs.GetProperty( "WarpTo", newHeroLocation );
		
		auto newMap		= MapManager::Instance().GetMap( newMapName );
		auto currentMap = MapManager::Instance().CurrentMap();
		if( newMap->Name() != currentMap->Name() )
		{
			DEBUG( "Changing to map: " << newMapName );			

			int intXCoordinate = static_cast< int >( newHeroLocation.x );
			int intYCoordinate = static_cast< int >( newHeroLocation.y );
			
			Agent* ptrToHero = MapManager::Instance().CurrentMap()->GetHero();

			if( newMap->IsInBounds( intXCoordinate, intYCoordinate - 1 ) )
			{
				ptrToHero->Translate( newHeroLocation.x, newHeroLocation.y - 1.0f );
			}
			else
			{
				ptrToHero->Translate( newHeroLocation.x, newHeroLocation.y );
			}		

			// Removing hero from this map
			/*auto theHero = */RemoveEntity( ptrToHero );
			m_hero = nullptr;
			m_hasHero = false;
			
			// Adding the hero to the new map			
			ptrToHero->SetMap( newMap );
			newMap->AddEntity( ptrToHero );	

			MapManager::Instance().SetMap( newMapName );
		}		
	}

	void Map::Draw( const Renderer2D& painter )
	{			
		// Draw the map
		painter.DrawLevel( *this );

		// Draw the tiles children
		//
		for( auto iter = m_tiles.begin(); iter != m_tiles.end(); ++iter )
		{					
			(*iter)->Draw( painter );			
		}			
	}

	void Map::AddEntity( Entity* entity )
	{
		// Load the content for the new added entity
		//
		entity->LoadContent();

		Agent* agent = dynamic_cast< Agent* >( entity );

		if( agent )
		{
			// Reference to the hero
			//
			if( StringExtensions::CompareStrings( entity->Name(), "hero" ))
			{
				if( !m_hasHero )
				{
					m_hasHero = true;
					m_hero    = agent;
				}
				else
				{
					// If there's a hero do nothing!
					//
					return;
				}
			}
			
			agent->LoadSlotWithItem( HEAD );
			agent->LoadSlotWithItem( CHEST );
			agent->LoadSlotWithItem( ARMS );
			agent->LoadSlotWithItem( LEGS );
		}

		entity->ValidateInitialLocation();
		Vector2 location = entity->GetLocation();
		auto tile = GetTileAtLocation( static_cast< int >( location.x ), static_cast< int > ( location.y ));

		// Add to the tile inventory
		//
		tile->AddItem( entity );
	}

	Entity* Map::RemoveEntity( Entity* entity )
	{
		for( auto iter = m_tiles.begin(); iter != m_tiles.end(); ++iter )
		{
			auto removedEntity = ( *iter )->RemoveItem( entity );
			if( removedEntity )
			{
				return removedEntity;
			}
		}
		return nullptr;
	}

	void Map::LoadContent()
	{
		for( auto iter = m_tiles.begin(); iter != m_tiles.end(); ++iter )
		{
			( *iter )->LoadContent();
		}
	}

	void Map::UpdateInput()
	{
		for( auto iter = m_tiles.begin(); iter != m_tiles.end(); ++iter )
		{
			(*iter)->UpdateInput();	
			if( m_skipUpdate ) 
			{
				m_skipUpdate = false;
				break;
			}
		}		
	}	

	void Map::UpdateSimulation( double deltaTime )
	{
		for( auto iter = m_tiles.begin(); iter != m_tiles.end(); ++iter )
		{
			(*iter)->UpdateSimulation( deltaTime );
			if( m_skipUpdate ) 
			{
				m_skipUpdate = false;
				break;
			}
		}
	}

	Rectangle Map::FromTileLocationToWorldPosition( int x, int y ) const
	{
		assert( IsInBounds( x, y ) );

		float fractionX = ( static_cast< float >( x ) / Width());
		float fractionY = ( static_cast< float >( y ) / Height());

		int newValueX = static_cast< int >( fractionX * ( m_worldWidth * 2 ) - m_worldWidth );
		int newValueY = static_cast< int >( fractionY * ( m_worldHeight * 2 ) - m_worldHeight );


		Rectangle rect;
		rect.left = newValueX;
		rect.right = static_cast< int >( newValueX + m_worldUnitsPerTile );
		rect.bottom = newValueY;
		rect.top = static_cast< int >( newValueY + m_worldUnitsPerTile );

		return rect;
	}

	std::shared_ptr< Tile >& Map::GetTileAtLocation( int x, int y )
	{
		if( IsInBounds( x, y ) ) return m_tiles[ y * m_width + x ];

		return m_tiles[ 0 ];
	}

	const std::shared_ptr< Tile >& Map::GetTileAtLocation( int x, int y ) const
	{
		return const_cast< Map* >( this )->GetTileAtLocation( x, y );
	}

	std::shared_ptr< Tile >& Map::GetTileAtLocation( const Vector2& location )
	{
		assert( location.x >= 0 && location.x < m_width );
		assert( location.y >= 0 && location.y < m_height );

		int x = static_cast< int >( location.x );
		int y = static_cast< int >( location.y );

		return m_tiles[ y * m_width + x ];
	}

	const std::shared_ptr< Tile >& Map::GetTileAtLocation( const Vector2& location ) const
	{
		return const_cast< Map* >( this )->GetTileAtLocation( location );
	}

	bool Map::IsInBounds( int tileX, int tileY ) const
	{
		return tileX >= 0 && tileX < m_width && tileY >= 0 && tileY < m_height;
	}

	bool Map::IsInBounds( float tileX, float tileY ) const
	{
		return IsInBounds( static_cast< int >( tileX ), static_cast< int >( tileY ) );
	}

	std::vector< std::shared_ptr< Tile >> Map::RayTraceToTiles( int x0, int y0, int x1, int y1 )
	{
		std::vector< std::shared_ptr< Tile >> visitedTiles;

		int dx = abs( x1 - x0 );
		int dy = abs( y1 - y0 );
		
		int amountOfTilesToVisit = 1 + dx + dy;

		// Reserving space for the amount of tiles we're going to visit
		//
		visitedTiles.reserve( amountOfTilesToVisit );

		int nextPositionInX = ( x1 > x0 ) ? 1 : -1;
		int nextPositionInY = ( y1 > y0 ) ? 1 : -1;

		// Pivot
		//
		int slopePivot = dx - dy;
		dx = dx * 2;
		dy = dy * 2;

		// Initial Position
		int x = x0;
		int y = y0;		

		for (  ; amountOfTilesToVisit > 0; --amountOfTilesToVisit )
		{
			// Adding the tile to the list
			//
			visitedTiles.push_back( GetTileAtLocation( x, y ) );

			// Determining which way we're going next
			//
			if ( slopePivot > 0 )
			{
				// Let's move in the x axis
				x		   = x + nextPositionInX;
				slopePivot = slopePivot - dy;
			}
			else
			{
				// Let's move in the y axis
				y		   = y + nextPositionInY;
				slopePivot = slopePivot + dx;
			}
		}

		return visitedTiles;
	}

	std::vector< std::shared_ptr< Tile >> Map::RayTraceToTiles( Vector2 startPosition, Vector2 endPosition )
	{

		return RayTraceToTiles( static_cast< int >( startPosition.x ), 
								static_cast< int >( startPosition.y ), 
								static_cast< int >( endPosition.x ), 
								static_cast< int >( endPosition.y ));
	}

	std::vector< Vector2 > Map::RayTraceToPositions( int x0, int y0, int x1, int y1 )
	{
		std::vector< Vector2 > visitedPositions;

		int dx = abs( x1 - x0 );
		int dy = abs( y1 - y0 );

		int nextPositionInX = ( x1 > x0 ) ? 1 : -1;
		int nextPositionInY = ( y1 > y0 ) ? 1 : -1;

		// Pivot
		//
		int slopePivot = dx - dy;
		dx = dx * 2;
		dy = dy * 2;

		// Initial Position
		int x = x0;
		int y = y0;

		int amountOfTilesToVisit = 1 + dx + dy;

		// Reserving space for the amount of tiles we're going to visit
		//
		visitedPositions.reserve( amountOfTilesToVisit );

		for (  ; amountOfTilesToVisit > 0; --amountOfTilesToVisit )
		{
			// Adding the tile to the list
			//
			visitedPositions.push_back( Vector2( x, y ) );

			// Determining which way we're going next
			//
			if ( slopePivot > 0 )
			{
				// Let's move in the x axis
				x		   = x + nextPositionInX;
				slopePivot = slopePivot - dy;
			}
			else
			{
				// Let's move in the y axis
				y		   = y + nextPositionInY;
				slopePivot = slopePivot + dx;
			}
		}

		return visitedPositions;
	}

	std::vector< Vector2 > Map::RayTraceToPositions( Vector2 startPosition, Vector2 endPosition )
	{
		return RayTraceToPositions( static_cast< int >( startPosition.x ), 
			static_cast< int >( startPosition.y ), 
			static_cast< int >( endPosition.x ), 
			static_cast< int >( endPosition.y ));		
	}

	void Map::ReConstruct( const std::vector< Tile* > tiles )
	{
		//m_tiles.clear();
		
		for( auto tile = tiles.begin(); tile != tiles.end(); ++tile )
		{
			Tile* currentTile = *tile;
			GetTileAtLocation( currentTile->GetLocation() ).
				reset( new Tile( *currentTile ));
		}
	}

	int Map::GenerateWeightedContent( std::map< int, std::shared_ptr< BluePrintBase >> cumulativeEntities, const std::shared_ptr< Map >& owner )
	{
		int totalAvailable = static_cast< int >( m_tiles.size() );
		int generatedEntities = 0;

		srand( GetTickCount() );
		int entitiesToGenerate = ( rand() % ( totalAvailable )) / 10;

		for( int i = 0; i < entitiesToGenerate; ++i )
		{
			auto bluePrint = WeightedRandom( cumulativeEntities );
			if( bluePrint )
			{
				auto agentBluePrint = dynamic_cast< BluePrint< Agent >* >( bluePrint.get() );
				if( agentBluePrint )
				{
					Agent* agent = agentBluePrint->Create();
					agent->SetMap( owner );
					AddEntity( agent );
					generatedEntities++;
				}

				auto entityBluePrint = dynamic_cast< BluePrint< Entity >* >( bluePrint.get() );
				if( entityBluePrint )
				{
					Entity* entity = entityBluePrint->Create();
					entity->SetMap( owner );
					AddEntity( entity );
					generatedEntities++;
				}

				auto itemBluePrint = dynamic_cast< BluePrint< Item >* >( bluePrint.get() );
				if( itemBluePrint )
				{
					Item* item = itemBluePrint->Create();
					item->SetMap( owner );
					AddEntity( item );
					generatedEntities++;
				}
			}
		}	

		if( !owner->HasHero() )
		{
			// Add the hero
			//
			BluePrint< Agent >* heroBluePrint;
			BluePrintManager::Instance().GetBluePrint( "Hero", heroBluePrint );
			Agent* hero = heroBluePrint->Create();
			hero->SetMap( owner );
			AddEntity( hero );
		}		

		return generatedEntities;
	}
}