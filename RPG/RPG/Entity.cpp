#include "stdafx.h"
#include "Entity.h"
#include "TextureManager.h"
#include "Item.h"
#include "MathUtilities.h"
#include "Agent.h"
#include "StateManager.h"

namespace Solidus
{
	Entity::Entity( const Entity& cSource )
		: GameObject( cSource.m_name, cSource.m_maxHealth, cSource.m_canBeDamanged, cSource.m_health )
		, m_color( cSource.m_color )
		, m_texturePath( cSource.m_texturePath )
		, m_canBeEquipped( cSource.m_canBeEquipped )
		, m_blocksPosition( cSource.m_blocksPosition )
		, m_entityThatUnblocks( cSource.m_entityThatUnblocks )
		, m_parent( cSource.m_parent )
		, m_destination( cSource.m_destination )
		, m_warpTo( cSource.m_warpTo )
		, m_deltaTime( cSource.m_deltaTime )
		, m_previousDeltaTime( cSource.m_previousDeltaTime )
	{
		m_inventoryItemsLimit = ( cSource.m_inventoryItemsLimit );
		Translate( cSource.m_x, cSource.m_y );

		for( auto item = cSource.m_items.begin(); item != cSource.m_items.end(); ++item )
		{
			AddItem( (*item).second );
		}
	}

	Entity::Entity( 
		std::string name, 
		bool blocksPosition, 
		std::string entityThatUnblocks, 
		float maxHealth, 
		float health, 
		bool canBeEquipped, 
		bool canBeDamaged, 
		int inventoryLimit, 
		std::string destination, 
		Vector2 warpTo, 
		Vector2 initialPosition, 
		Vector4 color )
		: GameObject( name, maxHealth,  canBeDamaged, health )		
		, m_color( color )	
		, m_texturePath( StringExtensions::EmptyString )
		, m_canBeEquipped( canBeEquipped )
		, m_blocksPosition( blocksPosition )
		, m_entityThatUnblocks( entityThatUnblocks )
		, m_parent( 0 )
		, m_destination( destination )
		, m_warpTo( warpTo )
		, m_deltaTime( 0.0 )
		, m_previousDeltaTime( 0.0 )
	{
		m_inventoryItemsLimit = ( inventoryLimit );
		Translate( initialPosition.x, initialPosition.y );
	}

	Entity::Entity( 
		std::string name, 
		bool blocksPosition, 
		std::string entityThatUnblocks, 
		const char* texturePath, 
		float textureScale, 
		float maxHealth,
		float health, 
		bool canBeEquipped, 
		bool canBeDamaged, 
		int inventoryLimit, 
		std::string destination, 
		Vector2 warpTo, 
		Vector2 initialPosition, 
		Vector4 color )
		: GameObject( name, maxHealth,  canBeDamaged, health )
		, Sprite( texturePath, textureScale, textureScale )
		, m_color( color )	
		, m_texturePath( texturePath )
		, m_canBeEquipped( canBeEquipped )		
		, m_blocksPosition( blocksPosition )
		, m_entityThatUnblocks( entityThatUnblocks )
		, m_parent( 0 )
		, m_destination( destination )
		, m_warpTo( warpTo )
		, m_deltaTime( 0.0 )
		, m_previousDeltaTime( 0.0 )
	{
		m_inventoryItemsLimit = ( inventoryLimit );
		Translate( initialPosition.x, initialPosition.y );		
	}

	Entity::~Entity( void )
	{}

	void Entity::Draw( const Renderer2D& painter )
	{		
		painter.DrawEntity( this );
	}

	void Entity::Init()
	{}	

	void Entity::UpdateInput( void )
	{}

	void Entity::UpdateSimulation( double deltaTime )
	{
		if( deltaTime > 0 ) 
		{
			StateManager::Instance().SaveEntityState( this );
		}
	}
	
	bool Entity::AddItem( Entity * item )
	{
		if( IsInventoryFull() ) return false;

		// Adding the item to the inventory
		m_previousItemAddedToInventoryId = m_lastItemAddedToInventoryId;
		m_lastItemAddedToInventoryId = item->Id();
		item->m_isDirty = false;
		item->m_parent = this;
		item->Translate( m_x, m_y );
		m_items.insert( std::pair< int, Entity* >( m_lastItemAddedToInventoryId, item ));
		
		return true;
	}

	bool Entity::UseItem( const std::string& itemName )
	{
		for( auto ptrToItem = m_items.begin(); ptrToItem != m_items.end(); ++ptrToItem )
		{
			if( StringExtensions::CompareStrings( ptrToItem->second->Name(), itemName ))
			{
				auto removedItem = RemoveItem( ptrToItem->second );
				delete removedItem;
				return true;
			}
		}
		return false;
	}

	Entity* Entity::FindItem( Entity* item )
	{
		if( m_items.size() > 0 )
		{
			std::map< int, Entity* >::iterator ptrToLastItemAddedToInventory 
				= m_items.find( item->Id() );

			// Did we find the item?
			//
			if( ptrToLastItemAddedToInventory != m_items.end() )
			{
				return ptrToLastItemAddedToInventory->second;				
			}			
		}
		return nullptr;
	}

	Entity* Entity::RemoveItem( Entity* item )
	{
		if( m_items.size() > 0 )
		{
			std::map< int, Entity* >::iterator ptrToLastItemAddedToInventory 
				= m_items.find( item->Id() );

			// Did we find the item?
			//
			if( ptrToLastItemAddedToInventory != m_items.end() )
			{
				Entity * copy = ptrToLastItemAddedToInventory->second;
				m_items.erase( ptrToLastItemAddedToInventory );			
				m_lastItemAddedToInventoryId = m_previousItemAddedToInventoryId;
				return copy;
			}			
		}
		return nullptr;
	}

	Entity* Entity::GetLastItem() 
	{
		if( m_items.size() > 0 )
		{
			std::map< int, Entity* >::iterator ptrToLastItemAddedToInventory 
				= m_items.find( m_lastItemAddedToInventoryId );

			// Did we find the item?
			//
			if( ptrToLastItemAddedToInventory != m_items.end() )
			{
				return ptrToLastItemAddedToInventory->second;				
			}			
		}
		return nullptr;
	}

	Entity* Entity::GetPenultimateItem() 
	{
		if( m_items.size() > 0 && m_previousItemAddedToInventoryId > 0 )
		{
			if( m_previousItemAddedToInventoryId == m_lastItemAddedToInventoryId)
			{
				auto firstElement = m_items.begin();
				m_lastItemAddedToInventoryId = firstElement->first;
				return (firstElement->second);
			}

			std::map< int, Entity* >::iterator ptrToLastItemAddedToInventory 
				= m_items.find( m_previousItemAddedToInventoryId );
			
			// Did we find the item?
			//
			if( ptrToLastItemAddedToInventory != m_items.end() )
			{
				return ptrToLastItemAddedToInventory->second;				
			}			
		}
		return nullptr;
	}

	Entity* Entity::RemoveLastItem() // Drop last picked
	{
		if( m_items.size() > 0 )
		{
			std::map< int, Entity* >::iterator ptrToLastItemAddedToInventory 
				= m_items.find( m_lastItemAddedToInventoryId );
			
			// Did we find the item?
			//
			if( ptrToLastItemAddedToInventory != m_items.end() )
			{
				Entity * copy = ptrToLastItemAddedToInventory->second;
				m_items.erase( ptrToLastItemAddedToInventory );
				m_lastItemAddedToInventoryId = m_previousItemAddedToInventoryId;
				return copy;
			}			
		}
		return nullptr;
	}

	void Entity::ReceiveHealing( float health )
	{
		// Adding the health
		//
		m_health += health;

		// Clamping the health
		//
		m_health = Clamp< float >( 0, m_maxHealth, m_health );
	}

	void Entity::TakeDamage( float damage,  bool forcedDamage )
	{						
		if( forcedDamage || !forcedDamage ) m_health -= damage;
	}

	Agent* Entity::HasAgents()
	{
		for( auto itemIterator = m_items.begin(); itemIterator != m_items.end(); ++itemIterator )
		{
			Entity* entity = itemIterator->second;
			auto agent = dynamic_cast< Agent* >( entity );
			if( agent )
			{
				return agent;
			}
		}
		return nullptr;
	}

	Entity* Entity::Clone()
	{
		return new Entity( *this );
	}

	void Entity::LoadContent( void )
	{
		if( StringExtensions::CompareStrings( m_name, "door" ) )
		{
			if( m_blocksPosition )
			{
				m_texture = TextureManager::Instance().GetTexture( "door_closed.png", 0.1f );
			}
		}
	}

	void Entity::UnBlock()
	{
		m_blocksPosition = false;
		m_texture = TextureManager::Instance().GetTexture( m_texturePath.c_str(), 0.1f );
	}

	void Entity::ValidateInitialLocation()
	{
		if( m_x == 0 && m_y == 0 )
		{
			srand( GetTickCount() );
			std::shared_ptr< Map > derreferencedMap;

			// If reference to shared pointer expired then exit
			if( m_map.expired() ) return;

			// Locking map
			derreferencedMap = m_map.lock();

			// Verifying the reference is not empty
			if( !derreferencedMap ) return;

			int x = rand() % derreferencedMap->Width();
			int y = rand() % derreferencedMap->Height();
			std::shared_ptr< Tile > tile = derreferencedMap->GetTileAtLocation( x, y );

			while( tile->IsSolid() || tile->CausesHarm() || tile->HasItems() )
			{
				x = rand() % derreferencedMap->Width();
				y = rand() % derreferencedMap->Height();
				tile = derreferencedMap->GetTileAtLocation( x, y );
			}

			Translate( static_cast< float >( x ), static_cast< float >( y ) );
		}		
	}

}