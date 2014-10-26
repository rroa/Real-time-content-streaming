#pragma once

#ifndef _BaseJob_H_
#define _BaseJob_H_

#include "ReportErrors.h"

namespace Solidus
{
	namespace JobPriority
	{
		enum Base
		{
			Low		= 0,
			Average = 1,
			High	= 2,
			None	= 4
		};
	}

	class BaseJob
	{
		friend class BaseJobComparison;
	public:
		explicit BaseJob( const std::string& name, const JobPriority::Base& priority )
			: m_name( name )
			, m_priority( priority )
		{
			m_threadId = GetCurrentThreadId();
		}

		virtual void Execute()
		{
			trace( "Executing: " << m_name );
			//Sleep( 1000 );
		}

		virtual void CallBack()
		{
			trace( "Callback " << m_name );
			//Sleep( 1000 );
		}

		inline std::string Name() { return m_name; }
		inline unsigned long ThreadId() { return m_threadId; }
		bool operator <( const BaseJob& rhs ) const
		{
			return ( m_priority < rhs.m_priority );
		}

		bool operator <( const BaseJob* rhs ) const
		{
			return ( m_priority < rhs->m_priority );
		}

	protected:
		std::string		  m_name;
		int				  m_priority;
		unsigned long	  m_threadId;
	};
}

#endif