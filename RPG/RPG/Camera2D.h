#pragma once

#ifndef _CAMERA2D_H_
#define _CAMERA2D_H_

#include "Vector2.h"
#include "Matrix44.h"
#include "Entity.h"

namespace Solidus
{
	class Camera2D
	{
	public:
		// CTOR
		//
		Camera2D( int _width, int _height );

		// FUNCTIONS
		//
		void LockMouseCursor();
		void SetTarget( Entity* _target );
		void SetZoomLevel( int _width, int _height );
		void UpdateViewMatrix();
		void SetWindowTileSize( const Vector2& tileSize);

		// INLINE
		inline const Matrix44& ViewMatrix() const { return m_viewMatrix; }
	private:
		Entity*  m_target;   // Camera position		
		Vector2  m_fov;
		Vector2	 m_WindowTileSize;
		Vector2  m_position;
		Matrix44 m_viewMatrix;
	};
}

#endif