#pragma once

#ifndef _DOWNLOADFILE_H_
#define _DOWNLOADFILE_H_

#include <string>
#include <fstream>
#include "BaseJob.h"
#include "ReportErrors.h"
//#include "Woot.h"

namespace Solidus
{
	class DownloadFileJob : public BaseJob
	{
	public:
		DownloadFileJob( const std::string& name, const std::string& fileName, const JobPriority::Base& priority )
			: BaseJob( name, priority )	
			, m_fileName( fileName )
		{}

		void Execute()
		{
			trace( "Executing: " << m_name );
			get_file( m_fileName );
		}

		void CallBack()
		{
			trace( "Loaded file for job " << m_name );
		}

	private:
		std::string m_fileName;
	};
}

#endif