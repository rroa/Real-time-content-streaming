#pragma once

#ifndef _LIGHT_H_
#define	_LIGHT_H_

#include "Vector4.h"
#include "Vector3.h"

namespace Solidus
{
	typedef Vector3 Position, Direction, Color;
	enum LightType { POINT, DIRECTIONAL, SPOT, NUM_TYPES };

	struct Attenuation
	{
		float Constant, Linear, Exponential;
	};

	class Light
	{
	public:
		// CTOR
		//
		Light();
		Light( Position position_, Color color_ );
		Light( Position position_, Direction direction_, Color color_, LightType type_ );
		Light( Position position_, Direction direction_, Color color_, Color specularColor_, Attenuation attenuation_, LightType type_ );

		// FUNCTIONS
		//
		void					TurnOn();              // Turns on light
		void					TurnOff();             // Turns off light
		void					Toggle();              // Toggles light on or off		
		void					Rotate( float angle, Vector3& rotationVector, bool usingRadians = false );

		// SETTERS
		void					SetColor( float r, float g, float b );
		void					SetColor( Color color_ );
		void					SetPosition( float x, float y, float z );
		void					SetDirection( float x, float y, float z );

		// INLINE
		//
		inline LightType	    GetLightType()  const				  { return static_cast< LightType >( m_lightType ); }
		inline bool			    GetOnOffState() const				  { return m_on_off_state; }		
		inline const Position&  GetPosition() const					  { return m_currentPosition; }
		inline const Direction& GetDirection() const				  { return m_direction; }
		inline const Color&     GetDiffuseColor() const				  { return m_diffuseColor; }
		inline const Color&     GetSpecularColor() const			  { return m_specularColor; }
		inline const Color&     GetAmbientColor() const				  { return m_ambientColor; }
		inline const float&     GetConstantAttenuation() const		  { return m_constantAttenuation; }
		inline const float&     GetLinearAttenuation() const		  { return m_linearAttenuation; }
		inline const float&     GetExponentialAttenuation() const	  { return m_exponentialAttenuation; }
		inline const float&     GetSpotCutOff() const				  { return m_spotCutOff; }
		inline const float&     GetSpotExponent() const				  { return m_spotExponent; }
		inline const float&     GetPower() const					  { return m_power; }
		inline const float&     GetAmbientColorIntensity() const	  { return m_ambientColorIntensity; }
	//private:
		// Position
		Direction				m_direction;
		Position				m_currentPosition;
		Position				m_initialPosition;
		
		// State
		bool					m_on_off_state;
		int						m_lightType;
		
		// Colors
		Color					m_diffuseColor;
		Color					m_specularColor;
		Color					m_ambientColor;
		
		// CutOff
		float					m_spotCutOff;
		float					m_spotExponent;

		// Attenuation
		float					m_constantAttenuation;
		float					m_linearAttenuation;
		float					m_exponentialAttenuation;

		// 
		float					m_power;
		float					m_ambientColorIntensity;
	};
}

#endif