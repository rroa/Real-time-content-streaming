#pragma once

#ifndef _LOADFILEJOB_H_
#define _LOADFILEJOB_H_

#include <string>
#include <fstream>
#include "BaseJob.h"
#include "ReportErrors.h"
#include "FileManager.h"
#include "md5.h"

namespace Solidus
{
	class LoadFileJob : public BaseJob
	{
	public:
		LoadFileJob( const std::string& name, const std::string& path, const JobPriority::Base& priority )
			: BaseJob( name, priority )			
		{
			m_path = path;//BuildPath( "Data/", path, ".txt" );
		}

		void Execute()
		{
			trace( "Executing: " << m_name );
			std::string buffer;
			
			std::ifstream t( m_path.c_str() );
			t.seekg( 0, std::ios::end ); 

			int fsize = static_cast< int >( t.tellg() );
			buffer.reserve( fsize );
			t.seekg( 0, std::ios::beg );

			buffer.assign( ( std::istreambuf_iterator< char >( t )),
				std::istreambuf_iterator< char >() );

			
			trace( "File Content: " << buffer );
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