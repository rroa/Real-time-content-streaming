#pragma once

#ifndef _FOGPARAMETERS_H_
#define	_FOGPARAMETERS_H_

#include "Vector4.h"

namespace Solidus
{
	enum FogMode
	{
		FOG_EQUATION_LINEAR	= 0,
		FOG_EQUATION_EXP	= 1,
		FOG_EQUATION_EXP2	= 2
	};

	class FogParameters
	{	
	public:
		FogParameters();		

		float Density;
		float Start;
		float End;
		Vector4 Color;
		int Mode;		
	};
}

extern Solidus::FogParameters g_FogParameters;

#endif