#include "stdafx.h"
#include "PlayerColor.h"
#include "StringExtensions.h"

namespace Solidus
{
	PlayerColor::PlayerColor()
		: Color( 1.0f, 1.0f, 1.0f, 1.0f )
		, TexturePath( StringExtensions::EmptyString )
	{}	
}

Solidus::PlayerColor g_PlayerColor;