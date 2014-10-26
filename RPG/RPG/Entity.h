#pragma once

#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <string>
#include <vector>

#include "GameObject.h"
#include "IInventory.h"
#include "Sprite.h"
#include "Vector4.h"
#include "StringExtensions.h"

namespace Solidus
{		
	class Map;
	class Agent;
	class Entity : public GameObject, public Sprite, public IInventory
	{
		friend class MapManager;
		friend class Renderer2D;
	public:
		/* =============================================================
		 * CTOR
		 * ============================================================= */
		Entity( const Entity& cSource );

		Entity( std::string name, bool blocksPosition, std::string entityThatUnblocks, float maxHealth, float health, bool canBeEquipped, 
			bool canBeDamaged, int inventoryLimit, std::string destination = StringExtensions::EmptyString, 
			Vector2 warpTo = Vector2::ZeroVector,
			Vector2 initialPosition = Vector2::ZeroVector, 
			Vector4 color = Vector4::EmptyColor );

		Entity( std::string name, bool blocksPosition, std::string entityThatUnblocks, const char* texturePath, float textureScale, 
			float maxHealth, float health, bool canBeEquipped, bool canBeDamaged, int inventoryLimit, 
			std::string destination = StringExtensions::EmptyString, Vector2 warpTo = Vector2::ZeroVector, 
			Vector2 initialPosition = Vector2::ZeroVector, Vector4 color = Vector4::EmptyColor );

		~Entity( void );

		/* =============================================================
		 * BASE CLASS FUNCTIONS
		 * ============================================================= */

		// SPRITE
		virtual void Draw( const Renderer2D& painter );
		virtual void Init( void );
		virtual void LoadContent( void );
		virtual void UpdateSimulation( double deltaTime );
		virtual void UpdateInput( void );

		// IINVENTORY		
		bool	  AddItem( Entity* item );
		Entity*	  FindItem( Entity* item );
		Entity*	  RemoveItem( Entity* item );
		Entity*	  GetLastItem();
		Entity*	  GetPenultimateItem();
		Entity*	  RemoveLastItem();
		Agent*	  HasAgents();
		bool	  UseItem( const std::string& itemName );

		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */
		
		virtual void    ReceiveHealing( float health );
		virtual void    TakeDamage( float damage, bool forcedDamage = false );		
		void		    UnBlock();
		virtual Entity* Clone();		
		void			ValidateInitialLocation();

		/* =============================================================
		 * INLINE
		 * ============================================================= */
		inline const void		 SetColor( const Vector4& color ) { m_color = color; } // Find a way of not doing this
		inline void				 SetMap( std::shared_ptr< Map > newMap ) { m_map = newMap; }
		inline const Vector4	 Color() const { return m_color; }
		inline const bool		 CanBeEquipped() const { return m_canBeEquipped; }
		inline const bool		 BlocksPosition() const { return m_blocksPosition; }
		inline const bool		 CanBeUnlocked() const { return !m_entityThatUnblocks.empty(); }
		inline const std::string ItemThatUnlocks() const { return m_entityThatUnblocks; }
		inline const std::string Destination() const { return m_destination; }
		inline const Vector2	 WarpTo() const { return m_warpTo; }
		inline const bool		 CanWarp() const { return m_destination.length() > 0; }				
	protected:
		/* =============================================================
		 * MEMBERS
		 * ============================================================= */
		Vector4		  	      m_color;
		std::string			  m_texturePath;
		bool				  m_canBeEquipped;
		bool				  m_blocksPosition;
		std::string			  m_entityThatUnblocks;
		std::string			  m_destination;
		Vector2				  m_warpTo;
		double				  m_deltaTime;		
		double				  m_previousDeltaTime;
		std::weak_ptr< Map >  m_map;
		Entity*				  m_parent;
	};
}

#endif