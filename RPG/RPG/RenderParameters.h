#pragma once

#ifndef _RENDER_PARAMS_H_
#define	_RENDER_PARAMS_H_

#include <string>

#include "Vector3.h"

namespace Solidus
{
	class RenderParameters
	{
	public:
		RenderParameters();

		// Properties
		
		// FBO
		//
		bool  FrameBufferPipelineEnabled;
		int	  FrameBufferEffectIndex;

		// Lights
		//
		bool	LightsDraw;
		bool	LightsDrawDirection;
		bool	LightsMovePoint;
		bool	LightsMoveSpot;
		bool	LightsMoveDirectional;		
		bool	LightsMoveNone;
		int		LightsOn;
		Vector3 LightsRotationAxis;
		float   LightsRotationAngle;

		//
		double DeltaTime;

		// General
		//
		bool UseFog;
		bool UseParallax;
		bool UseCustomEffect;
		std::string CurrentShader;

		// Model
		//
		bool  RotateModel;
		float RotationAngle;
		float RotationAngleRatio;
	};
}

extern Solidus::RenderParameters g_RenderParameters;

#endif