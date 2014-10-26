#include "stdafx.h"
#include "FileManager.h"

#include "StringExtensions.h"

#include <sstream>

namespace Solidus
{
	std::string BuildPath( const std::string& dir, const std::string& file, const std::string& fileExt )
	{
		std::stringstream path;
		path << dir << file << fileExt;

		return path.str();
	}

	std::string GetFullPath( const std::string& file )
	{
#if _WIN32
#ifndef UNICODE  
		typedef std::string String; 
#else
		typedef std::wstring String; 
#endif
		#define BUFSIZE 4096
		DWORD  retval=0;
		TCHAR  buffer[ BUFSIZE ]=TEXT(""); 		
		TCHAR** lppPart={NULL};
		
		// Assumes std::string is encoded in the current Windows ANSI codepage
		int bufferlen = ::MultiByteToWideChar(CP_ACP, 0, file.c_str(), file.size(), NULL, 0);

		if (bufferlen == 0)
		{
			// Something went wrong. Perhaps, check GetLastError() and log.
			return StringExtensions::EmptyString;;
		}

		// Allocate new LPWSTR - must deallocate it later
		LPWSTR widestr = new WCHAR[bufferlen + 1];

		::MultiByteToWideChar(CP_ACP, 0, file.c_str(), file.size(), widestr, bufferlen);

		// Ensure wide string is null terminated
		widestr[bufferlen] = 0;

		retval = GetFullPathName(widestr,
			BUFSIZE,
			buffer,
			lppPart);

		const std::wstring ws = String(buffer);
		return std::string( ws.begin(), ws.end() );
#else
		return StringExtensions::EmptyString;
#endif
	}

	std::string ExtractDirectory( const std::string& path )
	{
		return path.substr( 0, path.find_last_of( '\\' ) +1 );
	}

	std::string ExtractFilename( const std::string& path )
	{
		return path.substr( path.find_last_of( '\\' ) +1 );
	}

	std::string ChangeExtension( const std::string& path, const std::string& ext )
	{
		std::string filename = ExtractFilename( path );
		return ExtractDirectory( path ) +filename.substr( 0, filename.find_last_of( '.' ) ) +ext;
	}

	void CopyBinaryFiles( std::string initialFilePath, std::string outputFilePath )
	{

		std::ifstream source( initialFilePath, std::ios::binary);
		std::ofstream dest( outputFilePath,    std::ios::binary);

		if( source.is_open() && dest.is_open() )
		{
			std::istreambuf_iterator<char> begin_source(source);
			std::istreambuf_iterator<char> end_source;
			std::ostreambuf_iterator<char> begin_dest(dest); 
			std::copy(begin_source, end_source, begin_dest);

			source.close();
			dest.close();
		}
	}

	std::vector< std::string > ListFilesInDirectory( const std::string& directoryName )
	{
		WIN32_FIND_DATA findFileData;
		wchar_t * fileName = StringExtensions::StringToWchar(directoryName);
		HANDLE hFind = FindFirstFile(fileName, &findFileData);

		std::vector< std::string > listFileNames;
		listFileNames.push_back(StringExtensions::WcharTostring( findFileData.cFileName ));

		while (FindNextFile( hFind, &findFileData ))
		{
			listFileNames.push_back(StringExtensions::WcharTostring( findFileData.cFileName ));
		}
		
		return listFileNames;
	}

	std::string GetFileContents(const char *filename)
	{
		std::ifstream in( filename, std::ios::in | std::ios::binary );

		// If the file exists
		if (in)
		{
			std::string contents;

			// Reading the file
			in.seekg( 0, std::ios::end );
			contents.resize( static_cast< unsigned int >( in.tellg() ) );		
			in.seekg( 0, std::ios::beg );
			in.read( &contents[0], contents.size() );
			in.close();

			return(contents);
		}
		throw( errno );
	}

	int ReadFileToBuffer( char * filePath, char *& buffer )
	{
		FILE * file = fopen( filePath, "r" );

		if( file == NULL ) return 0;

		fseek( file, 0, SEEK_END );
		long fsize = ftell( file );
		fseek( file, 0, SEEK_SET );

		buffer = ( char * ) malloc( fsize + 1 );
		fread( buffer, fsize, 1, file );
		fclose( file );

		buffer[fsize] = 0;

		return fsize;
	}

	int ReadFileToBuffer( char * file, std::string& buffer )
	{
		std::ifstream t( file );
		t.seekg( 0, std::ios::end ); 
		
		int fsize = static_cast< int >( t.tellg() );
		buffer.reserve( fsize );
		t.seekg( 0, std::ios::beg );

		buffer.assign( ( std::istreambuf_iterator< char >( t )),
			std::istreambuf_iterator< char >() );

		return fsize;
	}

	std::fstream& GetFileAtLine( std::fstream& file, unsigned int lineNumber )
	{
		file.seekg( std::ios::beg ); // Beginning of file

		for( unsigned int i = 0; i < lineNumber - 1; ++i )
		{
			file.ignore( std::numeric_limits< std::streamsize >::max(), '\n' );
		}

		return file;
	}

	std::fstream& GetFileAtLine( const std::string& fileName, unsigned int lineNumber )
	{
		std::fstream file( fileName );
		return GetFileAtLine( file, lineNumber );
	}

	std::string& GetFileAtLineString( std::fstream& file, unsigned int lineNumber, std::string& lineContent )
	{
		GetFileAtLine( file, lineNumber );

		file >> lineContent;

		return lineContent;
	}

	std::string& GetFileLineAsString( const std::string& fileName, unsigned int lineNumber, std::string& lineContent )
	{
		std::fstream file( fileName );
		if( file.is_open() )
		{
			GetFileAtLine( file, lineNumber );

			file >> lineContent;

			file.close();
		}

		return lineContent;
	}

	bool CheckFileExtension( const std::string& file, const std::string& ext )
	{
		return ( file.substr( file.find_last_of(".") + 1 ) == ext );
	}
}