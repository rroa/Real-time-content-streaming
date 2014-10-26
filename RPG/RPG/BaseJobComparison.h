#pragma once

#ifndef _BASEJOBCOMPARISON_H_
#define _BASEJOBCOMPARISON_H_

#include "BaseJob.h"

namespace Solidus
{
	class BaseJobComparison
	{
		bool reverse;
	public:
		BaseJobComparison( const bool& revparam=false )
		{
			reverse=revparam;
		}

		bool operator() ( const BaseJob& lhs, const BaseJob& rhs) const
		{
			if (reverse) 
				return ( lhs.m_priority > rhs.m_priority );
			else 
				return ( lhs.m_priority < rhs.m_priority );
		}

		bool operator() ( const BaseJob* lhs, const BaseJob* rhs) const
		{
			if (reverse) 
				return ( lhs->m_priority > rhs->m_priority );
			else 
				return ( lhs->m_priority < rhs->m_priority );
		}
	};
}

#endif