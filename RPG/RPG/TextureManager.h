#pragma once

#ifndef _TEXTURE_MANAGER_H_
#define _TEXTURE_MANAGER_H_

#include <map>
#include <string>
#include "Texture.h"

namespace Solidus
{
	class TextureManager
	{
	public:		
		// FUNCTIONS
		//
		static void			   Create();
		static void			   Destroy();
		static int			   CreateRGBATexture( int width_, int height_, int textureSlot = 0 );
		static int			   CreateDepthTexture( int width_, int height_, int textureSlot = 0 );
		std::shared_ptr< Texture >	   GetTexture( const char* szFilePath, float scale = 1.0f );
		static TextureManager& Instance();

		// INLINE FUNCTIONS
		//
		inline int Count() { return m_pathsToTextures.size(); }
	private:
		// Setting the entity as non-copyable
		//
		TextureManager() {}
		TextureManager( TextureManager const& ) {};
		TextureManager& operator=( TextureManager const& ) {};

		// DATA
		//
		static TextureManager*		      s_instance;
		std::map< std::string, std::shared_ptr< Texture > > m_pathsToTextures;
	};
}

#endif