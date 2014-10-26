#include "stdafx.h"
#include "ReportErrors.h"

#include <functional>
#include <fstream>
#include <vector>
#include <sstream>

#include "Settings.h"
#include "StringExtensions.h"
#include "FileManager.h"

namespace Solidus
{
	void ReportError( const std::string& message )
	{		
#ifdef _WIN32
		MessageBoxA( NULL, message.c_str(), "ERROR", MB_ICONERROR );
#else
		std::cerr << message << std::endl;
#endif
	}

	void ReportShaderError( const std::string& message, const std::string& fileName, const std::string& path )
	{		
		// Retrieving the full path of the file
		//
		std::string fullPath = GetFullPath( path );

		// Splitting the error message
		//
		std::vector< std::string > splitted = StringExtensions::Split( message, ":" );

		// Retrieving the line number of the error
		//
		std::string lineNumber = StringExtensions::ExtractString( splitted[ 0 ], "(", ")" );
		
		std::stringstream output;

		// Retrieving the content at the line of the error
		//
		std::string lineContent;
		GetFileLineAsString( path, ::atoi( lineNumber.c_str() ) - 1, lineContent );

		// Printing to the output window
		//
		DEBUG( fullPath << "(" << lineNumber << "):" << splitted[ 1 ] << splitted[ 2 ] );

		output << fileName << ", line " << lineNumber << ":\n";
		output << "> " << lineContent << "\n\n";
		output << "ERROR: " << splitted[ 2 ] << "\n";
		output << "Current OpenGL version: " << glGetString( GL_VERSION ) << "\n";
		output << "RAW ERROR MESSAGE:\n";
		output << message << "\n";
		output << "The application will now close.\n";		

#ifdef _WIN32
		MessageBoxA( NULL, output.str().c_str(), "ERROR", MB_ICONERROR );
#else
		std::cerr << output.str() << std::endl;
#endif
	}

	void TraceStringA( const std::string& message)
	{
#ifdef _WIN32
		OutputDebugStringA( message.c_str() );
#else
		std::cerr << message;
#endif
	}

	void TraceStringW( const std::wstring& message)
	{
#ifdef _WIN32
		OutputDebugStringW( message.c_str() );
#else
		std::cerr << message;
#endif
	}
}