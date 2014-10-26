#pragma once

#ifndef _WORLD_H_
#define _WORLD_H_

#include "BitmapFontCharacter.h"
#include "Console.h"
#include "Matrix44.h"
#include "Map.h"
#include "Camera2D.h"
#include "Agent.h"

namespace Solidus
{
	class Clock;
	class World
	{
	public:
		// CTOR
		//
		explicit World( HWND hWnd, HDC hDC, double deltaTime, int width_, int height_ );
		~World();

		// FUNCTIONS
		//		
		void   OnResize( int width, int height );			
		void   Init();
		void   UpdateInput();
		void   UpdateSimulation();
		void   Draw();		

		// INLINES
		//
		double GetGameTime() const { return m_nUpdates * m_deltaTime; }


		void CreateColorJob( NamedProperties& args );
		
	private:		
		// FUNCTIONS
		//		
		void SetSize( int width_, int height_ );		

		// WIN32 API
		//
		HWND m_hWind;
		HDC m_hDC;

		// DATA
		//		
		unsigned int						m_nUpdates;
		double								m_deltaTime;
		int									m_width; 
		int									m_height;		
		
		// Game
		std::unique_ptr< Camera2D >		    m_camera;				

		bool taken;
		bool previouslyTaken;

		//
		Clock* m_clock1;
	};
}

#endif