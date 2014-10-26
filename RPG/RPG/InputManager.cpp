#include "stdafx.h"
#include "InputManager.h"
#include <iostream>

namespace Solidus
{
	const int InputManager::MouseDelta = 10;
	InputManager* InputManager::s_instance = nullptr;

	InputManager::InputManager()
	{		
		std::generate( m_currentKeys, m_currentKeys + 256, [] () { return false; }  );
		ResetInput(); 

		m_isKeyPressed	  = false; 
		m_UsingMouse	  = false;
		m_FistMouseSet	  = false;
		m_isMouseEnabled  = true;
		m_showMouseCursor = false;
		m_wheel_delta	  = 0;

		ShowCursor( m_showMouseCursor );
	}
	

	void InputManager::Update()
	{
		POINT mousePos;
		GetCursorPos( &mousePos ); // Gets mouse position

		if( m_isMouseEnabled ) SetMousePosition( mousePos.x, mousePos.y );

		// Copy current keys to previous keys.
		//
		std::copy( m_currentKeys, m_currentKeys + KEYCOUNT, m_previousKeys );
	}

	void InputManager::Create()
	{
		if( !s_instance )
		{
			s_instance = new InputManager;
		}
	}

	void InputManager::Destroy()
	{
		if( s_instance )
		{					
			// Resetting the instance
			delete s_instance;
			s_instance = nullptr;
		}		
	}

	InputManager& InputManager::Instance()
	{
		if( !s_instance )
		{
			Create();
		}

		return *s_instance;
	}

	bool InputManager::IsKeyDown( unsigned char key )
	{	
		return m_currentKeys[key];
	}

	bool InputManager::IsKeyReleased( unsigned char key )
	{
		return ( m_currentKeys[key] == false && m_previousKeys[key] == true );
	}

	void InputManager::KeyDown( unsigned char key )
	{
		m_UsingMouse		 = false;		
		m_currentKeys[ key ] = true;
	}

	bool InputManager::IsKeyPressed( unsigned char key )
	{
		//return ( m_currentKeys[ key ] == true && m_previousKeys[ key ] == false );
		return m_currentKeys[key];
	}

	void InputManager::KeyUp( unsigned char key )
	{		
		m_currentKeys[ key ] = false;
	}

	void InputManager::ResetInput()
	{
		std::generate( m_previousKeys, m_previousKeys + 256, [] () { return false; } );
	}

	void InputManager::SetMousePosition( int x, int y )
	{
		float _x = static_cast< float >( x );
		float _y = static_cast< float >( y );

		Vector2 temp( _x , _y );
		temp = temp - m_mousePosition;
		temp.Abs();
		int dif = static_cast< int >( temp.AddFields() );

		if(dif > MouseDelta)
		{
			m_UsingMouse = true && m_FistMouseSet;
		}
		m_mousePosition.x = _x;
		m_mousePosition.y = _y;

		m_FistMouseSet = true;
	}

	Vector2 InputManager::GetMousePosition()
	{
		return m_mousePosition;
	}

	void InputManager::PauseMouse()
	{
		 m_isMouseEnabled = !m_isMouseEnabled;
	}

	int InputManager::GetMouseWheelDelta() const
	{
		return m_wheel_delta;
	}
}

void onKeyboardDownEvent( unsigned char key )
{	
	Solidus::InputManager::Instance().SetKeyPressed( true );
	Solidus::InputManager::Instance().KeyDown( key );
}

void onKeyboardReleasedEvent( unsigned char key )
{
	Solidus::InputManager::Instance().SetKeyPressed( false );
	Solidus::InputManager::Instance().KeyUp( key );	
}

void onMouseMovementEvent( int x, int y )
{
	if( Solidus::InputManager::Instance().IsMouseEnabled() )
	{
		Solidus::InputManager::Instance().SetMousePosition( x, y );
	}
}

 void onMouseWheelEvent ( int delta )
{	
	if( Solidus::InputManager::Instance().IsMouseEnabled() )
	{
		Solidus::InputManager::Instance().SetMouseWheelDelta( delta );
	}
}

void keyboardPollEvent()
{
	if( Solidus::InputManager::Instance().IsMouseEnabled() )
	{
		Solidus::InputManager::Instance().Update();
	}
}

