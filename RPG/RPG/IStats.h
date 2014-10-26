#pragma once

#ifndef _ISTATS_H_
#define _ISTATS_H_

namespace Solidus
{	
	class IStats
	{
	public:
		/* =============================================================
		 * CTOR
		 * ============================================================= */

		IStats( float strength, int dexterity, float toughness, int dodge );
		~IStats();

		/* =============================================================
		 * INLINE
		 * ============================================================= */

		inline float Strength() const  { return m_strength; }
		inline int	 Dexterity() const { return m_dexterity; }
		inline float Toughness() const { return m_toughness; }
		inline int	 Dodge() const	   { return m_dodge; }
	protected:
		float		m_strength;
		int			m_dexterity;
		float		m_toughness;
		int			m_dodge;

		// MAX Values
		float m_maxStrength;
		int	  m_maxDexterity;
		float m_maxToughness;
		int	  m_maxDodge;
	};
}

#endif