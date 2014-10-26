#include "stdafx.h"
#include "Item.h"
#include "TextureManager.h"

namespace Solidus
{

	Item::Item( const Item& cSource )
		: Entity( cSource )
		, IStats( cSource.m_strength, cSource.m_dexterity, cSource.m_toughness, cSource.m_dodge )	
		, m_inventorySlot( cSource.m_inventorySlot )
	{
		Translate( cSource.m_x, cSource.m_y );
	}

	Item::Item( std::string name, int effectInSeconds, bool singleUse, const char* texturePath, float textureScale, InventorySlots inventorySlot, 
		float maxHealth, float health, bool canBeEquiped, bool canBeDamaged, float strength, int dexterity, 
		float toughness, int dodge, Vector2 initialPosition )
		: Entity( name, false, StringExtensions::EmptyString, texturePath, textureScale, maxHealth, health, canBeEquiped, 
		canBeDamaged, 0, StringExtensions::EmptyString, Vector2::ZeroVector, initialPosition )
		, IStats( strength, dexterity, toughness, dodge )	
		, m_inventorySlot( inventorySlot )
		, m_effectInSeconds( effectInSeconds )
		, m_equipped( false )
		, m_singleUse( singleUse )
		, m_remainingSeconds( effectInSeconds )
	{
		Translate( initialPosition.x, initialPosition.y );
	}

	Item::~Item( void )
	{}

	void Item::Draw( const Renderer2D& painter )
	{		
		painter.DrawEntity( this );
	}

	void Item::Init()
	{}	

	void Item::UpdateInput( void )
	{			
		Entity::UpdateInput();
	}

	void Item::UpdateSimulation( double deltaTime )
	{		
		m_deltaTime = deltaTime;
		
		if( m_equipped )
		{
			CheckItemLifetime();
		}		
		else
		{
			m_previousDeltaTime = m_deltaTime;
		}

		Entity::UpdateSimulation( deltaTime );
	}

	Item* Item::Clone()
	{
		return new Item( *this );
	}

	void Item::CheckItemLifetime()
	{
		if( m_effectInSeconds > 0 && m_remainingSeconds > 0 )
		{
			// Calculate the time that has passed
			//
			double diff = m_deltaTime - m_previousDeltaTime;
			if( diff > 1 )
			{
				m_remainingSeconds--;
				DEBUG( "The effect of " << m_name << " will last for " << m_remainingSeconds << " seconds!" )
				m_previousDeltaTime = m_deltaTime;
			}

			if( m_remainingSeconds == 0 )
			{
				m_equipped = false;
			}
		}
	}
}