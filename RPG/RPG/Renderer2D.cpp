#include "stdafx.h"
#include "Renderer2D.h"

#include <time.h>

#include "Agent.h"
#include "Entity.h"
#include "Geometry.h"
#include "Map.h"
#include "TileType.h"
#include "Vector4.h"
#include "Utilities.h"
#include "PlayerColor.h"

namespace 
{
	Vector4 GenerateRandomTorchColorPattern( Vector4 lightColor )
	{
		srand( GetTickCount() );
		unsigned char torchVisibility = 0;

		float visibility_factor = 0.09f;
		torchVisibility = rand() % 40;
		lightColor = lightColor + 
			Vector4( ( 80 - torchVisibility ) / 255.0f, 
			( 80 - torchVisibility ) / 255.0f, 
			( 40 - torchVisibility ) / 255.0f, 1.0f );

		visibility_factor = 0.02f;

		Vector4 visible_offset( visibility_factor,
			visibility_factor,
			visibility_factor, 1.0f );

		lightColor = lightColor - visible_offset;

		return lightColor;
	}	
}

namespace Solidus
{
	std::unique_ptr< Renderer2D > Renderer2D::s_instance = nullptr;

	void Renderer2D::Create()
	{
		if( !s_instance )
		{
			s_instance.reset( new Renderer2D() );
		}
	}

	void Renderer2D::Destroy()
	{
		if( s_instance )
		{
			s_instance.reset();
		}		
	}

	Renderer2D& Renderer2D::Instance()
	{
		if( !s_instance )
		{
			Create();
		}

		return *s_instance.get();
	}

	void Renderer2D::DrawLevel( const Map& level ) const
	{
		float worldUnitsPerTile = level.WorldUnitsPerTile();
		Agent* hero = level.GetHero();		
		srand( GetTickCount() );
		
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();

		glDisable( GL_TEXTURE_2D );
		glBegin( GL_QUADS );

			//Draw one strip per row
			for( int y = 0; y < level.Height(); ++ y )
			{		
				// Draw one quad (two triangles) per column (tile)
				//		
				for( int x = 0; x < level.Width(); ++x )
				{
					// Retrieve the tile
					auto tile = level.GetTileAtLocation(x , y);

					// Identify our tile's type
					std::shared_ptr< TileType > tileType = tile->m_tileType;

					if( !tileType ) continue;

					// Get the color!					
					Vector4 tileColor = tileType->Color;					

					// Let's paint the tile if its visible					
					
					//////////////////////////////////////////////////////////////////////////
					Vector4 lightColor( 0.0f, 0.0f, 0.0f, 1.0f );					
					int visible = 0;
					if( hero )
					{
						visible = hero->PointVisible( x, y );
						hero->CheckUpdateRadius( x, y );
					}

					if( visible )
					{
						if( !tile->Known() ) tile->SetKnown();
						lightColor = lightColor + Vector4( 0.69215f, 0.69215f, 0.69215f, 1.0f );
						
						if ( hero->HasTorch() ) 
						{
							lightColor = GenerateRandomTorchColorPattern( lightColor );
						}
					}
					else if( tile->Known() )
					{
						lightColor = lightColor + Vector4( 0.3f, 0.39215f, 0.39215f, 0.0f );
					}					

					Vector4 color = lightColor * tileColor;

					glColor4f( color.x, color.y, color.z, color.w );

					float halfTileInWorldSpace = worldUnitsPerTile / 2.0f;

					// Setting our tile's geometric attributes
					//
					glVertex2f( ( tile->m_x - halfTileInWorldSpace ) * worldUnitsPerTile, ( tile->m_y - halfTileInWorldSpace )  * worldUnitsPerTile );
					glVertex2f( ( tile->m_x - halfTileInWorldSpace ) * worldUnitsPerTile, ( tile->m_y + halfTileInWorldSpace )  * worldUnitsPerTile );
					glVertex2f( ( tile->m_x + halfTileInWorldSpace ) * worldUnitsPerTile, ( tile->m_y + halfTileInWorldSpace )  * worldUnitsPerTile );
					glVertex2f( ( tile->m_x + halfTileInWorldSpace ) * worldUnitsPerTile, ( tile->m_y - halfTileInWorldSpace )  * worldUnitsPerTile );
				}		
			}
			
		glEnd();
		glEnable( GL_TEXTURE_2D );
		glPopMatrix();
	}

	void Renderer2D::DrawEntity( Entity* entity ) const
	{
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();		
			// Let's apply our transformations
			//
			Drawing::ApplyTransformations( entity->m_x, entity->m_y, 0, entity->m_scaleX, entity->m_scaleY );
			glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
						
			int intXCoord = static_cast< int >( entity->m_x );
			int intYCoord = static_cast< int >( entity->m_y );

			Agent* hero = MapManager::Instance().CurrentMap()->GetHero();
			int visible = 0;
			if( hero )
			{
				visible = hero->PointVisible( intXCoord, intYCoord );				
			}			
			
			Vector4 lightColor( 0.0f, 0.0f, 0.0f, 1.0f );			

			if( visible )
			{				
				lightColor = lightColor + Vector4( 0.69215f, 0.69215f, 0.69215f, 1.0f );

				if ( hero->HasTorch() ) 
				{
					lightColor = GenerateRandomTorchColorPattern( lightColor );
				}
			}
			else if( MapManager::Instance().CurrentMap()->GetTileAtLocation( intXCoord, intYCoord )->Known() )
			{
				lightColor = lightColor + Vector4( 0.3f, 0.39215f, 0.39215f, 0.0f );
			}

			Vector4 color = lightColor * entity->Color();

			// Set the color
			if( hero )
			{
				glColor4f( g_PlayerColor.Color.x,g_PlayerColor.Color.y, g_PlayerColor.Color.z, g_PlayerColor.Color.w  );
			}
			else
			{
				glColor4f( color.x, color.y, color.z, color.w );
			}			

			// Apply the texture
			//
			entity->m_texture->Apply();
			
			
			Solidus::DrawQuad( entity->m_texture->Width(), entity->m_texture->Height() );
			
		glPopMatrix();
	}
}