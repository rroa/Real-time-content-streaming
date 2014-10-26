#pragma once

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "Renderer2D.h"
#include "Vector2.h"
#include "Texture.h"

namespace Solidus
{
	class Sprite	
	{
		friend class Renderer2D;
	public:
		// CTOR
		//
		Sprite();
		Sprite( const char* szFilePath, float scaleX, float scaleY );
		~Sprite();

		// FUNCTIONS
		//
		virtual void Draw( const Renderer2D& painter ) = 0;
		virtual void LoadContent( void );
		void		 SetScale( float scaleX, float scaleY );
		void		 AlterSetScale( float rateX, float rateY );
		void		 Translate(float x, float y);		
		void		 Teleport( float newX, float newY );
		virtual void UpdateSimulation( double deltaTime ) = 0;
		virtual void UpdateInput( void ) = 0;
		void		 UpdateTexture( const char* szFilePath );

		// INLINE
		//		
		inline Vector2 GetLocation() const	{ return Vector2( m_x, m_y); }
		inline int	   GetWidth()  { return m_texture->Width(); }
		inline int	   GetHeight() { return m_texture->Height(); }		
		inline Vector2 GetScale() const { return Vector2( m_scaleX, m_scaleY ); }	
		inline std::string TextureFile() const { return m_texture->Path(); }
	protected:		
		float m_x;
		float m_y;		
		float m_scaleX;
		float m_scaleY;
		std::shared_ptr < Texture >	m_texture;
	};
}

#endif