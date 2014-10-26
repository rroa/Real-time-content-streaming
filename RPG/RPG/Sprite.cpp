#include "stdafx.h"
#include "Sprite.h"
#include "TextureManager.h"

namespace Solidus
{
	const float EPSILON_SCALE = 0.001f;
	Sprite::Sprite() 
		: m_x( 0.0f )
		, m_y( 0.0f )		
		, m_scaleX( 1.0f )
		, m_scaleY( 1.0f )
	{}

	Sprite::Sprite( const char* szFilePath, float scaleX, float scaleY ) 
		: m_x( 0.0f )
		, m_y( 0.0f )		
		, m_scaleX( scaleX )
		, m_scaleY( scaleY )
	{
		m_texture = TextureManager::Instance().GetTexture( szFilePath, 0.1f );
	}

	Sprite::~Sprite()
	{}

	void Sprite::LoadContent( void )
	{}

	void Sprite::AlterSetScale( float rateX, float rateY )
	{
		m_scaleX *= rateX;
		m_scaleY *= rateY;

		if( m_scaleX <= EPSILON_SCALE )
		{
			m_scaleX = 0.0f;
		}

		if( m_scaleY <= EPSILON_SCALE )
		{
			m_scaleY = 0.0f;
		}
	}

	void Sprite::SetScale( float scaleX, float scaleY )
	{
		m_scaleX = scaleX;
		m_scaleY = scaleY;
	}

	void Sprite::Teleport( float newX, float newY )
	{
		m_x = newX;
		m_y = newY;
	}

	void Sprite::Translate( float x, float y )
	{
		m_x = x;
		m_y = y;
	}

	void Sprite::UpdateTexture( const char* szFilePath )
	{
		m_texture = TextureManager::Instance().GetTexture( szFilePath, 0.1f );
	}
}
