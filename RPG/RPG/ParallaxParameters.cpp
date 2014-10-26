#include "stdafx.h"
#include "ParallaxParameters.h"

namespace Solidus
{
	ParallaxParameters::ParallaxParameters()
		: Scale( 0.17f )
		, Bias( -0.03f )
	{}
}

Solidus::ParallaxParameters g_ParallaxParameters;