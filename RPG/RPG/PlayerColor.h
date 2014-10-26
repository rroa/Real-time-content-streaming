#pragma once

#ifndef _PLAYERCOLOR_H_
#define	_PLAYERCOLOR_H_

#include "Vector4.h"
#include <string>

namespace Solidus
{
	class PlayerColor
	{	
	public:
		PlayerColor();		
		Vector4 Color;
		std::string TexturePath;
	};
}

extern Solidus::PlayerColor g_PlayerColor;

#endif