#pragma once

#ifndef _GETPLAYERCOLORJOB_H_
#define _GETPLAYERCOLORJOB_H_

#include <string>
#include "BaseJob.h"
#include "HTTPWrapper.h"
#include "PlayerColor.h"
#include "StringExtensions.h"

namespace Solidus
{
	class GetPlayerColorJob : public BaseJob
	{
	public:
		GetPlayerColorJob( const std::string& name, const JobPriority::Base& priority )
			: BaseJob( name, priority )
		{}

		void Execute()
		{
			trace( "Executing: " << m_name );
			http_client client(U("http://localhost:7233"));

			make_request( client, methods::GET, web::json::value::null(), L"/api/Values" );
		}

		void CallBack()
		{
			trace( "Loaded file for job " << m_name );

			if( g_PlayerColor.TexturePath != StringExtensions::EmptyString )
			{
				Solidus::MapManager::Instance().CurrentMap()->GetHero()->UpdateTexture(  g_PlayerColor.TexturePath.c_str() );
			}			
		}

	private:
		std::string m_path;
	};
}

#endif