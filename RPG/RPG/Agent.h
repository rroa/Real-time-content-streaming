#pragma once

#ifndef _AGENT_H_
#define _AGENT_H_

#include <string>

#include "Entity.h"
#include "IStats.h"
#include "AI.h"
#include "MapManager.h"

namespace Solidus
{
	enum AgentOrientation { NORTH, SOUTH, EAST, WEST, NUM_AGENT_ORIENTATION };
	
	class Agent : public Entity, public IStats, public AI
	{
		friend class MapManager;
	public:
		/* =============================================================
		 * CTOR
		 * ============================================================= */
		Agent( const Agent& cSource );

				// Default
		Agent(	std::string name,
				const char* texturePath, 
				float textureScale,
				bool updateAlways,
				bool drawAlways,
				int inventoryLimit, 
				float maxHeatlh, 
				float health, 
				bool canBeDamaged,
				bool canBeControlled, 
				bool canMove, 
				float movementSpeed,
				BehaviorType behavior,
				float conditionalHealth,
				bool  conditionalMovement,
				// Stats
				float strength, 
				int dexterity, 
				float toughness, 
				int dodge,
				// Sprite properties
				Vector2 initialPosition = Vector2::ZeroVector, 
				Vector4 color = Vector4::EmptyColor );		

		/* =============================================================
		 * BASE CLASS FUNCTIONS
		 * ============================================================= */
		
		void						TakeDamage( float damage,  bool forcedDamage = false );
		void						UpdateInput( void );
		void						UpdateSimulation( double deltaTime );
		void						FindTarget( NamedProperties& eventArgs );
		void						CalculateDirectionFromHero( NamedProperties& eventArgs );
		Agent*						Clone();

		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */
		
		// Movement
		bool						MoveNorth();
		bool						MoveEast();
		bool						MoveWest();
		bool						MoveSouth();
		
		// Attack
		float						Attack();
		void						RangedAttack( int x, int y );
		
		// Inventory		
		void						DropItemFromInventory();
		bool						CanLoadSlot( const InventorySlots& inventorySlot );
		bool						ClearSlotFromItem( const InventorySlots& inventorySlot );
		bool						LoadSlotWithItem( const InventorySlots& inventorySlot );
		const bool					IsItemEquipped( const std::string& itemName ) const;

		// Field Of View
		void						CheckUpdateRadius( int x, int y );
		int							PointVisible( int x, int y );
		void						PickedTorch();

		/* =============================================================
		 * INLINE
		 * ============================================================= */

		inline const bool			RangeMode() const { return m_rangeMode; }
		inline const bool			CanMove() const { return m_canMove; }
		inline const BehaviorType	Behavior() const { return m_currentBehavior; }
		inline const bool			DrawAlways() const { return m_drawAlways; }
		inline const bool			UpdateAlways() const { return m_updateAlways; }
		inline const bool			HasTorch() const { return m_hasTorch; }
		inline const bool			IsHero() const {  return StringExtensions::CompareStrings( m_name, "hero" ); }
		inline const void			IgnoreInput( bool ignore ) { m_canBeControlled = !ignore; }
	private:
		/* =============================================================
		 * PRIVATE FUNCTIONS
		 * ============================================================= */

		void						CalculateStats();
		void						CheckIfHeroMovedOnTheMap();
		void						UpdateAgentPosition( int newXCoordinate, int newYCoordinate, Map* currentMap );
		void						EscapeToTarget( int x, int y, const Vector2& heroLocation = Vector2::ZeroVector ); 

		/* =============================================================
		 * MEMBERS
		 * ============================================================= */
		// Default
		AgentOrientation			m_currentDirection;
		bool						m_updateAlways;
		bool						m_drawAlways;
		bool						m_canBeControlled;
		bool						m_canMove;		
		unsigned int				m_currentLocationIndex;		
		double						m_torchPreviousDeltaTime;		
		float						m_movementSpeed;
		float						m_conditionalHealth;
		float						m_conditionalMovement;
		BehaviorType				m_currentBehavior;
		bool						m_hasTorch;
		double						m_torchDeltaTime;		

		int							m_viewRadius;
		float						m_fieldOfView;

		// Attack
		int							m_rangeLength;
		bool						m_rangeMode;

		// Stats
		float						m_totalStrengthFromBag;
		int							m_totalDexterityFromBag;
		float						m_totalToughnessFromBag;
		int							m_totalDodgeFromBag;
	};
}

#endif