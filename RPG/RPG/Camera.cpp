#include "stdafx.h"
#include "Camera.h"
#include "MathUtilities.h"
#include "InputManager.h"
#include "ReportErrors.h"

namespace Solidus
{
	const Vector3 Camera::ViewDirection( 0.0f, 0.0f, -1.0f );
	const float MOUSE_PITCH_POWER = 0.005f;
	const float MOUSE_YAW_POWER = 0.005f;
	const float KEYBOARD_IMPULSE_POWER = 0.05f;
	float horizontalAngle = 3.14f;	
	float verticalAngle = 0.0f;
	float speed = 3.0f;

	Camera::Camera() 
		: m_rotation   ( Vector3::ZeroVector ) 
		, m_position   ( Vector3::ZeroVector )
		, m_velocity   ( Vector3::ZeroVector )
		, m_viewVector ( Vector3::ZeroVector )
		, m_drag       ( 0.2f )
		, m_currentMode( 2 )
		, m_initialFoV( 45.0f )
	{}

	void Camera::UpdateInput()
	{
		CalculateDirection();

		// Move forward
		if ( Solidus::InputManager::Instance().IsKeyDown( VK_UP ) || Solidus::InputManager::Instance().IsKeyDown( 'W' ) )
		{
			m_position += m_direction * KEYBOARD_IMPULSE_POWER;
		}
		
		// Move backward
		if ( Solidus::InputManager::Instance().IsKeyDown( VK_DOWN ) || Solidus::InputManager::Instance().IsKeyDown( 'S' ) )
		{
			m_position -= m_direction * KEYBOARD_IMPULSE_POWER;
		}

		// Strafe right
		if( Solidus::InputManager::Instance().IsKeyDown( VK_RIGHT ) || Solidus::InputManager::Instance().IsKeyDown( 'D' ))
		{
			m_position += m_rightVector * KEYBOARD_IMPULSE_POWER;
		}

		// Strafe left
		if ( Solidus::InputManager::Instance().IsKeyDown( VK_LEFT ) || Solidus::InputManager::Instance().IsKeyDown( 'A' ) )
		{
			m_position -= m_rightVector * KEYBOARD_IMPULSE_POWER;
		}

		if( Solidus::InputManager::Instance().IsKeyDown( VK_SPACE ) )
		{
			m_position.y += 1.0f * KEYBOARD_IMPULSE_POWER;
		}

		if( Solidus::InputManager::Instance().IsKeyDown( VK_CONTROL ) || Solidus::InputManager::Instance().IsKeyDown( 'C' ))
		{
			m_position.y -= 1.0f * KEYBOARD_IMPULSE_POWER;
		}		
	}

	void Camera::UpdateDynamics()
	{
		// Reduce the camera velocity
		//
		m_velocity -= m_velocity * m_drag;

		// Update the position based on the velocity
		//
		m_position += m_velocity;		
	}

	void Camera::Update()
	{
		float FoV = m_initialFoV - 5 * Solidus::InputManager::Instance().GetMouseWheelDelta();
		//float FoV = m_initialFoV;

		m_projectionMatrix = Matrix44::MakePerspective( FoV, m_aspectRatio, 0.1f, 100000.0f );

		Vector3 looksAt = m_position + m_direction;

		// Camera matrix
		m_viewMatrix = Matrix44::MakeLookAt(
			m_position.x, m_position.y, m_position.z,           // Camera is here
			looksAt.x, looksAt.y, looksAt.z,					// and looks here : at the same position, plus "direction"
			m_upVector.x, m_upVector.y, m_upVector.z									// Head is up (set to 0,-1,0 to look upside-down)
			);
	}

	void Camera::CalculateDirection()
	{
		RECT screenRect;

		// Get window size
		//
		GetWindowRect( GetDesktopWindow(), &screenRect );
		m_aspectRatio = screenRect.right / static_cast< float >( screenRect.bottom );

		// Set the mouse center positions
		//
		m_MouseCenterX = static_cast< float >( screenRect.right  / 2 );
		m_MouseCenterY = static_cast< float >( screenRect.bottom / 2 );		

		Vector2 mousePos = Solidus::InputManager::Instance().GetMousePosition();

		// Setting Mouse Cursor (Lock)
		//
		SetCursorPos( static_cast< int >( m_MouseCenterX ), static_cast< int >( m_MouseCenterY ));

		// Compute new orientation
		horizontalAngle += MOUSE_YAW_POWER * float( m_MouseCenterX - mousePos.x );
		verticalAngle   += MOUSE_YAW_POWER * float( m_MouseCenterY - mousePos.y );

		// Clamp the x position to be between -90 and 90
		//
		//horizontalAngle = Clamp( 90.0f, -90.0f, horizontalAngle );

		// Direction : Spherical coordinates to Cartesian coordinates conversion
		m_direction = Vector3(
			cos(verticalAngle) * sin(horizontalAngle), 
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
			);

		// Orthogonal fix-up

		// Right vector
		m_rightVector = Vector3(
			sin( horizontalAngle - PI / 2.0f ), 
			0,
			cos( horizontalAngle - PI / 2.0f )
			);

		// Up vector		
		m_upVector = m_rightVector.CrossProduct( m_direction );
	}

}