#pragma once

#ifndef _BITMAP_FONT_CHARACTER_H_
#define _BITMAP_FONT_CHARACTER_H_

#include "Vector2.h"
#include <vector>

namespace Solidus
{
	class BitmapFontCharacter
	{
	public:
		// TODO: Move to the cpp
		BitmapFontCharacter()
			: m_glyph( 0 ) // TODO: take this out, this logic should be on the fonts manager
			, m_sheet( 0 ) // TODO: take this out, this logic should be on the fonts manager
			, m_abcA( 0.0f )
			, m_abcB( 0.0f )
			, m_abcC( 0.0f )
			, m_mins( 0.0f, 0.0f )
			, m_maxs( 0.0f, 0.0f )
			, m_height( 0.0f )
			, m_width( 0.0f )
		{

		}

		// TODO: Move to the cpp
		BitmapFontCharacter( const BitmapFontCharacter& rhs )
			: m_glyph( rhs.m_glyph ) // TODO: take this out, this logic should be on the fonts manager
			, m_sheet( rhs.m_sheet ) // TODO: take this out, this logic should be on the fonts manager
			, m_abcA( rhs.m_abcA )
			, m_abcB( rhs.m_abcB )
			, m_abcC( rhs.m_abcC )
			, m_mins( rhs.m_mins )
			, m_maxs( rhs.m_mins )
			, m_height( rhs.m_height )
			, m_width( rhs.m_width )
			, m_UVs( rhs.m_UVs )
		{

		}

		BitmapFontCharacter& operator= ( const BitmapFontCharacter& rhs )
		{
			// check for self-assignment by comparing the address of the
			// implicit object and the parameter
			if (this == &rhs)
				return *this;

			// do the copy
			m_glyph = rhs.m_glyph;
			m_sheet = rhs.m_sheet;
			m_abcA = rhs.m_abcA;
			m_abcB = rhs.m_abcB;
			m_abcC = rhs.m_abcC;
			m_mins = rhs.m_mins;
			m_maxs = rhs.m_mins;
			m_height = rhs.m_height;
			m_width = rhs.m_width;
			m_UVs = rhs.m_UVs;

			// return the existing object
			return *this;
		}

		BitmapFontCharacter( unsigned char glyphIndex, int sheet, float height, float a, float b, float c, Vector2 mins, Vector2 maxs );
		
		// INLINE
		//
		inline const float GetWidth() const { return m_abcB + m_abcC; }
		inline const float A() const { return m_abcA; }
		inline const float B() const { return m_abcB; }
		inline const float C() const { return m_abcC; }
		inline const std::vector< Vector2 > GetTextureCoords() const { return m_UVs; }
	/*private:*/
		// Font data
		//
		unsigned char m_glyph;
		unsigned int  m_sheet;
		
		// Font size
		//
		float  m_abcA; // pre-draw advancement
		float  m_abcB; // draw width advancement
		float  m_abcC; // post-draw advancement		
		float  m_width;
		float  m_height;
		// Texture mapping
		//
		Vector2 m_mins;  // bottom left corner 
		Vector2 m_maxs;  // upper right corner		
		std::vector< Vector2 > m_UVs;
	};
}

#endif