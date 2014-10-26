#include "stdafx.h"
#include "BitmapFont.h"

namespace Solidus
{

	BitmapFont::BitmapFont()
		: m_sheetsAmount( 0 )
		, m_height( 0.0f )
		, m_textureId( 0 )
	{}

	BitmapFont::BitmapFont( const std::string& name, std::string locale, int sheets, float height, GLuint textureId )
		: m_name( name )
		, m_locale( locale )
		, m_sheetsAmount( sheets )
		, m_height( height )
		, m_textureId( textureId )
	{}

	void BitmapFont::AddCharacter( unsigned char _character, const BitmapFontCharacter& _characterFont )
	{
		m_fontCharacterMap[ _character ] = _characterFont;		
	}

	BitmapFont::~BitmapFont()
	{
		m_fontCharacterMap.clear();
		m_sheets.clear();
	}

	const BitmapFontCharacter BitmapFont::GetCharacter( const unsigned char& _glyph )
	{
		if( m_fontCharacterMap.size() > 0 )
		{
			std::map< const unsigned char, Solidus::BitmapFontCharacter >::iterator bitmapFontIterator;
			bitmapFontIterator = m_fontCharacterMap.find( _glyph );
			if( bitmapFontIterator != m_fontCharacterMap.end() )
			{
				return bitmapFontIterator->second;
			}
		}

		BitmapFontCharacter nullBitmapFont; // TODO: fix this warning
		return nullBitmapFont;		
	}

	const float BitmapFont::CalcStringPixelWidth( const char* text )
	{
		unsigned int length = strlen( text );
		float totalWidth = 0.0f;
		
		for ( unsigned int i = 0 ; i < length ; ++i )
		{
			BitmapFontCharacter fontCharacter = GetCharacter( text[i] );
			totalWidth += fontCharacter.GetWidth() * 25;
		}

		return totalWidth;
	}

	const float BitmapFont::CalcStringPixelWidth( const std::string& text )
	{
		return CalcStringPixelWidth( text.c_str() );
	}

	const float BitmapFont::CalcStringFontWidth( const char* text )
	{
		unsigned int length = strlen( text );
		float totalWidth = 0.0f;

		for ( unsigned int i = 0 ; i < length ; ++i )
		{
			BitmapFontCharacter fontCharacter = GetCharacter( text[i] );
			totalWidth += fontCharacter.GetWidth();
		}

		return totalWidth;
	}

	const float BitmapFont::CalcStringFontWidth( const std::string& text )
	{
		return CalcStringFontWidth( text.c_str() );
	}

}