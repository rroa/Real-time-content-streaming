#include "stdafx.h"
#include "Agent.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "ReportErrors.h"
#include "Map.h"
#include "Item.h"
#include "MathUtilities.h"
#include "EventSystem.h"
#include "AStar.h"


namespace Solidus
{
	const int	 DEFAULT_HIT_DAMAGE			  = 250;
	const int	 AMOUNT_OF_CARDINAL_MOVEMENTS = 4;
	const int	 DISTANCE_RADIUS			  = 6;
	const int	 LIGHT_OFFSET				  = 2;
	const double TORCH_LIFETIME				  = 10.0;
	const int	 VIEW_RADIUS				  = 2;

	Agent::Agent( const Agent& cSource )
		: Entity( cSource )
		, IStats( cSource.m_strength, cSource.m_dexterity, cSource.m_toughness, cSource.m_dodge )
		, AI( cSource.m_behavior )
		, m_updateAlways( cSource.m_updateAlways )
		, m_drawAlways( cSource.m_drawAlways )
		, m_canMove( cSource.m_canMove )
		, m_currentLocationIndex( cSource.m_currentLocationIndex )		
		, m_movementSpeed( cSource.m_movementSpeed )
		, m_canBeControlled( cSource.m_canBeControlled )
		, m_rangeLength( cSource.m_rangeLength )
		, m_rangeMode( cSource.m_rangeMode )
		, m_conditionalHealth( cSource.m_conditionalHealth )
		, m_conditionalMovement( cSource.m_conditionalMovement )
		, m_currentBehavior( cSource.m_currentBehavior )
		, m_viewRadius( cSource.m_viewRadius )
		, m_fieldOfView( cSource.m_fieldOfView )
		, m_currentDirection( cSource.m_currentDirection )
		, m_hasTorch( cSource.m_hasTorch )
		, m_torchDeltaTime( cSource.m_torchDeltaTime )
		, m_torchPreviousDeltaTime( cSource.m_torchPreviousDeltaTime )
	{
		if( !StringExtensions::CompareStrings( m_name, "hero" ))
		{			
			g_EventManager.Register( this, &Agent::FindTarget, "FindTarget" );
			g_EventManager.Register( this, &Agent::CalculateDirectionFromHero, "CalculateDirectionFromHero" );			
		}
	}

	Agent::Agent( std::string name, 
		const char* texturePath, 
		float textureScale, 
		bool updateAlways, 
		bool drawAlways, 
		int inventoryLimit, 
		// Health
		float maxHeatlh, 
		float health, 
		bool canBeDamaged, 
		// Movement
		bool canBeControlled, 
		bool canMove, 
		float movementSpeed, 
		BehaviorType behavior, 
		float conditionalHealth, 
		bool conditionalMovement, 
		// Stats
		float strength, 
		int dexterity, 
		float toughness, 
		int dodge,
		Vector2 initialPosition, Vector4 color )
		: Entity( name, true, StringExtensions::EmptyString, texturePath, textureScale, maxHeatlh, health, false, 
			canBeDamaged, inventoryLimit, StringExtensions::EmptyString, Vector2::ZeroVector, 
			initialPosition, color )
		, IStats( strength, dexterity, toughness, dodge )
		, AI( behavior )
		, m_updateAlways( updateAlways )
		, m_drawAlways( drawAlways )
		, m_canMove( canMove )
		, m_currentLocationIndex( 0 )		
		, m_movementSpeed( movementSpeed )
		, m_canBeControlled( canBeControlled )
		, m_rangeLength( 2 )
		, m_rangeMode( false )
		, m_conditionalHealth( conditionalHealth )
		, m_conditionalMovement( conditionalMovement )
		, m_currentBehavior( NO_BEHAVIOR )
		, m_viewRadius( VIEW_RADIUS )
		, m_fieldOfView( PI * 0.4f )
		, m_currentDirection( NORTH )
		, m_hasTorch( false )
		, m_torchDeltaTime( 0.0 )
		, m_torchPreviousDeltaTime( 0.0 )
	{
		if( !StringExtensions::CompareStrings( m_name, "hero" ))
		{			
			g_EventManager.Register( this, &Agent::FindTarget, "FindTarget" );
			g_EventManager.Register( this, &Agent::CalculateDirectionFromHero, "CalculateDirectionFromHero" );			
		}		
	}

	bool Agent::MoveNorth()
	{		
		int intXCoord = static_cast< int >( m_x );
		int intYCoord = static_cast< int >( m_y );

		if( m_rangeMode )
		{
			RangedAttack( intXCoord, intYCoord + m_rangeLength );
			return false;
		}

		// Update coordinate to North
		//
		intYCoord +=1;
		m_currentDirection = NORTH;

		std::shared_ptr< Map > derreferencedMap;

		// If reference to shared pointer expired then exit
		if( m_map.expired() ) return false;

		// Locking map
		derreferencedMap = m_map.lock();

		// Verifying the reference is not empty
		if( !derreferencedMap ) return false;

		// Check if new coordinate is in bounds
		if( !derreferencedMap->IsInBounds( intXCoord, intYCoord )) return false;

		// Interact with the tile
		bool canMoveToTile = 
			derreferencedMap->InteractWithEntityAtTileLocation( this, intXCoord, intYCoord );

		if( canMoveToTile )
		{
			// Updating my location
			UpdateAgentPosition( intXCoord, intYCoord, derreferencedMap.get() );
		}

		return canMoveToTile;
	}

	bool Agent::MoveSouth()
	{		
		int intXCoord = static_cast< int >( m_x );
		int intYCoord = static_cast< int >( m_y );

		if( m_rangeMode )
		{
			RangedAttack( intXCoord, intYCoord - m_rangeLength );
			return false;
		}

		// Updating coordinate to south
		intYCoord -= 1;
		m_currentDirection = SOUTH;

		std::shared_ptr< Map > derreferencedMap;

		// If reference to shared pointer expired then exit
		if( m_map.expired() ) return false;

		derreferencedMap = m_map.lock();

		if( !derreferencedMap->IsInBounds( intXCoord, intYCoord )) return false;

		bool canMoveToTile = 
			derreferencedMap->InteractWithEntityAtTileLocation( this, intXCoord, intYCoord );

		if( canMoveToTile )
		{			
			// Updating my location
			UpdateAgentPosition( intXCoord, intYCoord, derreferencedMap.get() );
		}

		return canMoveToTile;
	}

	bool Agent::MoveEast()
	{
		int intXCoord = static_cast< int >( m_x );
		int intYCoord = static_cast< int >( m_y );

		if( m_rangeMode )
		{
			RangedAttack( intXCoord + m_rangeLength, intYCoord );
			return false;
		}

		// Updating coordinate to East
		//
		intXCoord += 1;
		m_currentDirection = EAST;

		std::shared_ptr< Map > derreferencedMap;

		// If reference to shared pointer expired then exit
		if( m_map.expired() ) return false;

		derreferencedMap = m_map.lock();

		if( !derreferencedMap->IsInBounds( intXCoord, intYCoord )) return false;

		bool canMoveToTile = derreferencedMap->InteractWithEntityAtTileLocation( this, intXCoord, intYCoord );

		if( canMoveToTile )
		{			
			// Updating my location
			UpdateAgentPosition( intXCoord, intYCoord, derreferencedMap.get() );
		}	

		return canMoveToTile;
	}

	bool Agent::MoveWest()
	{
		int intXCoord = static_cast< int >( m_x );
		int intYCoord = static_cast< int >( m_y );

		if( m_rangeMode )
		{
			RangedAttack( intXCoord - m_rangeLength, intYCoord );
			return false;
		}

		// Updating coordinate to West
		//
		intXCoord -= 1;
		m_currentDirection = WEST;

		std::shared_ptr< Map > derreferencedMap;

		// If reference to shared pointer expired then exit
		if( m_map.expired() ) return false;

		derreferencedMap = m_map.lock();

		if( !derreferencedMap->IsInBounds( intXCoord, intYCoord )) return false;

		bool canMoveToTile = 
			derreferencedMap->InteractWithEntityAtTileLocation( this, intXCoord, intYCoord );

		if( canMoveToTile )
		{			
			// Updating my location
			UpdateAgentPosition( intXCoord, intYCoord, derreferencedMap.get() );
		}

		return canMoveToTile;
	}

	void Agent::UpdateInput( void )
	{		
		if( m_canBeControlled )
		{
			/* =============================================================
			* Navigation
			* ============================================================= */

			if( InputManager::Instance().IsKeyReleased( 'w' ) || InputManager::Instance().IsKeyReleased( 'W' ) ) MoveNorth();
			if( InputManager::Instance().IsKeyReleased( 's' ) || InputManager::Instance().IsKeyReleased( 'S' ) ) MoveSouth();
			if( InputManager::Instance().IsKeyReleased( 'a' ) || InputManager::Instance().IsKeyReleased( 'A' ) ) MoveWest();
			if( InputManager::Instance().IsKeyReleased( 'd' ) || InputManager::Instance().IsKeyReleased( 'D' ) ) MoveEast();
			if( InputManager::Instance().IsKeyReleased( 't' ) || InputManager::Instance().IsKeyReleased( 'T' ) ) DropItemFromInventory();

			/* =============================================================
			* Inventory
			* ============================================================= */

			// Load
			if( InputManager::Instance().IsKeyReleased( '1' ) )
			{
				bool loaded = LoadSlotWithItem( HEAD );
				if( !loaded )
				{
					ClearSlotFromItem( HEAD );
				}
			}
			
			if( InputManager::Instance().IsKeyReleased( '2' ) ) 
			{
				bool loaded = LoadSlotWithItem( CHEST );
				if( !loaded )
				{
					ClearSlotFromItem( CHEST );
				}
			}

			if( InputManager::Instance().IsKeyReleased( '3' ) ) 
			{
				bool loaded = LoadSlotWithItem( ARMS );
				if( !loaded )
				{
					ClearSlotFromItem( ARMS );
				}
			}

			if( InputManager::Instance().IsKeyReleased( '4' ) ) 
			{
				bool loaded = LoadSlotWithItem( LEGS );
				if( !loaded )
				{
					ClearSlotFromItem( LEGS );
				}
			}

			if( InputManager::Instance().IsKeyReleased( '5' ) ) 
			{
				LoadSlotWithItem( POTION_FIRE );
			}

			if( InputManager::Instance().IsKeyReleased( '6' ) ) 
			{
				LoadSlotWithItem( POTION_WATER );				
			}

			/* =============================================================
			* Combo binding
			* ============================================================= */

			if( InputManager::Instance().IsKeyReleased( 'r' ) || InputManager::Instance().IsKeyReleased( 'R' ) )
			{
				m_rangeMode = !m_rangeMode;
				if( m_rangeMode )
				{
					DEBUG( "Range mode engaged!" );
				}
				else
				{
					DEBUG( "Range mode disengaged!" );
				}
			}
		}		
	}	

	void Agent::UpdateSimulation( double deltaTime )
	{
		if( StringExtensions::CompareStrings( m_name, "hero" ))
		{
			//switch( m_currentDirection )
			//{
			//case NORTH:
			//	//m_texture = TextureManager::Instance().GetTexture( "avatar-north.png", 0.1f );
			//	break;
			//case SOUTH:
			//	//m_texture = TextureManager::Instance().GetTexture( "avatar-south.png", 0.1f );
			//	break;
			//case EAST:
			//	//m_texture = TextureManager::Instance().GetTexture( "avatar-east.png", 0.1f );
			//	break;
			//case WEST:
			//	//m_texture = TextureManager::Instance().GetTexture( "avatar-west.png", 0.1f );
			//	break;
			//default:
			//	m_texture = TextureManager::Instance().GetTexture( "avatar.png", 0.1f );
			//	break;
			//}
		}

		//if( !m_updateAlways ) return;
		m_deltaTime = deltaTime;

		srand(GetTickCount());

		// Torch
		if( m_hasTorch )
		{			
			double diff = m_deltaTime - m_torchPreviousDeltaTime;			
			if( diff > m_torchDeltaTime )
			{
				m_viewRadius = VIEW_RADIUS;
				m_hasTorch = false;
				m_torchPreviousDeltaTime = 0;
			}
		}
		
		if( m_conditionalMovement )
		{
			if( m_health <= m_conditionalHealth )
			{
				m_currentBehavior = m_behavior;
			}
			else
			{
				m_currentBehavior = NO_BEHAVIOR;
			}
		}

		if( m_canMove )
		{			
			double diff = deltaTime	- m_previousDeltaTime;

			if( diff > static_cast< double >( 1 * m_movementSpeed ) )
			{
				if( m_currentBehavior == INDIFFERENCE )
				{
					int randomOption = rand() % AMOUNT_OF_CARDINAL_MOVEMENTS;
					randomOption = rand() / AMOUNT_OF_CARDINAL_MOVEMENTS;

					switch( randomOption )
					{
					case 0:
						MoveNorth();
						break;
					case 1:
						MoveSouth();
						break;
					case 2:
						MoveEast();
						break;
					case 3:
						MoveWest();
						break;
					default:
						MoveEast();
					}
				}				
				else
				{
					size_t amountOfLocations = m_walkPath.size();
					if( amountOfLocations > 0 )
					{
						if( m_currentLocationIndex >= amountOfLocations ) return;

						Vector2 currentLocation = GetLocation();
						Vector2 nextLocation = m_walkPath[ m_currentLocationIndex ];

						bool validMove = false;

						if( nextLocation.y > currentLocation.y )
						{
							validMove = MoveNorth();
						}
						else if( nextLocation.y < currentLocation.y )
						{
							validMove = MoveSouth();
						}
						else if( nextLocation.x > currentLocation.x )
						{
							validMove = MoveEast();
						}
						else
						{
							validMove = MoveWest();
						}

						if( validMove )	++m_currentLocationIndex;						
					}					
				}				
				m_previousDeltaTime = deltaTime;
			}						
		}

		CalculateStats();
		m_updateAlways = false;

		for( auto item = m_equippedInventory.begin(); item != m_equippedInventory.end(); ++item )
		{
			if( ( *item ))
			{
				auto itemObject = dynamic_cast< Item* >( *item );
				if( itemObject )
				{
					itemObject->UpdateSimulation( deltaTime );

					if( itemObject->IsDeadItem() )
					{
						ClearSlotFromItem( itemObject->InventorySlot() );
						RemoveItem( itemObject );
						delete itemObject;
						break;
					}
				}				
			}
		}

		Entity::UpdateSimulation( deltaTime );
	}

	const bool Agent::IsItemEquipped( const std::string& itemName ) const
	{
		for( auto item = m_equippedInventory.begin(); item != m_equippedInventory.end(); ++item )
		{
			if( *item )
			{
				if( ( *item )->Name() == itemName )
					return true;
			}
		}
		return false;
	}

	void Agent::TakeDamage( float damage, bool forcedDamage )
	{
		if( forcedDamage )
		{
			if( m_health <= 1 )
			{
				m_health = 0;
				DEBUG( Name() << " has died!" );

				if( IsHero() )
				{
					MapManager::Instance().CurrentMap()->RemoveEntity( this );
					MapManager::Instance().CurrentMap()->SetHeroe( nullptr );
					delete this;
					Solidus::ReportError( "Game Over! (Please restart the app [for now])" );
				}

				return;
			}

			m_health -= damage;
			DEBUG( Name() << " received " << damage << " points damage!" );
			return;
		}

		srand( GetTickCount() );

		if( ( rand() % m_maxDodge ) > m_dodge + m_totalDodgeFromBag )
		{
			damage   -= ( ( m_toughness + m_totalToughnessFromBag ) * 0.3f ) / 2.0f;
			m_health -= damage;
			DEBUG( Name() << " received " << damage << " points damage!" );
		}
		else
		{
			DEBUG( Name() << " dodge the attack of " << damage << " points damage!" );
		}
	}

	float Agent::Attack()
	{
		float damage = 0.0f;
		srand( GetTickCount() );

		int x = rand() % m_maxDexterity;

		if( x < m_dexterity + m_totalDexterityFromBag )
		{
			damage += m_strength + m_totalStrengthFromBag + rand() % DEFAULT_HIT_DAMAGE;
			DEBUG( this->Name() << " inflicted damage of " << damage );
			return damage;
		}
		else
		{
			DEBUG( this->Name() << " attack MISSED!" );
			return damage;
		}
	}

	void Agent::CalculateStats()
	{
		float totalStrength  = 0.0f;
		int totalDexterity   = 0;
		float totalToughness = 0.0f;
		int totalDodge		 = 0;

		for( auto ptrToItem = m_equippedInventory.begin(); ptrToItem != m_equippedInventory.end(); ++ptrToItem )
		{
			auto itemCast = dynamic_cast< Item* >( *ptrToItem );
			if( itemCast )
			{
				totalStrength  += itemCast->Strength();
				totalDexterity += itemCast->Dexterity();
				totalToughness += itemCast->Toughness();
				totalDodge     += itemCast->Dodge();
			}
		}

		m_totalStrengthFromBag  = Clamp< float >( 0, m_maxStrength, totalStrength );
		m_totalDexterityFromBag = Clamp< int >( 0, m_maxDexterity, totalDexterity );
		m_totalToughnessFromBag = Clamp< float >( 0, m_maxToughness, totalToughness );
		m_totalDodgeFromBag     = Clamp< int >( 0, m_maxDodge, totalDodge );
	}

	bool Agent::ClearSlotFromItem( const InventorySlots& inventorySlot )
	{
		if( m_items.size() == 0 )
		{
			DEBUG( "Your item bag is empty!" );
		}

		for( auto ptrToItem = m_items.begin(); ptrToItem != m_items.end(); ++ptrToItem )
		{
			auto itemCast = dynamic_cast< Item* >( ptrToItem->second );
			if( itemCast )
			{
				if( itemCast->InventorySlot() == inventorySlot )
				{
					itemCast->UnEquip();
					ReleaseSlot( itemCast->InventorySlot() );
					return true;
				}
			}
		}

		return false;
	}

	bool Agent::LoadSlotWithItem( const InventorySlots& inventorySlot )
	{
		if( m_items.size() == 0 )
		{
			DEBUG( "Your item bag is empty!" );
		}

		for( auto ptrToItem = m_items.begin(); ptrToItem != m_items.end(); ++ptrToItem )
		{
			auto itemCast = dynamic_cast< Item* >( ptrToItem->second );
			if( itemCast )
			{
				if( itemCast->InventorySlot() == inventorySlot )
				{
					bool equipped = EquipSlot( inventorySlot, itemCast );
					if( equipped )
					{
						itemCast->Equip();
						DEBUG( itemCast->Name() << " was loaded into " << GetStringFromInventorySlot( inventorySlot ) << " slot successfully!" );
					}
					else			
					{
						DEBUG( m_equippedInventory[ GetValueFromInventorySlot( inventorySlot ) ]->Name() << " is already loaded into " << GetStringFromInventorySlot( inventorySlot ) << " slot." );
					}
					return equipped;
				}
			}
		}
		return false;
	}

	void Agent::RangedAttack( int x, int y )
	{
		std::shared_ptr< Map > derreferencedMap;

		// If reference to shared pointer expired then exit
		if( m_map.expired() ) return;

		derreferencedMap = m_map.lock();

		if( !derreferencedMap->IsInBounds( x, y )) return;

		bool tileOccupied = derreferencedMap->InteractWithEntityAtTileLocation( this, x, y );
		if( tileOccupied )
		{
			DEBUG("Someone was ranged attacked! at [" << x << ", " << y << "]" );
		}
		else
		{
			DEBUG("There's no one at [" << x << ", " << y << "]" );
		}
	}

	void Agent::FindTarget( NamedProperties& eventArgs )
	{
		if( m_currentBehavior == HATE )
		{		
			m_currentLocationIndex = 0; // Resetting the walkable location

			Entity* target;
			eventArgs.GetProperty( "Target", target );

			if( !target ) return;

			std::shared_ptr< Map > derreferencedMap;

			// If reference to shared pointer expired then exit
			if( m_map.expired() ) return;

			derreferencedMap = m_map.lock();

			//
			if( derreferencedMap && target )
			{
				m_walkPath = AStar::Instance().FindPath( derreferencedMap.get(), this, target );
			}			
		}
	}

	void Agent::EscapeToTarget( int x, int y, const Vector2& heroLocation )
	{
		std::shared_ptr< Map > derreferencedMap;

		// If reference to shared pointer expired then exit
		if( m_map.expired() ) return;

		derreferencedMap = m_map.lock();

		//
		if( derreferencedMap )
		{
			m_currentLocationIndex = 0;
			m_walkPath = AStar::Instance().FindPath( derreferencedMap.get(), this->GetLocation(), Vector2( x, y ), heroLocation );
		}
	}

	void Agent::CheckIfHeroMovedOnTheMap()
	{
		if( StringExtensions::CompareStrings( m_name, "hero" ))
		{
			NamedProperties args;
			Entity* target = this;
			args.SetProperty( "Target", target );

			g_EventManager.FireEvent( "FindTarget", args );
			g_EventManager.FireEvent( "CalculateDirectionFromHero", args );
		}
	}

	void Agent::UpdateAgentPosition( int newXCoordinate, int newYCoordinate, Map* currentMap )
	{
		int intCurrentXCoord = static_cast< int >( m_x );
		int intCurrentYCoord = static_cast< int >( m_y );		

		NamedProperties args;
		Agent* myReference = this;
		args.SetProperty( "Agent", myReference );

		// Removing from current tile
		currentMap->GetTileAtLocation( intCurrentXCoord, intCurrentYCoord )->MovedFromEvent( args );

		// Deleting my parent reference (tile)					
		m_parent = nullptr;		

		// This will break the update cycle in the map and avoid issues with broken iterators
		currentMap->SkipUpdate(); 

		// Adding the agent to its new parent
		currentMap->GetTileAtLocation( newXCoordinate, newYCoordinate )->MovedToEvent( args );		

		// If I'm not the hero let me refresh my path towards him
		CheckIfHeroMovedOnTheMap();
	}

	void Agent::CalculateDirectionFromHero( NamedProperties& eventArgs )
	{
		if( m_currentBehavior == FEAR )
		{
			Entity* target;
			eventArgs.GetProperty( "Target", target );

			if( !target ) return;

			srand( GetTickCount() );

			Vector2 targetLocation = target->GetLocation();
			Vector2 myLocation	   = GetLocation();

			float length = ( targetLocation - myLocation ).CalcLengthSquared();

			if( length < ( DISTANCE_RADIUS * DISTANCE_RADIUS ) )
			{
				std::shared_ptr< Map > derreferencedMap;

				// If reference to shared pointer expired then exit
				if( m_map.expired() ) return;

				derreferencedMap = m_map.lock();				
				
				float x = 0.0f;
				float y = 0.0f;

				do 
				{
					int randomAngle = rand() % 360;
					x = m_x + DISTANCE_RADIUS + (( 2 * DISTANCE_RADIUS ) * cos( ConvertToRadians( static_cast< float >( randomAngle ))));
					y = m_y + DISTANCE_RADIUS + (( 2 * DISTANCE_RADIUS ) * sin( ConvertToRadians( static_cast< float >( randomAngle ))));
				} 
				while ( !derreferencedMap->IsInBounds( static_cast< int >( x ), static_cast< int >( y )) && !derreferencedMap->GetTileAtLocation( static_cast< int >( x ), static_cast< int >( y ))->IsSolid() );
				
				// Add the hero to the blocked paths				

				EscapeToTarget( static_cast< int >( x ), static_cast< int >( y ), targetLocation );				
			}
		}				
	}

	void Agent::CheckUpdateRadius( int x, int y )
	{
		std::shared_ptr< Map > derreferencedMap;

		// If reference to shared pointer expired then exit
		if( m_map.expired() ) return;//return false;

		derreferencedMap = m_map.lock();

		// Get the hero up
		auto hero = derreferencedMap->GetHero();

		if( hero )
		{
			Vector2 heroLocation = hero->GetLocation();

			NamedProperties args;
			Entity* target = this;
			args.SetProperty( "Target", target );

			auto agentInRadius = derreferencedMap->
				GetTileAtLocation( x, y )->HasAgents();
			if( agentInRadius && !agentInRadius->UpdateAlways() )
			{
				agentInRadius->CalculateDirectionFromHero( args );
				agentInRadius->m_updateAlways = true;
				//agentInRadius->UpdateSimulation( m_deltaTime );
				//agentInRadius->Draw( Solidus::Renderer2D::Instance() );
			}
		}
	}

	int Agent::PointVisible( int x, int y )
	{
		int intXCoordinate = static_cast< int >( m_x );
		int intYCoordinate = static_cast< int >( m_y );

		int offset_x = 0;
		int offset_y = 0;

		std::shared_ptr< Map > derreferencedMap;

		// If reference to shared pointer expired then exit
		if( m_map.expired() ) return 0;//return false;

		derreferencedMap = m_map.lock();

		switch ( m_currentDirection ) 
		{
		case NORTH: 
			offset_y = -LIGHT_OFFSET;
			break;
		case SOUTH: 
			offset_y = LIGHT_OFFSET;
			break;
		case EAST: 
			offset_x = -LIGHT_OFFSET;
			break;
		case WEST: 
			offset_x = LIGHT_OFFSET;
			break;
		}

		if ( intXCoordinate + offset_x == x && intYCoordinate + offset_y == y ) 
		{
			return 0;
		}

		int dx = x - intXCoordinate;
		int dy = y - intYCoordinate;

		if (dx * dx + dy * dy > m_viewRadius * m_viewRadius ) 
		{
			return 0;
		}

		float phi = atan2( static_cast< float >( dy - offset_y ), static_cast< float >( dx - offset_x ));

		if (m_currentDirection == NORTH) 
		{
			phi -= PI * 0.5f;
		} 
		else if (m_currentDirection == SOUTH) 
		{
			phi += PI * 0.5f;
		} 
		else if (m_currentDirection == WEST) 
		{
			if ( phi > 0.0f ) 
			{
				phi -= PI;
			} 
			else 
			{
				phi += PI;
			}
		}

		if (phi >= -m_fieldOfView && phi <= m_fieldOfView)
		{
			auto tracedTiles = derreferencedMap->RayTraceToTiles( intXCoordinate, intYCoordinate,
				x, y );

			for( size_t index = 0; index < tracedTiles.size(); ++index )
			{
				Vector2 currentLocation( tracedTiles[ index ]->GetLocation() );
				if (tracedTiles[ index ]->IsSolid() ) 
				{
					if ( currentLocation.x != x || currentLocation.y != y ) 
					{
						return 0;
					}
				}
			}
			return static_cast< int >( tracedTiles.size() );
		}
		return 0;
	}

	void Agent::PickedTorch()
	{
		 m_torchDeltaTime += m_deltaTime + TORCH_LIFETIME;
		 m_torchPreviousDeltaTime = 0;
		 m_hasTorch = true;
		 m_viewRadius += 2;
	}

	Agent* Agent::Clone()
	{
		return new Agent( *this );
	}

	bool Agent::CanLoadSlot( const InventorySlots& inventorySlot )
	{
		if( m_items.size() == 0 )
		{
			DEBUG( "Your item bag is empty!" );
		}

		for( auto ptrToItem = m_items.begin(); ptrToItem != m_items.end(); ++ptrToItem )
		{
			auto itemCast = dynamic_cast< Item* >( ptrToItem->second );
			if( itemCast )
			{
				if( itemCast->InventorySlot() == inventorySlot )
				{
					return false;
				}
			}
		}
		return true;
	}

	void Agent::DropItemFromInventory()
	{
		std::shared_ptr< Map > derreferencedMap;

		// If reference to shared pointer expired then exit
		if( m_map.expired() ) return;

		// Locking map
		derreferencedMap = m_map.lock();

		// Verifying the reference is not empty
		if( !derreferencedMap ) return;		

		int intXCoord = static_cast< int >( m_x );
		int intYCoord = static_cast< int >( m_y );

		auto currentTile = derreferencedMap->GetTileAtLocation( intXCoord, intYCoord );
		for( auto ptrToItem = m_items.begin(); ptrToItem != m_items.end(); ++ptrToItem )
		{
			auto itemCast = dynamic_cast< Item* >( ptrToItem->second );
			if( itemCast )
			{
				if( itemCast->InventorySlot() != NO_SLOT )
				{
					// Clear slot
					//
					ClearSlotFromItem( itemCast->InventorySlot() );
					RemoveItem( itemCast );
					currentTile->AddItem( itemCast );
					break;
				}				
			}
		}
	}
}