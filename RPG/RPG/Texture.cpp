#include "stdafx.h"
#include "Texture.h"

#define STBI_HEADER_FILE_ONLY
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

#include <cassert>

#include "stb_image.c"
#include "ReportErrors.h"
#include "FileManager.h"

namespace Solidus
{
	Texture::Texture( const char* szFilePath, float scale )
		: m_textureId( 0 )
		, m_height( 0 )
		, m_width( 0 )
		, m_scale( scale )
		, m_path( szFilePath )
	{
		if( szFilePath )
		{
			Load( szFilePath );
		}
	}

	Texture::~Texture()
	{
		glDeleteTextures( 1, &m_textureId );
	}

	void Texture::Apply()
	{
		assert( m_textureId );

		glBindTexture( GL_TEXTURE_2D, m_textureId );		
	}

	void Texture::Load( const char* szFilePath )
	{
		LoadImage( szFilePath );
	}	

	void Texture::LoadImage( const char* szFilePath )
	{
		// Load the file pixels into memory.
		// Load the texture at szPath, puts it into an OpenGL texture, and returns
		// the ID of the texture. ( RGBA in byte order ).
		int nChannels;

		// nChannels will actually have the amount of bytes of the loaded image
		// Assuming all images are 4 bytes
		unsigned char *pixels = stbi_load( szFilePath, &m_width, &m_height, &nChannels, 4 ); 

		if( !pixels )
		{
			Solidus::ReportError( std::string( "Error loading texture file '") + szFilePath + "'." );
			return;
		}

		glGenTextures( 1, &m_textureId );

		glBindTexture( GL_TEXTURE_2D, m_textureId );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // gets applied when the ratio between texels and pixels is bigger than 1:1

		// Very slow operation do it only on the beginning of the game
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

		glewInit(); // TODO: GET THIS OUT OF HERE!
		glGenerateMipmap( GL_TEXTURE_2D );

		stbi_image_free( pixels );		
	}
}