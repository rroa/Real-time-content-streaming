#include "stdafx.h"
#include "World.h"

#include <time.h>

#include "BluePrint.h"
#include "BluePrintManager.h"
#include "EventSystem.h"
#include "InputManager.h"
#include "MapManager.h"
#include "ReportErrors.h"
#include "Settings.h"
#include "TextureManager.h"
#include "TextManager.h"
#include "Utilities.h"
#include "Vector2.h"
#include "StateManager.h"
#include "TimeManager.h"
#include "NamedProperties.h"
#include "JobManager.h"

namespace Solidus
{	
	World::World( HWND hWnd, HDC hDC, double deltaTime, int width_, int height_ )
		: m_hWind( hWnd )
		, m_hDC( hDC )
		, m_nUpdates( 0 )
		, m_deltaTime( deltaTime )		
		, m_width( width_ )
		, m_height( height_ )		
		, taken( false )	
		, previouslyTaken( false )
	{		
		m_camera.reset( new Camera2D( width_, height_ ));
		
		// Loading maps		
		Solidus::MapManager::Instance().Create();
		Solidus::MapManager::Instance().SetMap( "WasteLands" );

		// Set Windows Size
		SetSize( width_, height_ );		

		Init();
	}

	World::~World()
	{
		//TODO: Symphony of destruction
		Solidus::InputManager::Destroy();
		Solidus::TextureManager::Destroy();
		Solidus::ConsoleSystem::Destroy();
	}

	void World::OnResize( int width, int height )
	{
		SetSize( width, height );
	}

	void World::Draw()
	{		
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Drawing
		//
		glClearColor( 0.1f, 0.1f, 0.15f, 1.0f );

		Solidus::MapManager::Instance().CurrentMap()->Draw( Solidus::Renderer2D::Instance() );
		
		// Console Area
		//
		if( Solidus::ConsoleSystem::Instance().IsVisibile() ) 
		{			
			Solidus::ConsoleSystem::Instance().Draw();
		}		
		SwapBuffers( m_hDC );
	}

	void World::UpdateInput()
	{				
		// Input
		//
		HWND focusedWin = GetFocus();
		if ( m_hWind == focusedWin )
		{						

			if( InputManager::Instance().IsKeyReleased( 'b' ) || InputManager::Instance().IsKeyReleased( 'B' ))
			{
				Solidus::MapManager::Instance().CurrentMap()->GetHero()->UpdateTexture( "avatar.png" );
			}

			if( InputManager::Instance().IsKeyReleased( 't' ) || InputManager::Instance().IsKeyReleased( 'T' ))
			{
				//Solidus::MapManager::Instance().CurrentMap()->GetHero()->UpdateTexture();
				//Solidus::JobManager::Instance().CreateJob();
				//Solidus::JobManager::Instance().SpawnJobs( 10 );
				//Solidus::MapManager::Instance().Destroy();
				Solidus::MapManager::Instance().Create( true );
				Solidus::MapManager::Instance().SetMap( "WasteLands" );
			}

			if( InputManager::Instance().IsKeyReleased( 'p' ) || InputManager::Instance().IsKeyReleased( 'P' ) ) 
			{
				StateManager::Instance().Pause();
			}

			if( InputManager::Instance().IsKeyReleased( 'r' ) || InputManager::Instance().IsKeyReleased( 'R' ) ) 
			{
				StateManager::Instance().MovePrev();
			}

			if( InputManager::Instance().IsKeyReleased( 'g' ) || InputManager::Instance().IsKeyReleased( 'G' ) ) 
			{
				StateManager::Instance().MoveNext();
			}

			if( InputManager::Instance().IsKeyReleased( 'u' ) || InputManager::Instance().IsKeyReleased( 'U' ) ) 
			{
				StateManager::Instance().StartRecording();
			}

			if( InputManager::Instance().IsKeyReleased( 'y' ) || InputManager::Instance().IsKeyReleased( 'Y' ) ) 
			{
				StateManager::Instance().StopRecording();
			}

			if( Solidus::StateManager::Instance().IsPaused() ) return;

			m_camera->LockMouseCursor();

			Solidus::MapManager::Instance().CurrentMap()->UpdateInput();
		}
	}

	void World::UpdateSimulation()
	{
		if( StateManager::Instance().IsPaused() ) 
		{
			StateManager::Instance().UpdateEntitiesToCurrentFrame();			
		}
		else
		{
			// Simulation
			//
			++m_nUpdates;
			m_deltaTime = m_nUpdates / 60.0;

			UpdateTimeSystem( m_deltaTime );
			
			StateManager::Instance().SetCurrentUpdate( m_nUpdates );
		}
		
		m_camera->SetTarget( Solidus::MapManager::Instance().CurrentMap()->GetHero() );

		Solidus::MapManager::Instance().CurrentMap()->UpdateSimulation( m_deltaTime );
		
		m_camera->UpdateViewMatrix();
	}

	void World::Init()
	{		
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		m_camera->SetTarget( Solidus::MapManager::Instance().CurrentMap()->GetHero() );

		g_EventManager.Register( this, &World::CreateColorJob, "ColorJob" );	

		m_clock1 = CreateClock();
		NamedProperties args;
		m_clock1->CreateAlarm( 100, "ColorJob", args, true );
	}	

	void World::SetSize( int width_, int height_ )
	{
		m_width = width_;
		m_height = height_;
		
		glViewport( 0, 0, width_, height_ );		
		
		const float PixelsPerTile = Solidus::MapManager::Instance().CurrentMap()->PixelsPerTile();

		Vector2 windowTile( width_ / PixelsPerTile,
							height_ / PixelsPerTile);

		m_camera->SetWindowTileSize( windowTile );

		glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			glOrtho( 0, windowTile.x, 0, windowTile.y, 0.0f, 1000.0f );
		
		glMatrixMode( GL_MODELVIEW );
	}

	void World::CreateColorJob( NamedProperties& args )
	{
		std::string test;
		args.GetProperty("test", test);
		if(test == "")
		{

		}

		JobManager::Instance().CreateJob( JobPriority::Average );
	}
}