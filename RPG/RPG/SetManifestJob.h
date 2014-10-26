#pragma once

#ifndef _SETMANIFESTJOB_H_
#define _SETMANIFESTJOB_H_

#include <string>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>

#include "BaseJob.h"
#include "HTTPWrapper.h"
#include "AppInfo.h"
#include "StringExtensions.h"

using namespace web::http;
using namespace web::http::client;

namespace Solidus
{
	class SetManifestJob : public BaseJob
	{
	public:
		SetManifestJob( const std::string& name, const JobPriority::Base& priority )
			: BaseJob( name, priority )
		{}

		void Execute()
		{
			trace( "Executing: " << m_name );
			http_client client(U("http://localhost:56716"));
			
			m_currentVersion = g_AppInfo.CurrentVersion;
	
			make_request( client, methods::GET, web::json::value::null(), L"/api/application/manifest/Halloween,General" );
		}

		void CallBack()
		{
			trace( "Loaded file for job " << m_name );
			if( g_AppInfo.ManifestFile != "" )
			{				
				NamedProperties args;
				args.SetProperty( "fileName", g_AppInfo.ManifestFile );
				Solidus::JobManager::Instance().CreateJob( "DownloadManifest", args, JobPriority::High );
			}			
					
		}

	private:
		std::string m_currentVersion;
	};
}

#endif