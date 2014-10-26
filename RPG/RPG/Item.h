#pragma once

#ifndef _ITEM_H_
#define _ITEM_H_

#include <string>

#include "GameObject.h"
#include "Map.h"
#include "Vector2.h"
#include "IStats.h"
#include "MapManager.h"

namespace Solidus
{	
	class Item : public Entity, public IStats
	{
		friend class MapManager;
	public:
		/* =============================================================
		 * CTOR
		 * ============================================================= */
		Item( const Item& cSource );

		Item( std::string name, 
			  int effectInSeconds, 
			  bool singleUse, 
			  const char* texturePath, 
			  float textureScale, 
			  InventorySlots inventorySlot, 
			  float maxHealth, 
			  float health, 
			  bool canBeEquiped, 
			  bool canBeDamaged, 
			  // Stats
			  float strength, 
			  int dexterity, 
			  float toughness, 
			  int dodge, 
			  Vector2 initialPosition = Vector2::ZeroVector );
		~Item();

		/* =============================================================
		 * BASE CLASS FUNCTIONS
		 * ============================================================= */
	
		void  Draw( const Renderer2D& painter );
		void  Init( void );		
		void  UpdateSimulation( double deltaTime );
		void  UpdateInput( void );
		Item* Clone();

		/* =============================================================
		 * INLINE
		 * ============================================================= */
		inline const bool			IsEquipped() const	      { return m_equipped; }
		inline const bool			IsDeadItem() const	      { return m_effectInSeconds > 0 && m_remainingSeconds == 0 && !m_equipped; }
		inline void					Equip()				      { m_equipped = true; }
		inline void					UnEquip()			      { if( !m_singleUse ) m_equipped = false; }
		inline const InventorySlots InventorySlot() const     { return m_inventorySlot; }
	private:

		/* =============================================================
		 * INLINE
		 * ============================================================= */
		
		void CheckItemLifetime();

		InventorySlots m_inventorySlot;
		int			   m_effectInSeconds;
		bool		   m_equipped;
		bool		   m_singleUse;
		int			   m_remainingSeconds;
	};
}

#endif