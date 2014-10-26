#include "stdafx.h"
#include "Utilities.h"

#include <fstream>

#include "ReportErrors.h"
#include "Settings.h"
#include "BitmapFontCharacter.h"
#include "BitmapFont.h"
#include "TextureManager.h"
#include "StringExtensions.h"

// SHADERS
//
GLuint LoadShaders( const char * shaderName )
{
	std::vector< GLuint > shadersBuffer;

	// Retrieve the supported shaders
	//
	auto supportedShaders = Solidus::Settings::Instance().GetSupportedShaders();

	// Retrieve the shaders path directory
	//
	const std::string path = Solidus::Settings::Instance().GetShadersPath();

	// Compile shaders
	//
	for( auto iter = supportedShaders.begin();  iter != supportedShaders.end(); ++iter )
	{
		ShaderType type = ( *iter );
		GLuint glShader = glCreateShader( type );

		std::string ext = Solidus::Settings::Instance().GetShaderExtension( type );

		char buffer[ 1024 ];
		char fileName [ 1024 ];
		sprintf( buffer, "%s/%s.%s", path.c_str(), shaderName, ext.c_str() );
		sprintf( fileName, "%s.%s", shaderName, ext.c_str() );

		// Loading the file into a memory stream
		//
		std::string shaderCode;
		std::ifstream shaderStream( buffer, std::ios::in );
		if( shaderStream.is_open() )
		{
			std::string line = "";
			while( getline( shaderStream, line ) ) shaderCode += "\n" + line;				
			shaderStream.close();
		}
		else
		{
			std::string errorMessage( "Can't open the path: " );
			errorMessage.append( buffer );

			Solidus::ReportError( errorMessage );

			exit( 1 );
		}

		// Shader compilation
		GLint result = GL_FALSE;
		int infoLogLength;

		// Compile Vertex Shader
		DEBUG( "Compiling shader : " << buffer );
		char const * VertexSourcePointer = shaderCode.c_str();
		glShaderSource( glShader, 1, &VertexSourcePointer , NULL );
		glCompileShader( glShader );

		// Check Shader for errors
		//
		glGetShaderiv( glShader, GL_COMPILE_STATUS, &result );
		glGetShaderiv( glShader, GL_INFO_LOG_LENGTH, &infoLogLength );
		if ( !result && infoLogLength > 0 ) // if infoLogLength has data then something bad happened
		{
			std::vector<char> vertexShaderErrorMessage( infoLogLength + 1 );
			glGetShaderInfoLog( glShader, infoLogLength, NULL, &vertexShaderErrorMessage[0] );
			Solidus::ReportShaderError( &vertexShaderErrorMessage[0], fileName, buffer );	
			exit( 1 );
		}

		shadersBuffer.push_back( glShader );
	}

	// Link the shaders program
	//
	GLuint programID = glCreateProgram();

	if( shadersBuffer.size() > 0 )
	{
		DEBUG( "Linking shaders" );			
		for( auto shaderId = shadersBuffer.begin(); shaderId != shadersBuffer.end(); ++shaderId )
		{
			glAttachShader( programID, *shaderId );
		}
	}

	glLinkProgram( programID );
	GLint result = GL_FALSE;
	int infoLogLength;

	// Check the program
	glGetProgramiv( programID, GL_LINK_STATUS, &result );
	glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLogLength );
	if ( !result && infoLogLength > 0 )
	{
		std::vector<char> programErrorMessage( infoLogLength + 1) ;
		glGetProgramInfoLog( programID, infoLogLength, NULL, &programErrorMessage[0] );
		Solidus::ReportError( &programErrorMessage[0] );
	}

	if( shadersBuffer.size() > 0 )
	{
		DEBUG( "Deleting shaders" );			
		for( auto shaderId = shadersBuffer.begin(); shaderId != shadersBuffer.end(); ++shaderId )
		{
			glDeleteShader( *shaderId );
		}
	}

	return programID;
}

GLuint LoadShaders( const std::string& shaderName )
{
	return LoadShaders( shaderName.c_str() );
}

char CapsFixUp( const char key )
{	
#ifdef _WIN32
	SHORT lshift = GetAsyncKeyState( VK_LSHIFT ) & 0x8000;
	BOOL caps = GetKeyState( VK_CAPITAL );

	if( key >= 65 && key <= 90 )
	{		
		if( caps && lshift )
		{
			return key + 32; // make lower case
		}
		else if( caps && !lshift )
		{	
			return key;
		}
		else if( lshift )
		{
			return key;
		}
		else
		{
			return key + 32; // make lower case
		}		
	}
#endif
	return key;
}

bool LoadFontXML( const char* pszFilename, const char* pszTextureFileName, Solidus::BitmapFont& bitmapFont )
{	
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file( pszFilename );

	if( !result )
	{
		Solidus::ReportError( "FONTS: Could not open the specified file. Check the path and try again." );
		return false;
	}

	pugi::xml_node root = doc.child( "FontDefinition" );

	if( !root )
	{
		Solidus::ReportError( "FONTS: Invalid file format." );
		return false;
	}

	// Parsing FontInfo
	//
	pugi::xml_node fonts = root.child("FontInfo");

	int sheets		    = fonts.attribute("numTextureSheets").as_int();
	float height		= fonts.attribute("cellPixelHeight").as_float();
	std::string name	= fonts.attribute("name").as_string();
	std::string locale	= fonts.attribute("locale").as_string();

	// Loading font texture
	//
	auto fontTexture = Solidus::TextureManager::Instance().GetTexture( pszTextureFileName );

	bitmapFont = Solidus::BitmapFont( name, locale,sheets, height, fontTexture->Id() );

	// Get glyphs by sheet.
	//	
	for(int i = 0; i < sheets; ++i )
	{
		// Building XPATH query
		//
		char buffer[256];
		sprintf_s( buffer, 256, "/FontDefinition/Glyph[@sheet='%d']", i );

		// Retrieving the font glyphs
		//
		pugi::xpath_node_set glyphs = doc.select_nodes( buffer );

		for ( pugi::xpath_node_set::const_iterator it = glyphs.begin(); it != glyphs.end(); ++it )
		{
			pugi::xpath_node node = *it;
			auto glyph = node.node();
			unsigned int sheet;
			float a, b, c;
			std::string mins_s, maxs_s;
			std::vector<std::string> mins, maxs;
			unsigned char character;

			// Meta
			//
			character = static_cast< unsigned char>( glyph.attribute("ucsIndex").as_uint() );
			sheet	  = glyph.attribute( "sheet" ).as_uint();

			// Parse coordinates
			//
			mins_s = glyph.attribute("texCoordMins").as_string();
			maxs_s = glyph.attribute("texCoordMaxs").as_string();

			mins = Solidus::StringExtensions::Split( mins_s, "," );
			maxs = Solidus::StringExtensions::Split( maxs_s, "," );

			// ABC
			//
			a = glyph.attribute("ttfA").as_float();
			b = glyph.attribute("ttfB").as_float();
			c = glyph.attribute("ttfC").as_float();

			Solidus::BitmapFontCharacter bitmapFontCharacter( character,
															  sheet,
															  height,
															  a, b, c,
															  Vector2( static_cast< float >( ::atof( mins[0].c_str() )), static_cast< float >( ::atof( mins[1].c_str() )) ),
															  Vector2( static_cast< float >( ::atof( maxs[0].c_str() )), static_cast< float >( ::atof( maxs[1].c_str() )) ));
			
			bitmapFont.AddCharacter( character, bitmapFontCharacter );
		}
	}
	return true;
}

bool LoadFontXML( const std::string& pszFilename, const char* pszTextureFileName, Solidus::BitmapFont& bitmapFont )
{
	return LoadFontXML( pszFilename.c_str(), pszTextureFileName, bitmapFont );
}

const float CalcStringPixelWidth( Solidus::BitmapFont& bitmapFont, const char* text )
{
	return bitmapFont.CalcStringPixelWidth( text );
}

const float CalcStringPixelWidth( Solidus::BitmapFont& bitmapFont, const std::string& text )
{
	return CalcStringPixelWidth( bitmapFont, text.c_str() );
}

unsigned int ToRGBA( float r, float g, float b, float a )
{
	return ( ( static_cast< unsigned int >( r * 255.0f ) & 0xFF ) << 0  )  |
		( ( static_cast< unsigned int >( g * 255.0f ) & 0xFF ) << 8  )  |
		( ( static_cast< unsigned int >( b * 255.0f ) & 0xFF ) << 16 )  |
		( ( static_cast< unsigned int >( a * 255.0f )        ) << 24 );
}


void FromRGBA( float& r, float& g, float& b, float& a, unsigned int rgba )
{
	a = ((( rgba >> 24 ) & 0xFF ) / 255.0f );
	b = ((( rgba >> 16 ) & 0xFF ) / 255.0f );
	g = ((( rgba >>  8 ) & 0xFF ) / 255.0f );
	r = ((( rgba >>  0 ) & 0xFF ) / 255.0f );
}

void Drawing::ApplyTransformations( float x, float y, float angle, float scaleX, float scaleY )
{
	glTranslatef( x, y, 0.0f );
	glRotatef( angle, 0.f, 0.f, 1.0f );
	glScalef( scaleX, scaleY, 1.0f );
}

extern "C"
{
#ifdef WIN32
#include <Rpc.h>
#else
#include <uuid/uuid.h>
#endif
}

std::string NewUUID()
{
#ifdef WIN32
	UUID uuid;
	UuidCreate ( &uuid );

	unsigned char * str;
	UuidToStringA ( &uuid, &str );

	std::string s( ( char* ) str );

	RpcStringFreeA ( &str );
#else
	uuid_t uuid;
	uuid_generate_random ( uuid );
	char s[37];
	uuid_unparse ( uuid, s );
#endif
	return s;
}