#pragma once

#ifndef _GAME_H_
#define	_GAME_H_

#include "World.h"

namespace Solidus
{
	class Game
	{
	public:
		// CTOR
		//
		explicit Game();
		~Game();

		// FUNCTIONS
		//
		void   Start();
		void   GoFullScreen();
		double TimeRunning();
		void   Update();
	protected:
		void   Init();
	private:
		bool					 m_enableConsole;
		std::unique_ptr< World > m_world;
		float				     m_frameRate;
		float				     m_frameTime;
		double				     m_timeElapsed;
	};
}

#endif