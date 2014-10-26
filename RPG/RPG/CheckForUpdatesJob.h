#pragma once

#ifndef _CHECKFORUPDATESJOB_H_
#define _CHECKFORUPDATESJOB_H_

#include <string>
#include "BaseJob.h"
#include "JobManager.h"
#include "AppInfo.h"

namespace Solidus
{
	class CheckForUpdateJob : public BaseJob
	{
	public:
		CheckForUpdateJob( const std::string& name, const JobPriority::Base& priority )
			: BaseJob( name, priority )			
		{}

		void Execute()
		{
			trace( "Executing: " << m_name );

			m_currentVersion = g_AppInfo.CurrentVersion;

			http_client client(U("http://localhost:56716"));

			make_request( client, methods::GET, web::json::value::null(), L"/api/application/version" );
		}

		void CallBack()
		{
			trace( "Loaded file for job " << m_name );

			if( m_currentVersion != g_AppInfo.CurrentVersion )
			{				
				NamedProperties args;				
				Solidus::JobManager::Instance().CreateJob( "SetManifest", args, JobPriority::High );
			}
		}

	private:
		std::string m_currentVersion;
	};
}

#endif