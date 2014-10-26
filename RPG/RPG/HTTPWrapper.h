#pragma once

#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include <Windows.h>
#include "PlayerColor.h"
#include "StringExtensions.h"
#include "ReportErrors.h"
#include "Agent.h"
#include "MapManager.h"
#include "AppInfo.h"

using namespace utility;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

int convertFromHex( std::string hex )
{
	int value = 0;
	int a = 0;
	int b = hex.length() - 1;
	for (; b >= 0; a++, b--)
	{
		if ( hex[b] >= '0' && hex[ b ] <= '9' )
		{
			value += (hex[b] - '0') * (1 << (a * 4));
		}
		else
		{
			switch ( hex[ b ] )
			{
			case 'A':
			case 'a':
				value += 10 * ( 1 << ( a * 4 ));
				break;
			case 'B':
			case 'b':
				value += 11 * ( 1 << ( a * 4 ));
				break;
			case 'C':
			case 'c':
				value += 12 * ( 1 << ( a * 4 ));
				break;
			case 'D':
			case 'd':
				value += 13 * ( 1 << ( a * 4 ));
				break;
			case 'E':
			case 'e':
				value += 14 * ( 1 << ( a * 4 ));
				break;
			case 'F':
			case 'f':
				value += 15 * ( 1 << ( a * 4 ));
				break;
			default:
				std::cout << "Error, invalid character '" << hex[a] << "' in hex number" << std::endl;
				break;
			}
		}
	}
	return value;
}

void hextodec( std::string hex/*, std::vector<unsigned char>& rgb*/ )
{
	if( hex.length() != 3 && hex.length() != 6 ) 
	{
		std::cout << "Invalid HEX color" << std::endl;
		return;
	}
	
	if( hex.length() == 3 ) // If hex color is shortened lets expand it.
	{
		std::string temp;
		for( int i = 0; i < 3; ++i )
		{
			temp.append( 2, hex[ i ] );
		}

		hex = temp;
	}


	// since there is no prefix attached to hex, use this code

	std::string redString	= hex.substr( 0, 2 );	
	std::string greenString = hex.substr( 2, 2 );
	std::string blueString	= hex.substr( 4, 2 );

/*
	if the prefix # was attached to hex, use the following code
	string redString = hex.substr(1, 2);
	string greenString = hex.substr(3, 2);
	string blueString = hex.substr(5, 2);
*/

/*
	if the prefix 0x was attached to hex, use the following code
	string redString = hex.substr(2, 2);
	string greenString = hex.substr(4, 2);
	string blueString = hex.substr(6, 2);
*/
	unsigned char red	= ( unsigned char )( convertFromHex( redString ));
	unsigned char green = ( unsigned char )( convertFromHex( greenString ));
	unsigned char blue	= ( unsigned char )( convertFromHex( blueString ));    

	/*rgb[ 0 ] = red;
	rgb[ 1 ] = green;
	rgb[ 2 ] = blue;*/

	g_PlayerColor.Color.x = red / 255.0f;
	g_PlayerColor.Color.y = green / 255.0f;
	g_PlayerColor.Color.z = blue / 255.0f;
}

void GetHardewareInfos()
{  
	SYSTEM_INFO siSysInfo;

	// Copy the hardware information to the SYSTEM_INFO structure.

	::GetSystemInfo( &siSysInfo );

	// Display the contents of the SYSTEM_INFO structure.
	char_t hn[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD dwLen = MAX_COMPUTERNAME_LENGTH;	

	printf("Hardware information: \n");
	if( GetComputerName( hn, &dwLen ) )
	{
		std::wcout << " Computer Name: " << hn << std::endl;
	}

	printf(" OEM ID: %u\n", siSysInfo.dwOemId);
	printf(" Number of processors: %u\n",
		siSysInfo.dwNumberOfProcessors);
	printf(" Page size: %u\n", siSysInfo.dwPageSize);
	printf(" Processor type: %u\n", siSysInfo.dwProcessorType);
	printf(" Minimum application address: %lx\n",
		siSysInfo.lpMinimumApplicationAddress);
	printf(" Maximum application address: %lx\n",
		siSysInfo.lpMaximumApplicationAddress);
	printf(" Active processor mask: %u\n",
		siSysInfo.dwActiveProcessorMask);  
}

void update_texture( const std::string textureName )
{
	//http_client client(U("http://localhost:7233"));

	//make_request( client, methods::GET, web::json::value::null(), L"/api/Values" );
}

void get_file( const std::string& file )
{
	const string_t outputFileName = std::wstring( file.begin(), file.end() );//L"avatar.png";

	// Open a stream to the file to write the HTTP response body into.
	auto fileBuffer = std::make_shared<streambuf<uint8_t>>();
	file_buffer<uint8_t>::open(outputFileName, std::ios::out).then([=](streambuf<uint8_t> outFile) -> pplx::task<http_response>
	{
		*fileBuffer = outFile; 

		// Create an HTTP request.
		// Encode the URI query since it could contain special characters like spaces.
		http_client client(U("http://localhost:56716/"));
		return client.request(methods::GET, uri_builder(U("/api/SimpleFiles")).append_query(U("fileName"), outputFileName).to_string());
	})
		// Write the response body into the file buffer.
		.then([=](http_response response) -> pplx::task< size_t >
	{
		printf("Response status code %u returned.\n", response.status_code());

		return response.body().read_to_end(*fileBuffer);
	})

		// Close the file buffer.
		.then([=]( size_t bufferSize )
	{				
		trace("Buffer size" << bufferSize);
		fileBuffer->close();

		std::string stdPath = Solidus::StringExtensions::WcharTostring( outputFileName.c_str());
		g_PlayerColor.TexturePath = stdPath;

	})

		// Wait for the entire response body to be written into the file.
		.wait();		
}

void display_field_map_json( web::json::value& jvalue)
{
	if (!jvalue.is_null())
	{
		//std::wcout << jvalue.array(2).as_string() << std::endl;
		//std::wcout << jvalue[0].as_string() << std::endl;
		//std::wcout << jvalue[1].as_string() << std::endl;		
		//traceW( "wtf" << jvalue[0].as_string());


		//int i = 0;
		for(auto iter = jvalue.begin(); iter != jvalue.end(); ++iter)
		{		

			//	for( auto t = iter->first.begin(); t != iter->first.end(); ++t )
			//	{
			//		++i;
			//	}
			//	
			//	
			//	////iter.first;
			const web::json::value &str = iter->first;
			const web::json::value &v = iter->second;		
			

			if( str.as_string() == L"Color" )
			{
				std::string simpleString = Solidus::StringExtensions::WcharTostring(v.as_string().c_str());
				hextodec( simpleString );
			}

			if( str.as_string() == L"Texture" )
			{
				std::string texture = Solidus::MapManager::Instance().CurrentMap()->GetHero()->TextureFile();
				std::string newTexture = Solidus::StringExtensions::WcharTostring(v.as_string().c_str());

				if( texture != newTexture && newTexture != Solidus::StringExtensions::EmptyString )
				{
					get_file( newTexture );
				}
			}

			if( str.as_string() == L"Manifest" )
			{
				g_AppInfo.ManifestFile = Solidus::StringExtensions::WcharTostring(v.as_string().c_str());
			}

			if( str.as_string() == L"Version" )
			{
				g_AppInfo.CurrentVersion = Solidus::StringExtensions::WcharTostring(v.as_string().c_str());
			}
			
			//simpleString.assign(str.as_string().begin(), str.as_string().end());			
			traceW( "Property: " << str.as_string().c_str() << " Value: " << v.as_string().c_str() );
			//std::wcout << "Property: " << str.as_string() << " Value: " << v.as_string() << std::endl;
			//	///*std::wcout << iter->first.as_string() << L" : " << iter->second.as_string() << std::endl;*/
		}
		//std::cout << i << std::endl;
	}
}

pplx::task<http_response> make_task_request(http_client & client, 
	method mtd, 
	web::json::value const & jvalue, 
	const std::wstring& path )
{
	return ( mtd == methods::GET || mtd == methods::HEAD ) ? 
		client.request(mtd, path/*L"/api/Values"*/) : 

	client.request(mtd, /*L"/api/Values"*/path, jvalue);
}

pplx::task< web::json::value > empty()
{
	pplx::task< web::json::value > emptyTask;
	return pplx::task_from_result( web::json::value() );
	//return emptyTask;
}

void make_request( http_client & client, method mtd, web::json::value const & jvalue, const std::wstring& path )
{
	make_task_request( client, mtd, jvalue, path )
		.then([] (http_response response) -> pplx::task< web::json::value > {


			http_headers headers = response.headers();			
			if ( response.status_code() == status_codes::OK )
			{
				std::wcout << "Content type: " << headers.content_type() << std::endl;
				std::cout << "Response returned OK!" << std::endl;
				std::cout << "Extracting result!" << std::endl;		
				return response.extract_json();
			}			

			return pplx::task_from_result( web::json::value() );/*empty();*/

	}).then([](pplx::task< web::json::value> previousTask ){

		try
		{

			web::json::value jsonObject = previousTask.get();
			if( jsonObject.is_null() )
			{
				return;
			}

			try
			{
				display_field_map_json( jsonObject );
			}
			catch ( web::json::json_exception const & e)
			{
				std::wcout << e.what() << std::endl;
			}
		}
		catch ( ... )
		{
			std::wcout << "The request did not receive a json response." << std::endl;
		}		

	}).wait();
}