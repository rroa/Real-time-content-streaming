#pragma once

#ifndef _TEXT_MANAGER_H_
#define	_TEXT_MANAGER_H_

#include "BitmapFont.h"
#include "Vector4.h"

namespace Solidus
{
	class TextManager
	{
		typedef Vector4 Color;
		enum 
		{
			ATTRIB_POSITION,
			ATTRIB_TEXCOORD,
			ATTRIB_COLOR,			
			NUM_ATTRIBUTES 
		};
	public:
		// FUNCTIONS
		//
		static void				  Create();
		static void				  Create( const BitmapFont& font );
		static void				  Destroy();
		static TextManager&		  Instance();
		void					  Print( const std::string& text, float x, float y, int fontSize, Color color, float xOffset = 0.0f, float yOffset = 0.0f );

		// INLINE FUNCTIONS
		//
		inline const BitmapFont&  GetFont() { return m_currentFont; }
		inline void				  SetFont( const BitmapFont& font ) { m_currentFont = font; }
	private:
		// CTOR
		//
		TextManager() {}
		TextManager( const BitmapFont& font );
		TextManager( TextManager const& ) {};
		TextManager& operator=( TextManager const& ) {};
		~TextManager();
		
		// FUNCTIONS
		//
		void Cleanup();
		void InitializeTextVertexBuffer();

		// TEXT MANAGER
		float					   m_padding;

		// FONT
		BitmapFont				   m_currentFont;

		// VBOS
		//
		GLuint					   m_program;
		GLuint					   m_textureUniformSlot;
		GLuint					   m_vertexBuffer;
		GLuint					   m_uvsBuffer;
		GLuint					   m_colorBuffer;		

		// SINGLETON
		static TextManager*		   s_instance;
	};
}
#endif