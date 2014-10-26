#include "stdafx.h"
#include "FogParameters.h"

namespace Solidus
{	
	FogParameters::FogParameters()
		: Density( 0.50f )
		, Start  ( 1.0f )
		, End	 ( 5.0f )
		, Color  ( Vector4( 0.7f, 0.7f, 0.7f, 1.0f ))
		, Mode   ( FOG_EQUATION_EXP )
	{}
}

Solidus::FogParameters g_FogParameters;