#pragma once

#ifndef _PARALLAXPARAMETERS_H_
#define _PARALLAXPARAMETERS_H_

namespace Solidus
{
	class ParallaxParameters
	{
	public:
		// CTOR
		//
		ParallaxParameters();

		float Scale;
		float Bias;
	};
}

extern Solidus::ParallaxParameters g_ParallaxParameters;

#endif