#pragma once

#ifndef _BLUEPRINT_H_
#define _BLUEPRINT_H_

#include "Entity.h"
#include "BluePrintBase.h"
#include "XMLExtensions.h"
#include "Agent.h"
#include "Vector2.h"
#include "Item.h"
#include "Tile.h"

namespace Solidus
{		
	/* =============================================================
	* BLUE PRINT BASE TEMPLATE IMPLEMENTATION
	* ============================================================= */

	template <class T >
	class BluePrint : public BluePrintBase
	{		
	private:
		XMLNode m_node;
	public:
		BluePrint( const XMLNode& node )
			: m_node( node )
		{}

		virtual BluePrintBase* Clone() const
		{
			return new BluePrint<T>( m_node );
		}	
	};

	/* =============================================================
	* BLUE PRINT ENTITY TEMPLATE SPECIALIZATION
	* ============================================================= */
	template <>
	class BluePrint< Entity > : public BluePrintBase
	{
	private:
		XMLNode		m_node;
		Vector2		m_position;
		Vector4		m_color;	
		float		m_health;
		float		m_maxHealth;
		std::string m_name;
		bool		m_canBeDamaged;
		bool		m_blocksPosition;
		std::string m_entityThatUnblocks;
		int			m_inventoryItemsLimit;
		std::string m_texturePath;
		float       m_scale;
		std::string m_destination;
		Vector2     m_warpTo;		
	public:
		BluePrint( const XMLNode& node )
			: m_node( node )
		{
			m_health			  = GetFloatXMLAttribute( node, "health", 0.0f );
			m_maxHealth			  = GetFloatXMLAttribute( node, "maxHealth", 1000.0f );
			m_canBeDamaged		  = GetBoolXMLAttribute( node, "canBeDamaged", false );
			m_blocksPosition	  = GetBoolXMLAttribute( node, "blocksPosition", true );
			m_entityThatUnblocks  = GetStringXMLAttribute( node, "entityThatUnblocks", "" );
			m_name				  = GetStringXMLAttribute( node, "name", "UNNAMED" );
			m_color				  = GetRGBAXMLAttribute( node, "color", Vector4::EmptyColor );
			m_position			  = GetVector2XMLAttribute( node, "position", Vector2::ZeroVector );
			m_inventoryItemsLimit = GetIntXMLAttribute( node, "inventoryLimit", -1 );
			m_texturePath		  = GetStringXMLAttribute( node, "texture", "entity.png" );
			m_scale				  = GetFloatXMLAttribute( node, "scale", 0.01f );
			m_destination		  = GetStringXMLAttribute( node, "Destination", "" );
			m_warpTo			  = GetVector2XMLAttribute( node, "WarpTo", Vector2::ZeroVector );			
		}

		Entity * Create()
		{			
			return new Entity( m_name, m_blocksPosition, m_entityThatUnblocks, m_texturePath.c_str(), m_scale, m_maxHealth, 
				m_health, false, m_canBeDamaged, m_inventoryItemsLimit, m_destination, m_warpTo, 
				m_position, m_color );
		}

		virtual BluePrintBase* Clone() const
		{
			return new BluePrint< Entity * >( m_node );
		}	
	};

	/* =============================================================
	* BLUE PRINT AGENT TEMPLATE SPECIALIZATION
	* ============================================================= */
	template <>
	class BluePrint< Agent > : public BluePrintBase
	{
	private:
		XMLNode		 m_node;
		Vector4		 m_color;
		bool		 m_canMove;
		float		 m_health;
		float		 m_maxHealth;
		Vector2		 m_position;
		std::string  m_name;
		bool		 m_canBeControlled;
		float		 m_movementSpeed;
		float		 m_strength;
		int			 m_dexterity;
		float		 m_toughness;
		int			 m_dodge;
		bool		 m_canBeDamaged;		
		int			 m_inventoryItemsLimit;
		BehaviorType m_behavior;
		float		 m_conditionalHealth;
		bool		 m_conditionalMovement;
		std::string	 m_texturePath;
		float		 m_scale;
		bool		 m_updateAlways;
		bool		 m_drawAlways;
	public:
		BluePrint( const XMLNode& node )
			: m_node( node )
		{
			m_name			      = GetStringXMLAttribute( node, "name", "UNNAMED" );
			m_canMove		      = GetBoolXMLAttribute( node, "canMove", false );
			m_canBeControlled     = GetBoolXMLAttribute( node, "canBeControlled", false );
			m_canMove		      = GetBoolXMLAttribute( node, "canMove", false );
			m_health		      = GetFloatXMLAttribute( node, "health", 1000.0f );
			m_maxHealth			  = GetFloatXMLAttribute( node, "maxHealth", 1000.0f );
			m_canBeDamaged	      = GetBoolXMLAttribute( node, "canBeDamaged", false );
			m_movementSpeed	      = GetFloatXMLAttribute( node, "movementSpeed", 1.0f );
			m_position		      = GetVector2XMLAttribute( node, "position", Vector2::ZeroVector );
			m_color			      = GetRGBAXMLAttribute( node, "color", Vector4::EmptyColor );
			m_inventoryItemsLimit = GetIntXMLAttribute( node, "inventoryLimit", -1 );
			m_behavior			  = AI::GetBehaviorFromString( GetStringXMLAttribute( node, "behavior", "2" ) );
			m_conditionalHealth   = GetFloatXMLAttribute( node, "conditionalHealth", m_health );
			m_conditionalMovement = ( m_conditionalHealth == m_health );
			m_texturePath		  = GetStringXMLAttribute( node, "texture", "default_alpha.png" );
			m_scale				  = GetFloatXMLAttribute( node, "scale", 0.01f );
			m_updateAlways		  = GetBoolXMLAttribute( node, "updateAlways", false );
			m_drawAlways		  = GetBoolXMLAttribute( node, "drawAlways", false );

			// Stats
			m_strength			  = GetFloatXMLAttribute( node, "strength", 1.0f );
			m_dexterity			  = GetIntXMLAttribute( node, "dexterity", 1 );
			m_toughness			  = GetFloatXMLAttribute( node, "toughness", 1.0f );
			m_dodge				  = GetIntXMLAttribute( node, "dodge", 1 );
		}

		Agent * Create()
		{			
			return new Agent( m_name, m_texturePath.c_str(), m_scale, m_updateAlways, m_drawAlways, m_inventoryItemsLimit, 
				m_maxHealth, m_health, m_canBeDamaged, m_canBeControlled, m_canMove, m_movementSpeed, 
				m_behavior, m_conditionalHealth, m_conditionalMovement, m_strength, m_dexterity, 
				m_toughness, m_dodge, m_position, m_color );
		}

		virtual BluePrintBase* Clone() const
		{
			return new BluePrint< Agent * >( m_node );
		}	
	};

	/* =============================================================
	* BLUE PRINT ITEM TEMPLATE SPECIALIZATION
	* ============================================================= */
	template <>
	class BluePrint< Item > : public BluePrintBase
	{
	private:
		XMLNode		   m_node;		
		std::string    m_name;
		Vector2		   m_position;
		bool		   m_canBeDamaged;		
		float		   m_health;
		float		   m_maxHealth;
		float		   m_strength;
		int			   m_dexterity;
		float		   m_toughness;
		int			   m_dodge;
		InventorySlots m_inventorySlot;		 
		std::string    m_texturePath;
		float          m_scale;
		int			   m_effectInSeconds;
		bool		   m_singleUse;
	public:
		BluePrint( const XMLNode& node )
			: m_node( node )
		{
			m_name			  = GetStringXMLAttribute( node, "name", "UNNAMED" );			
			m_position		  = GetVector2XMLAttribute( node, "position", Vector2::ZeroVector );
			m_health		  = GetFloatXMLAttribute( node, "health", 0.0f );
			m_maxHealth		  = GetFloatXMLAttribute( node, "maxHealth", 1000.0f );
			m_canBeDamaged	  = GetBoolXMLAttribute( node, "canBeDamaged", false );	
			m_inventorySlot   = IInventory::GetInventorySlotFromString( GetStringXMLAttribute( node, "inventorySlot", "NO_SLOT" ));
			m_texturePath	  = GetStringXMLAttribute( node, "texture", "item.png" );
			m_scale			  = GetFloatXMLAttribute( node, "scale", 0.01f );
			m_effectInSeconds = GetIntXMLAttribute( node, "effectInSeconds", 0 );
			m_singleUse		  = GetBoolXMLAttribute( node, "singleUse", true );

			// Stats
			m_strength		  = GetFloatXMLAttribute( node, "strength", 1.0f );
			m_dexterity		  = GetIntXMLAttribute( node, "dexterity", 1 );
			m_toughness		  = GetFloatXMLAttribute( node, "toughness", 1.0f );
			m_dodge			  = GetIntXMLAttribute( node, "dodge", 1 );
		}

		Item * Create()
		{			
			return new Item( m_name, m_effectInSeconds, m_singleUse, m_texturePath.c_str(), m_scale, m_inventorySlot, m_maxHealth, m_health, 
				true, m_canBeDamaged, m_strength, m_dexterity, m_toughness, m_dodge, m_position );
		}

		virtual BluePrintBase* Clone() const
		{
			return new BluePrint< Item * >( m_node );
		}	
	};
}

#endif