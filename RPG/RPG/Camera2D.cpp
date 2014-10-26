#include "stdafx.h"
#include "Camera2D.h"

namespace Solidus
{
	Camera2D::Camera2D( int _width, int _height )
		: m_fov( static_cast< float >( _width ), static_cast< float >( _height ))		
	{}

	void Camera2D::SetZoomLevel( int _width, int _height )
	{
		m_fov = Vector2( static_cast< float >( _width ), static_cast< float >( _height ));
	}

	void Camera2D::SetTarget( Entity* _target )
	{
		if( _target != m_target)
		{
			m_target = _target;	
			if( _target )
				m_position;
		}		
	}

	void Camera2D::UpdateViewMatrix()
	{
		if( !m_target )
			return;

		Vector2 dif ;

		// Playing with camera behavior. Move depending of the screen position of the target
		if( abs(dif.x) > m_WindowTileSize.x / 4.0f )
			m_position.x -= dif.x - m_WindowTileSize.x / 4.0f;
		if( abs(dif.y) > m_WindowTileSize.y /4.0f )
			m_position.y -= dif.y -m_WindowTileSize.y / 4.0f;

		m_viewMatrix = Matrix44::CreateTranslationMatrix( -m_target->GetLocation() + m_WindowTileSize );

		glLoadMatrixf( m_viewMatrix.GetMatrix() );
	}

	void Camera2D::SetWindowTileSize( const Vector2& tileSize )
	{
		m_WindowTileSize.x = tileSize.x / 2.0f;
		m_WindowTileSize.y = tileSize.y / 2.0f;
	}

	void Camera2D::LockMouseCursor()
	{
		RECT screenRect;

		// Get window size
		//
		GetWindowRect( GetDesktopWindow(), &screenRect );		

		// Set the mouse center positions
		//
		float m_MouseCenterX = static_cast< float >( screenRect.right  / 2 );
		float m_MouseCenterY = static_cast< float >( screenRect.bottom / 2 );

		// Setting Mouse Cursor (Lock)
		//
		SetCursorPos( static_cast< int >( m_MouseCenterX ), static_cast< int >( m_MouseCenterY ));
	}
}