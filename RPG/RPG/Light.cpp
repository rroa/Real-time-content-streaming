#include "stdafx.h"
#include "Light.h"
#include "Matrix44.h"

namespace Solidus
{
	Light::Light()
		: m_initialPosition       ( Vector3( 0.0f ))
		, m_currentPosition       ( Vector3( 0.0f ))
		, m_direction	          ( Vector3( -1.0f ))
		, m_diffuseColor          ( Vector3( 1.0f ))
		, m_specularColor         ( Vector3( 1.0f ))
		, m_constantAttenuation   ( 1.0f )
		, m_linearAttenuation     ( 1.0f )
		, m_exponentialAttenuation( 1.0f )
		, m_spotCutOff			  ( 1.0f )
		, m_spotExponent		  ( 0.0f )
		, m_on_off_state		  ( true )
		, m_lightType			  ( 0 )
	{}

	Light::Light( Position position_, Color color_ )
		: m_initialPosition       ( position_ )
		, m_currentPosition       ( position_ )
		, m_direction	          ( Vector3( -1.0f ))
		, m_diffuseColor          ( color_ )
		, m_specularColor         ( Vector3( 1.0f ))
		, m_constantAttenuation   ( 1.0f )
		, m_linearAttenuation     ( 1.0f )
		, m_exponentialAttenuation( 1.0f )
		, m_spotCutOff			  ( 1.0f )
		, m_spotExponent		  ( 0.0f )
		, m_on_off_state		  ( true )
		, m_lightType			  ( 0 )		
	{}	

	Light::Light( Position position_, Direction direction_, Color color_, LightType type_ )
		: m_initialPosition       ( position_)
		, m_currentPosition       ( position_)
		, m_direction	          ( direction_)
		, m_diffuseColor          ( color_ )
		, m_specularColor         ( Vector3( 1.0f ))
		, m_constantAttenuation   ( 1.0f )
		, m_linearAttenuation     ( 1.0f )
		, m_exponentialAttenuation( 1.0f )
		, m_spotCutOff			  ( 1.0f )
		, m_spotExponent		  ( 0.0f )
		, m_on_off_state		  ( true )
		, m_lightType			  ( static_cast< int >( type_ ))
	{}

	Light::Light( Position position_, Direction direction_, Color color_, Color specularColor_, Attenuation attenuation_, LightType type_ )
		: m_initialPosition       ( position_)
		, m_currentPosition       ( position_)
		, m_direction	          ( direction_)
		, m_diffuseColor          ( color_ )
		, m_specularColor         ( specularColor_)
		, m_constantAttenuation   ( attenuation_.Constant )
		, m_linearAttenuation     ( attenuation_.Linear )
		, m_exponentialAttenuation( attenuation_.Exponential )
		, m_spotCutOff			  ( 1.0f )
		, m_spotExponent		  ( 0.0f )
		, m_on_off_state		  ( true )
		, m_lightType			  ( static_cast< int >( type_ ))
	{}

	void Light::TurnOn()
	{
		m_on_off_state = true;
	}

	void Light::TurnOff()
	{
		m_on_off_state = false;
	}

	void Light::Toggle()
	{
		m_on_off_state = !m_on_off_state;
	}

	void Light::Rotate( float angle, Vector3& rotationVector, bool usingRadians )
	{
		Matrix44 rot = Matrix44::Rotate( Matrix44::MakeIdentity(), angle, rotationVector, usingRadians );	
		m_currentPosition = rot.TransformPosition3D( m_initialPosition );
	}

	void Light::SetColor( float r, float g, float b )
	{
		m_diffuseColor = Vector3( r, g, b );
	}

	void Light::SetColor( Color color_ )
	{
		m_diffuseColor = Vector3( color_ );
	}

	void Light::SetPosition( float x, float y, float z )
	{
		m_currentPosition = Vector3( x, y, z );
	}

	void Light::SetDirection( float x, float y, float z )
	{
		m_direction = Vector3( x, y, z );
		m_direction.Normalize();
		m_direction = -m_direction;
	}

}