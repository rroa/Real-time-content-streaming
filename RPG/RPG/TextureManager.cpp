#include "stdafx.h"
#include "TextureManager.h"

namespace Solidus
{
	TextureManager* TextureManager::s_instance = nullptr;	

	void TextureManager::Create()
	{
		if( !s_instance )
		{
			s_instance = new TextureManager;
		}
	}

	void TextureManager::Destroy()
	{
		if( s_instance )
		{
			// Clearing the map
			s_instance->m_pathsToTextures.clear();
			
			// Resetting the instance
			delete s_instance;
			s_instance = nullptr;
		}		
	}

	std::shared_ptr< Texture > TextureManager::GetTexture( const char* szFilePath, float scale )
	{
		// Check if the texture was previously loaded
		auto iter = m_pathsToTextures.find( szFilePath );

		if( iter != m_pathsToTextures.end() )
		{
			//It was here
			return (iter->second);
		}
		else
		{
			//auto pTexture = new Texture( szFilePath, scale );
			auto pTexture = std::make_shared< Texture >( szFilePath,scale );
			m_pathsToTextures[ szFilePath ] = pTexture;
			return pTexture;
		}
	}

	TextureManager& TextureManager::Instance()
	{
		if( !s_instance )
		{
			Create();
		}

		return *s_instance;
	}

	int TextureManager::CreateRGBATexture( int width_, int height_, int textureSlot )
	{
		GLuint colorTexture = GLuint( -1 );

		// Activating the texture slot
		glActiveTexture( GL_TEXTURE0 + textureSlot );

		// Generating the texture
		glGenTextures( 1, &colorTexture );		
		

		// Binding the texture
		glBindTexture( GL_TEXTURE_2D, colorTexture );

		// Creating the texture
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

		// Texture param
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); 
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

		return colorTexture;
	}

	int TextureManager::CreateDepthTexture( int width_, int height_, int textureSlot )
	{
		GLuint depthTexture = GLuint( -1 );

		// Activating the texture slot
		glActiveTexture( GL_TEXTURE0 + textureSlot );

		// Generating the texture
		glGenTextures( 1, &depthTexture );
		
		// Binding the texture
		glBindTexture( GL_TEXTURE_2D, depthTexture );
		
		// Creating the texture
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
		
		// Texture param
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); 
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

		// Binding the texture
		//glBindTexture( GL_TEXTURE_2D, depthTexture );

		return depthTexture;
	}

}