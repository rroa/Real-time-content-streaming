#pragma once

#ifndef _BITMAP_FONT_H_
#define _BITMAP_FONT_H_

#include <map>
#include <string>

#include "BitmapFontCharacter.h"

namespace Solidus
{
	class BitmapFont
	{
	public:
		// CTOR
		//
		BitmapFont();
		BitmapFont( const std::string& name, std::string locale, int sheets, float height, GLuint textureId );
		~BitmapFont();

		// FUNCTIONS
		//
		void				       AddCharacter( unsigned char _character, const BitmapFontCharacter& _characterFont );
		const BitmapFontCharacter  GetCharacter( const unsigned char& _glyph );
		const float				   CalcStringFontWidth( const char* text );
		const float				   CalcStringFontWidth( const std::string& text );
		const float		           CalcStringPixelWidth( const char* text );
		const float		           CalcStringPixelWidth( const std::string& text );

		// INLINE
		//
		const inline GLuint        GetTexture() const { return m_textureId; } 
		const inline float		   GetHeight()  const { return m_height; }
	private:				
		std::string																 m_name;
		std::string																 m_locale;
		int																		 m_sheetsAmount;
		float																	 m_height;
		GLuint																	 m_textureId;
		std::map< int, std::map< unsigned char, Solidus::BitmapFontCharacter > > m_sheets; // TODO: Add support for multiple sheets
		std::map< const unsigned char, Solidus::BitmapFontCharacter >			 m_fontCharacterMap;
	};
}

#endif