#pragma once

#ifndef _IINVENTORY_H_
#define _IINVENTORY_H_

#include <map>
#include "ReportErrors.h"
#include "StringExtensions.h"

namespace Solidus
{
	enum InventorySlots { NO_SLOT, HEAD, CHEST, ARMS, LEGS, POTION_FIRE, POTION_WATER, CONSUMABLE, NUM_INVENTORY_SLOTS };

	class Entity;
	class Item;
	class IInventory
	{
	public:
		/* =============================================================
		 * CTOR
		 * ============================================================= */

		IInventory()
			: m_lastItemAddedToInventoryId( 0 )
			, m_inventoryItemsLimit( -1 )
			, m_previousItemAddedToInventoryId( 0 )
		{
			m_equippedInventory.resize( NUM_INVENTORY_SLOTS );
		}

		/* =============================================================
		 * PURE VIRTUALS
		 * ============================================================= */

		virtual bool	  AddItem( Entity* item ) = 0;
		virtual Entity*	  FindItem( Entity* item ) = 0;
		virtual Entity*	  RemoveItem( Entity* item ) = 0;
		virtual Entity*	  RemoveLastItem() = 0;		
		virtual bool	  UseItem( const std::string& itemName ) = 0;

		/* =============================================================
	 * ENUM TRANSFORMATIONS
	 * ============================================================= */

	static const InventorySlots GetInventorySlotFromString( const std::string& inventorySlot )
	{				
		if( StringExtensions::CompareStrings( inventorySlot, "head" ))
		{
			return HEAD;
		}
		else if( StringExtensions::CompareStrings( inventorySlot, "chest" ))
		{
			return CHEST;
		}
		else if( StringExtensions::CompareStrings( inventorySlot, "arms" ))
		{
			return ARMS;
		}
		else if( StringExtensions::CompareStrings( inventorySlot, "legs" ))
		{
			return LEGS;
		}
		else if( StringExtensions::CompareStrings( inventorySlot, "potion_fire" ))
		{
			return POTION_FIRE;
		}
		else if( StringExtensions::CompareStrings( inventorySlot, "potion_water" ))
		{
			return POTION_WATER;
		}

		return NO_SLOT;
	}

	static const int GetValueFromInventorySlot( const InventorySlots& inventorySlot )
	{
		switch( inventorySlot )
		{
		case HEAD:
			return 0;
			break;
		case CHEST:
			return 1;
			break;
		case ARMS:
			return 2;
			break;
		case LEGS:
			return 3;
			break;
		case POTION_FIRE:
			return 4;
			break;
		case POTION_WATER:
			return 5;
			break;
		default:
			return -1;
		}			
	}

	static const std::string GetStringFromInventorySlot( const InventorySlots& inventorySlot )
	{
		switch( inventorySlot )
		{
		case HEAD:
			return "HEAD";
			break;
		case CHEST:
			return "CHEST";
			break;
		case ARMS:
			return "ARMS";
			break;
		case LEGS:
			return "LEGS";
			break;
		case POTION_FIRE:
			return "POTION_FIRE";
			break;
		case POTION_WATER:
			return "POTION_WATER";
			break;
		default:
			return "NO_SLOT";
		}			
	}

		/* =============================================================
		 * INLINE
		 * ============================================================= */

		inline void		  SetInventoryLimit( int inventoryLimit ) { m_inventoryItemsLimit = inventoryLimit; }
		inline const bool HasItems() const						  { return m_items.size() > 0; }

		inline const bool IsInventoryFull() const 
		{ 
			if( m_inventoryItemsLimit != -1 )
			{
				if( static_cast< int >( m_items.size() ) == m_inventoryItemsLimit ) return true;
			}
			return false;
		}

		inline const bool IsSlotInUse( const InventorySlots& inventorySlot )
		{
			if( inventorySlot != NO_SLOT && inventorySlot != NUM_INVENTORY_SLOTS )
			{
				int slotIndex = GetValueFromInventorySlot( inventorySlot );
				return !( m_equippedInventory[ slotIndex ] == nullptr );
			}
			return false;
		}

		inline const bool EquipSlot( InventorySlots inventorySlot, Entity* item )
		{
			if( inventorySlot != NO_SLOT && inventorySlot != NUM_INVENTORY_SLOTS )
			{
				int slotIndex = GetValueFromInventorySlot( inventorySlot );
				if( m_equippedInventory[ slotIndex ] == nullptr )
				{
					m_equippedInventory[ slotIndex ] = item;
					return true;
				}				
			}
			return false;
		}

		inline const bool ReleaseSlot( InventorySlots inventorySlot )
		{
			if( inventorySlot != NO_SLOT && inventorySlot != NUM_INVENTORY_SLOTS )
			{
				int slotIndex = GetValueFromInventorySlot( inventorySlot );
				m_equippedInventory[ slotIndex ] = nullptr;
				DEBUG("Your " << GetStringFromInventorySlot( inventorySlot ) << " Slot was successfully released.");
				return true;
			}
			DEBUG("Your " << GetStringFromInventorySlot( inventorySlot ) << " Slot was empty.");
			return false;
		}

	protected:
		int						  m_lastItemAddedToInventoryId;		
		int						  m_previousItemAddedToInventoryId;		
		int						  m_inventoryItemsLimit;
		std::map< int, Entity * > m_items;
		std::vector< Entity * >   m_equippedInventory;
	};
}

#endif