#pragma once

#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#define KEYCOUNT 256
#define _KEY_INVALID -2

#include <algorithm>
#include "Vector2.h"

namespace Solidus
{
	class InputManager
	{
	public:
		// Constant Fields
		static const int MouseDelta; 

		// SINGLETON FUNCTIONS
		//
		static void Create();
		static void Destroy();	
		static InputManager& Instance();

		// FUNCTIONS
		//
		bool		   IsKeyDown	   ( unsigned char key );
		bool		   IsKeyPressed    ( unsigned char key );
		bool		   IsKeyReleased   ( unsigned char key );
		void		   KeyDown		   ( unsigned char key );
		void		   KeyUp		   ( unsigned char key );		
		void		   SetMousePosition( int x, int y );
		void		   ResetInput();
		Vector2		   GetMousePosition();
		int			   GetMouseWheelDelta() const;

		// INLINE
		//
		inline bool	IsAnyKeyPressed () const { return m_isKeyPressed; }		
		inline void	SetKeyPressed( bool isKeyPressed ) { m_isKeyPressed = isKeyPressed; }
		inline void	SetMouseWheelDelta( int delta ) { m_wheel_delta += delta; }
		inline bool	IsUsingMouse() const { return m_UsingMouse; }
		inline bool	SetIsUsingMouse( bool UsingMouse ) { m_UsingMouse = UsingMouse; }
		inline bool IsMouseEnabled() const { return m_isMouseEnabled; }
		inline void ShowMouseCursor( const bool& showCursor ) const { ShowCursor( showCursor ); }
		void PauseMouse();
		void Update();
	private:
		// Setting the entity as non-copyable
		//
		InputManager();
		InputManager( InputManager const& ) {};
		InputManager& operator=( InputManager const& ) {};		
		// DATA
		//		
		bool					m_currentKeys[ KEYCOUNT ];
		bool					m_previousKeys[ KEYCOUNT ];
		bool					m_isMouseEnabled;
		bool					m_isKeyPressed;		
		Vector2					m_mousePosition;
		bool					m_UsingMouse;
		bool					m_FistMouseSet;
		bool					m_showMouseCursor;
		int						m_wheel_delta;		
		static InputManager*	s_instance;
	};
}

extern void onKeyboardDownEvent  ( unsigned char key );
extern void onKeyboardReleasedEvent	 ( unsigned char key );
extern void onMouseMovementEvent ( int x, int y );
extern void onMouseWheelEvent ( int delta );
extern void keyboardPollEvent();

#endif