#pragma once

#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <string>

namespace Solidus
{			
	class GameObject
	{
		friend class Renderer2D;
	public:
		/* =============================================================
		 * CTOR
		 * ============================================================= */

		GameObject( std::string name, float maxHealth, bool canBeDamaged, float currentHealth = 0 );
		~GameObject();

		/* =============================================================
		 * INLINE
		 * ============================================================= */

		inline const int		  Id() const		   { return m_id; }
		inline const std::string Name() const		   { return m_name; }
		inline const float		  Health() const	   { return m_health; }
		inline const float		  MaxHealth() const	   { return m_maxHealth; }
		inline const bool		  CanBeDamaged() const { return m_canBeDamanged; }
		inline bool				  IsDirty() const	   { return m_isDirty; }
		inline void				  SetDirty()		   { m_isDirty = true; }
	protected:
		/* =============================================================
		 * MEMBERS
		 * ============================================================= */

		int			m_id;
		float		m_health;
		float		m_maxHealth;
		std::string m_name;
		bool		m_canBeDamanged;
		bool		m_isDirty;

		/* =============================================================
		 * STATIC
		 * ============================================================= */

		static int	s_nextID;		
	};
}

#endif