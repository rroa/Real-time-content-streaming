#pragma once

#ifndef _FILE_MANAGER_H_
#define _FILE_MANAGER_H_
#endif

#include <fstream>
#include <string>
#include <limits>
#include <vector>

namespace Solidus
{
	// HELPERS
	//	
	std::string				   BuildPath( const std::string& dir, const std::string& file, const std::string& fileExt );
	std::string				   GetFullPath( const std::string& file );
	bool					   CheckFileExtension( const std::string& file, const std::string& ext );
	std::string				   ExtractDirectory( const std::string& path );
	std::string				   ExtractFilename( const std::string& path );
	std::string				   ChangeExtension( const std::string& path, const std::string& ext );	

	// FILE READERS
	//
	void					   CopyBinaryFiles( std::string initialFilePath, std::string outputFilePath );
	std::vector< std::string > ListFilesInDirectory( const std::string& directoryName );
	std::string				   GetFileContents( const char * filePath );
	int						   ReadFileToBuffer( const char * filePath, char *& buffer );
	int						   ReadFileToBuffer( const char * file, std::string& buffer );	
	std::fstream&			   GetFileAtLine( std::fstream& file, unsigned int lineNumber );
	std::fstream&			   GetFileAtLine( const std::string& fileName, unsigned int lineNumber );
	std::string&			   GetFileAtLineString( std::fstream& file, unsigned int lineNumber, std::string& lineContent );
	std::string&			   GetFileLineAsString( const std::string& fileName, unsigned int lineNumber, std::string& lineContent );
}