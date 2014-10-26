#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Vector3.h"
#include "Matrix44.h"

namespace Solidus
{	
	class Camera
	{		
	public:
		// CTOR
		//
		Camera();

		// FUNCTIONS
		//
		void			CalculateDirection();
		void		    UpdateInput();
		void		    UpdateDynamics();
		void		    Update();
		const Matrix44& ProjectionMatrix() const			  { return m_projectionMatrix; }
		const Matrix44& ViewMatrix() const					  { return m_viewMatrix; }
		const Vector3&  ViewVector() const					  { return m_viewVector; }
		const Vector3&  Rotation()	const					  { return m_rotation; }
		const Vector3&  Position()	const					  { return m_position; }
		const void	    Position( float x, float y, float z ) { m_position.SetXYZ( x, y, z ); }
		
		// INLINES
		//
		inline size_t GetCurrentMode() const 
		{ 
			return m_currentMode; 
		}
		
		inline void SetCurrentMode( size_t cameraMode ) 
		{ 
			m_currentMode = cameraMode; 
		}

		// ENUMS
		//
		enum Modes { NORMAL, FLYING, NOCLIP, NUM_MODES };

		// STATICS
		//
		static const Vector3 ViewDirection;
	private:
		float    m_drag;		  // Camera drag force
		Vector3  m_direction;
		Vector3	 m_upVector;
		Vector3	 m_rightVector;
		Vector3  m_position;   // Camera position
		Vector3  m_velocity;   // Camera velocity
		Vector3  m_viewVector; // Camera view direction
		Vector3  m_rotation;	  // Pitch, yaw, roll. (Angles in degrees.)
		Matrix44 m_viewMatrix;
		float    m_MouseCenterX;
		float    m_MouseCenterY;
		size_t   m_currentMode;	
		Matrix44 m_projectionMatrix;
		float	 m_initialFoV;
		float	 m_aspectRatio;
	};
}
#endif