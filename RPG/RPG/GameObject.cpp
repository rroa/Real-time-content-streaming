#include "stdafx.h"
#include "GameObject.h"

namespace Solidus
{
	int GameObject::s_nextID = 0;

	GameObject::GameObject( std::string name, float maxHealth, bool canBeDamaged, float currentHealth )
		: m_id( ++s_nextID )
		, m_name( name )		
		, m_health( maxHealth )
		, m_maxHealth( maxHealth )
		, m_canBeDamanged( canBeDamaged )
		, m_isDirty( false )
	{
		if( currentHealth > 0 )
		{
			m_health = currentHealth;
		}
	}

	GameObject::~GameObject()
	{}
}