#pragma once

#ifndef _EXTENSIONS_H_
#define _EXTENSIONS_H_

#include <vector>
#include <string>
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace Solidus
{
	class StringExtensions
	{
	public:
		// MEMBERS
		//
		static const std::string EmptyString;

		// FUNCTIONS
		//
		static std::string				 Join( std::vector< std::string> values, std::string delim, bool toLower = false );
		static std::vector< std::string> Split( std::string str, std::string delim, bool toLower = false );
		static std::string				 ExtractString( std::string source, std::string start, std::string end, bool includeDelimiters = false );
		static std::string				 ToLower( const std::string& text );
		static std::string				 ToUpper( const std::string& text );
		static std::string				 WcharTostring(const wchar_t *wchar);
		static wchar_t *				 StringToWchar( const std::string &str );
		static bool						 CompareStrings( const std::string &lhs, const std::string &rhs );
		
		// trim from start
		static inline std::string &ltrim(std::string &s) 
		{
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			return s;
		}

		// trim from end
		static inline std::string &rtrim(std::string &s) 
		{
			s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
			return s;
		}

		// trim from both ends
		static inline std::string &trim(std::string &s) 
		{
			return ltrim(rtrim(s));
		}
	private:
		static char tolower( const char letter );
		static char toupper( const char letter );
	};
}

#endif