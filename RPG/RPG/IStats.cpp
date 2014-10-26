#include "stdafx.h"
#include "IStats.h"

namespace Solidus
{
	
	IStats::IStats( float strength, int dexterity, float toughness, int dodge )
		: m_strength( strength )
		, m_dexterity( dexterity )
		, m_toughness( toughness )
		, m_dodge( dodge )
		// Constants - TODO: Later this values could increase with the level?
		, m_maxStrength( 500.0f )
		, m_maxDexterity( 100 )
		, m_maxToughness( 500.0f )
		, m_maxDodge( 100 )
	{}

	IStats::~IStats()
	{}
}