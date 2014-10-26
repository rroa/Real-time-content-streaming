#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "stdafx.h"
#include <string>

namespace Solidus
{	
	class TextureManager;
	class Texture
	{
	public:		
		// CTOR
		//
		explicit Texture( const char* szFilePath, float scale = 1.0f );
		~Texture();

		// FUNCTIONS
		//
		void		  Apply();
		static void   ApplyNullTexture();
		void		  Load( const char* szFilePath );		
		inline int	  Id() const { return static_cast<int>( m_textureId); }
		inline std::string	  Path() const { return std::string( m_path ); }
		inline int	  Width() const { return static_cast<int>( m_width * m_scale); }
		inline int	  Height() const { return static_cast<int>( m_height * m_scale); }	

		// FRIENDS
		//
		friend class TextureManager;
	private:
		// Setting the entity as non-copyable
		//
		Texture() {}
		Texture( const Texture& );/* {}*/
		Texture& operator=( const Texture& );/* {}*/

		// FUNCTIONS
		//
		void LoadImage( const char* szFilePath );		

		// DATA
		//
		int	   m_height;
		int	   m_width;
		float  m_scale;
		const char*  m_path;
		GLuint m_textureId;
	};
}

#endif