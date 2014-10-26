#pragma once

#ifndef _REPORT_ERRORS_H_
#define _REPORT_ERRORS_H_

#include <string>
#include <sstream>
#include <vector>

namespace Solidus
{
#define trace( message )			\
	{								\
	std::ostringstream s;		\
	s << message << std::endl;	\
	Solidus::TraceStringA( s.str() );		\
}

#define traceW( message )			\
	{								\
	std::basic_ostringstream<wchar_t> s;		\
	s << message << std::endl;	\
	Solidus::TraceStringW( s.str() );		\
}

#if _DEBUG
#define DEBUG(x) trace( x )
#define DEBUGW(x) traceW( x )
#else
#define DEBUG(x)
#define DEBUGW(x)
#endif

	void ReportError( const std::string& message );
	void ReportShaderError( const std::string& message, const std::string& fileName, const std::string& path );
	void TraceStringA( const std::string& message );
	void TraceStringW( const std::wstring& message);
}

#endif