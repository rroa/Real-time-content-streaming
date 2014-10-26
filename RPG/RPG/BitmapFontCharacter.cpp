#include "stdafx.h"
#include "BitmapFontCharacter.h"

namespace Solidus
{
	BitmapFontCharacter::BitmapFontCharacter( unsigned char glyphIndex, int sheet, float height, float a, float b, float c, Vector2 mins, Vector2 maxs )
		: m_glyph( glyphIndex ) // TODO: take this out, this logic should be on the fonts manager
		, m_sheet( sheet ) // TODO: take this out, this logic should be on the fonts manager
		, m_abcA( a)
		, m_abcB( b )
		, m_abcC( c )
		, m_mins( mins )
		, m_maxs( maxs )
		, m_height( height )
		, m_width( m_abcA + m_abcB + m_abcC )
	{
		/*
		  0,0 _____ 1,0	
		   |		 |
		   |		 |
		  0,1 _____ 1,1
		*/
		Vector2 uv_up_left    = Vector2( mins.x, mins.y );
		Vector2 uv_up_right   = Vector2( maxs.x, mins.y );
		Vector2 uv_down_right = Vector2( maxs.x, maxs.y );
		Vector2 uv_down_left  = Vector2( mins.x, maxs.y );
		/*
		0,0 _____ 1,0	
		|	  	 / |
		|	  /	   |
		0,1 _____ 1,1
		*/
		m_UVs.push_back( uv_up_left   );
		m_UVs.push_back( uv_down_left );
		m_UVs.push_back( uv_up_right  );

		m_UVs.push_back(uv_down_right);
		m_UVs.push_back(uv_up_right);
		m_UVs.push_back(uv_down_left);
	}
}