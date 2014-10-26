#include "stdafx.h"
#include "Tile.h"
#include "TileType.h"
#include "Agent.h"
#include "EventSystem.h"
#include "StateManager.h"

namespace Solidus
{
	const int SHRINK_CONTROL_THRESHOLD = 35;
	Tile::Tile()
		: Entity( "default", false, StringExtensions::EmptyString, 1000.0f, 0.0f, false, false, -1 )
		, m_isListDirty( false )
		, m_known( false )
	{}


	Tile::Tile( const Tile& cSource )
		: Entity( cSource )
		, m_tileType( cSource.m_tileType )
		, m_worldUnits( cSource.m_worldUnits )
		, m_isListDirty( cSource.m_isListDirty )	
		, m_known( cSource.m_known )
		, m_lastUpdated( cSource.m_lastUpdated )
	{
		Translate( cSource.m_x, cSource.m_y );
	}


	Tile::Tile( std::string name, std::shared_ptr< TileType > tileType, float worldUnits, float maxHealth, float health, bool canBeDamaged, Vector2 initialPosition, Vector4 color )
		: Entity( name, false, StringExtensions::EmptyString, maxHealth, health, false, canBeDamaged, -1, StringExtensions::EmptyString, 
		Vector2::ZeroVector, initialPosition, color ) // TODO: FIX THIS
		, m_tileType( tileType )
		, m_worldUnits( worldUnits )
		, m_isListDirty( false )	
		, m_known( false )
	{
		Translate( initialPosition.x, initialPosition.y );		
	}	
	void Tile::Draw( const Renderer2D& painter )
	{					
		for( auto ptrToItem = m_items.begin(); ptrToItem != m_items.end(); ++ptrToItem )
		{			
			painter.DrawEntity( ptrToItem->second );			
		}		
	}

	void Tile::LoadContent( void )
	{
		for( auto ptrToItem = m_items.begin(); ptrToItem != m_items.end(); ++ptrToItem )
		{
			ptrToItem->second->LoadContent();			
		}
	}

	void Tile::UpdateSimulation( double deltaTime )
	{	
		m_deltaTime = deltaTime;
		size_t currentSize = m_items.size();
		for( auto ptrToItem = m_items.begin(); ptrToItem != m_items.end(); ++ptrToItem )
		{			
			// Cause damage!
			//
			CauseDamageToEntity( ptrToItem->second );

			ptrToItem->second->UpdateSimulation( deltaTime );			

			if( currentSize != m_items.size() )
				break;			
		}

		CheckForDeadElementsIOwn();
		Entity::UpdateSimulation( deltaTime );
	}

	void Tile::UpdateInput( void )
	{		
		size_t currentSize = m_items.size();
		for( auto ptrToItem = m_items.begin(); ptrToItem != m_items.end(); ++ptrToItem )
		{
			ptrToItem->second->UpdateInput();
			if( currentSize != m_items.size() )
				break;
		}
	}

	const bool Tile::IsSolid() const
	{
		// If reference to shared pointer expired then exit
		if( !m_tileType ) false;		

		return m_tileType->IsSolid;
	}

	const bool Tile::CausesHarm() const
	{
		// If reference to shared pointer expired then exit
		if( !m_tileType ) false;		

		return m_tileType->DamageBehavior != NO_BEHAVIOR;
	}

	void Tile::CheckForDeadElementsIOwn()
	{
		for( auto ptrToItem = m_items.begin(); ptrToItem != m_items.end(); ++ptrToItem )
		{
			if( ptrToItem->second->Health() <= 0.0f || ptrToItem->second->GetScale() == Vector2::ZeroVector )
			{				
				Entity* cpy = RemoveItem( ptrToItem->second );
				auto currentAgent = dynamic_cast< Agent* >( cpy );
				if( currentAgent )
				{
					g_EventManager.UnRegister( currentAgent, &Agent::FindTarget, "FindTarget" );
					
					if( currentAgent->IsHero() )
					{
						MapManager::Instance().CurrentMap()->RemoveEntity( this );
						MapManager::Instance().CurrentMap()->SetHeroe( nullptr );
						delete cpy;
						Solidus::ReportError( "Game Over! (Please restart the app [for now])" );
						break;
					}
				}				

				delete cpy;
				break;
			}
		}
	}

	void Tile::MovedToEvent( NamedProperties& eventArgs )
	{
		Agent* agentThatLandedOn;
		eventArgs.GetProperty( "Agent", agentThatLandedOn );
		
		if( !agentThatLandedOn ) return;

		// Translating 
		agentThatLandedOn->Translate( m_x, m_y );
		//agentThatLandedOn->IgnoreInput( false );

		// Adding the item from the inventory
		AddItem( agentThatLandedOn );
		//DEBUG( agentThatLandedOn->Name() << " moved to tile at [" << m_x << ", " << m_y << "]" );
	}

	void Tile::MovedFromEvent( NamedProperties& eventArgs )
	{
		Agent* agentThatLandedOff;
		eventArgs.GetProperty( "Agent", agentThatLandedOff );

		if( !agentThatLandedOff ) return;

		// Removing the item from the inventory
		RemoveItem( agentThatLandedOff );
		//DEBUG( agentThatLandedOff->Name() << " moved from tile at [" << m_x << ", " << m_y << "]" );
	}

	Tile* Tile::Clone()
	{
		return new Tile( *this );
	}

	void Tile::CauseDamageToEntity( Entity* entity )
	{
		if( StateManager::Instance().IsPaused() ) return;

		srand( GetTickCount() );
		
		Agent* agent = dynamic_cast< Agent* >( entity );
		
		// If its not an agent then do nothing to them.
		//
		if( !agent ) return;

		// If damage behavior is none then do nothing!
		//
		if( m_tileType->DamageBehavior == NO_BEHAVIOR ) 
		{
			agent->SetScale( 0.01f, 0.01f );
			return;
		}

		// Calculate the time that has passed
		//
		double diff = m_deltaTime - m_previousDeltaTime;
		if( diff > static_cast< double >( m_tileType->DamageSpeed ) )
		{
			switch( m_tileType->DamageBehavior )
			{
				// TODO: Check if the user has an item that repels this damage
				// It will have to do it through the name of the tile type and the damage type
			case TileType::HURT:
				if( !agent->IsItemEquipped( m_tileType->DoesNotWorkAgainst ))
				{
					agent->TakeDamage( entity->Health() * m_tileType->DamageRatePerSecond, true );
				}				
				break;
			case TileType::SHRINK:	
				if( !agent->IsItemEquipped( m_tileType->DoesNotWorkAgainst ))
				{
					// Leave to luck if user controls get blocked!
					//
					int canBeControlled = rand() % 100;

					if( StringExtensions::CompareStrings( agent->Name(), "hero" ))
					{
						agent->IgnoreInput( canBeControlled <= SHRINK_CONTROL_THRESHOLD );
					}					

					// Alter the scale
					//
					agent->AlterSetScale( m_tileType->DamageRatePerSecond, m_tileType->DamageRatePerSecond );
				
					// Kill the agent
					//
					Vector2 entityScale = entity->GetScale();
					if( entityScale.x <= 0.0f || entityScale.y <= 0.0f  )
					{
						agent->TakeDamage( entity->Health(), true );
					}
				}
				break;
			}

			m_previousDeltaTime = m_deltaTime;
		}
	}
}