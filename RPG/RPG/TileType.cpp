#include "stdafx.h"
#include "TileType.h"

namespace Solidus
{
	TileType::TileType()
	{}

	TileType::TileType( const std::string& name, const float& health )
		: IsSolid( false )
		, Color( 1.0 )
		, Name( name )
		, MaxHealth( health )
		, DamageSpeed( 0 )
		, DamageRatePerSecond( 0.0f )
		, DamageBehavior( NONE )
		, DoesNotWorkAgainst( StringExtensions::EmptyString )
	{}	
}