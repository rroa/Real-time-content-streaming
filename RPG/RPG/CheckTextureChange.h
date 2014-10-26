#pragma once

#ifndef _CHECKTEXTURECHANGEJOB_H_
#define _CHECKTEXTURECHANGEJOB_H_

#include <string>
#include <fstream>
#include "BaseJob.h"
#include "Agent.h"
#include "MapManager.h"

namespace Solidus
{
	class CheckTextureChangeJob : public BaseJob
	{
	public:
		CheckTextureChangeJob( const std::string& name, const JobPriority::Base& priority )
			: BaseJob( name, priority )			
		{}

		void Execute()
		{
			trace( "Executing: " << m_name );
			std::string texture = Solidus::MapManager::Instance().CurrentMap()->GetHero()->TextureFile();

			trace( texture );
		}

		void CallBack()
		{
			trace( "Loaded file for job " << m_name );
		}

	private:
		std::string m_path;
	};
}

#endif