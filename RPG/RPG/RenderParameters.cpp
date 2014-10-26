#include "stdafx.h"
#include "RenderParameters.h"

#include "ResourceManager.h"

namespace Solidus
{
	RenderParameters::RenderParameters()
		// FBO
		: FrameBufferPipelineEnabled( false )
		, FrameBufferEffectIndex( 0 )
		, DeltaTime( 0.0 )
		// Lights
		, LightsDraw( true )
		,LightsDrawDirection( true )
		,LightsMovePoint( true )
		,LightsMoveSpot( true )
		,LightsMoveDirectional( true )	
		,LightsMoveNone( true )
		,LightsOn( 1 )
		,LightsRotationAxis( 0.0f, 0.0f, 0.0f )
		,LightsRotationAngle( 0.0f )
		// General
		, UseFog( false )
		, UseParallax( false )
		, UseCustomEffect( false )
		, CurrentShader( "BasicShader" )
		//Model
		, RotateModel( false )
		, RotationAngle( 0.0f )
		, RotationAngleRatio( 0.1f )
	{}
}

Solidus::RenderParameters g_RenderParameters;