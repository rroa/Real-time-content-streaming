#include "stdafx.h"
#include "StringExtensions.h"

namespace Solidus
{
	const std::string StringExtensions::EmptyString = "";

	std::string StringExtensions::Join( std::vector< std::string> values, std::string delim, bool toLower )
	{
		std::stringstream output;

		for( auto value = values.begin(); value != values.end(); ++value )
		{
			if( toLower )
			{
				output << StringExtensions::ToLower( *value ) << delim;
			}
			else
			{
				output << *value << delim;	
			}					
		}

		return output.str().substr( 0, output.str().length() - 1 );
	}

	std::vector< std::string> StringExtensions::Split( std::string str, std::string delim, bool toLower )
	{ 
		unsigned start = 0;
		unsigned end; 
		std::vector< std::string > v; 

		if( str.length() > 0 )
		{
			while( (end = str.find( delim, start )) != std::string::npos )
			{ 
				// TODO: Fix this dirt!~
				if( toLower )
				{
					v.push_back( StringExtensions::ToLower( str.substr( start, end - start ))); 
				}
				else
				{
					v.push_back( str.substr( start, end - start )); 
				}

				start = end + delim.length(); 
			}

			// TODO: Fix this dirt!~
			if( toLower )
			{
				v.push_back( StringExtensions::ToLower( str.substr( start )));
			}
			else
			{
				v.push_back( str.substr( start )); 
			}
		}		

		return v; 
	}

	std::string StringExtensions::ExtractString( std::string source, std::string start, std::string end, bool includeDelimiters )
	{
		std::size_t startIndex = source.find( start );

		// If the starting delimiter is not found on the string
		// stop the process, you're done!
		//
		if( startIndex == std::string::npos )
		{
			return EmptyString;
		}

		// Adding the length of the delimiter to our starting index
		// this will move us to the beginning of our sub-string.
		//
		startIndex += start.length();

		// Looking for the end delimiter
		//
		std::string::size_type endIndex = source.find( end, startIndex );

		// Returning the substring between the start index and
		// the end index. If the end index is invalid then the
		// returned value is empty string.
		if( includeDelimiters )
		{
			std::string extractedMessage(start);
			extractedMessage.append( source.substr( startIndex, endIndex - startIndex ));
			extractedMessage.append(end);

			return extractedMessage;
		}

		return source.substr( startIndex, endIndex - startIndex );
	}

	char StringExtensions::tolower( const char letter )
	{
		if( letter >= 65 && letter <= 90 )
		{
			return letter + 32;
		}
		return letter;
	}

	char StringExtensions::toupper( const char letter )
	{
		if( letter >= 97 && letter <= 122 )
		{
			return letter - 32;
		}
		return letter;
	}

	std::string StringExtensions::ToLower( const std::string& text )
	{
		const int stringLength = static_cast< int > ( text.length() );
		std::vector< char > lowerString;
		lowerString.reserve( stringLength );

		for( int i = 0; i < stringLength; ++i )
		{			
			lowerString.push_back( tolower( text[i] ));
		}

		return std::string( lowerString.begin(), lowerString.end());
	}

	std::string StringExtensions::ToUpper( const std::string& text )
	{
		const int stringLength = static_cast< int > ( text.length() );
		std::vector< char > upperString;
		upperString.reserve( stringLength );

		for( int i = 0; i < stringLength; ++i )
		{			
			upperString.push_back( toupper( text[i] ));
		}

		return std::string( upperString.begin(), upperString.end());
	}

	std::string StringExtensions::WcharTostring(const wchar_t *wchar)
	{
		std::string str = "";
		int index = 0;
		while(wchar[index] != 0)
		{
			str += (char)wchar[index];
			++index;
		}
		return str;
	}

	wchar_t * StringExtensions::StringToWchar( const std::string &str )
	{
		wchar_t wchar[260];
		unsigned int index = 0;
		while(index < str.size())
		{
			wchar[index] = (wchar_t)str[index];
			++index;
		}
		wchar[index] = 0;

		return &wchar[ 0 ];
	}

	bool StringExtensions::CompareStrings( const std::string &lhs, const std::string &rhs )
	{
		return ( ToLower( lhs ) == ToLower( rhs ) );
	}
}