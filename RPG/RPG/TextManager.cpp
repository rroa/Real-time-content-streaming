#include "stdafx.h"
#include "TextManager.h"
#include "Utilities.h"
#include "ResourceManager.h"

namespace Solidus
{
	void TextManager::Print( const std::string& text, float x, float y, int fontSize, Color color, float xOffset, float yOffset )
	{
		if( text.length() == 0 ) return;
		
		//
		//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

		// Offset
		//
		x += xOffset;
		y += yOffset;

		// Calculating the length of the string
		//
		unsigned int length = text.length();
		
		/*
		############################################################################
			BUFFER BUILDING MAGIC!
		############################################################################
		*/

		// buffer data containers
		//
		std::vector< Vector2 > vertices;
		std::vector< Vector2 > UVs;
		std::vector< Color > colors;

		// Keep track of previous drawn char
		//
		float previousIndex = static_cast< float >( x ) + m_padding;

		for ( unsigned int i = 0 ; i < length ; ++i )
		{
			// Initializing locals
			//
			char character								 = text[ i ];
			const BitmapFontCharacter fontCharacter      = m_currentFont.GetCharacter( character );
			std::vector< Vector2 > textureCoords		 = fontCharacter.GetTextureCoords();

			/*
			###########################
				VERTICES
			###########################
			*/

			// Building vertices
			//
			Vector2 vertex_up_left    = Vector2( previousIndex	* static_cast< float >( fontSize ), 
												 static_cast< float >( y ) + static_cast< float >( fontSize ));
			
			Vector2 vertex_up_right   = Vector2( ( previousIndex + fontCharacter.m_width ) * static_cast< float >( fontSize ), 
												 static_cast< float >( y ) + static_cast< float >( fontSize ));

			Vector2 vertex_down_right = Vector2( ( previousIndex + fontCharacter.m_width ) * static_cast< float >( fontSize ), 
												 static_cast< float >( y ));

			Vector2 vertex_down_left  = Vector2( previousIndex * static_cast< float >( fontSize ), 
												 static_cast< float >( y ));			

			// Creating the upper triangle
			//
			vertices.push_back( vertex_up_left   ); colors.push_back( color );
			vertices.push_back( vertex_down_left ); colors.push_back( color );
			vertices.push_back( vertex_up_right  ); colors.push_back( color );

			// Creating the lower triangle
			//
			vertices.push_back( vertex_down_right); colors.push_back( color );
			vertices.push_back( vertex_up_right);   colors.push_back( color );
			vertices.push_back( vertex_down_left);  colors.push_back( color );

			/*
			###########################
				TEXTURE MAPPING
			###########################
			*/

			// Font UV's
			//
			for( auto iter = textureCoords.begin(); iter != textureCoords.end(); ++iter )
			{
				UVs.push_back( *iter );
			}

			// Moving index to the right
			//
			previousIndex += fontCharacter.m_width;
		}

		// Buffer data bind
		//

		// VBO
		glBindBuffer( GL_ARRAY_BUFFER, m_vertexBuffer );
		glBufferData( GL_ARRAY_BUFFER, 
					  vertices.size() * sizeof( Vector2 ), 
					  &vertices[0], 
					  GL_STATIC_DRAW );
		
		// TextCoords
		glBindBuffer( GL_ARRAY_BUFFER, m_uvsBuffer );
		glBufferData( GL_ARRAY_BUFFER, 
					  UVs.size() * sizeof( Vector2 ), 
					  &UVs[0], 
					  GL_STATIC_DRAW );

		// Colors	
		glBindBuffer( GL_ARRAY_BUFFER, m_colorBuffer );		
		glBufferData( GL_ARRAY_BUFFER, 
					  colors.size() * sizeof( Color ), 
					  &colors[0], 
					  GL_STATIC_DRAW );


		glDisable( GL_DEPTH_TEST );

		/*
		############################################################################
			BUFFER BINDING MAGIC!
		############################################################################
		*/

		// Load the shader & its data
		//
		glUseProgram( m_program );

		// Bind texture
		//
		glActiveTexture( GL_TEXTURE8 ); // Using big slot to avoid interference with models
		glBindTexture( GL_TEXTURE_2D, m_currentFont.GetTexture() );		
		glUniform1i( m_textureUniformSlot, 8 );

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray( ATTRIB_POSITION );
		glBindBuffer( GL_ARRAY_BUFFER, m_vertexBuffer );
		glVertexAttribPointer( ATTRIB_POSITION,
							   sizeof( vertices.front() ) / sizeof( GL_FLOAT ),
							   GL_FLOAT, 
							   GL_FALSE,  
							   0,		  
							   (void*)0 );

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray( ATTRIB_TEXCOORD );
		glBindBuffer( GL_ARRAY_BUFFER, m_uvsBuffer );
		glVertexAttribPointer( ATTRIB_TEXCOORD, 
							   sizeof( UVs.front() ) / sizeof( GL_FLOAT ),
							   GL_FLOAT, 
							   GL_FALSE, 
							   0, 
							   (void*)0 );

		// 3rd attribute buffer : Colors
		glEnableVertexAttribArray( ATTRIB_COLOR );
		glBindBuffer( GL_ARRAY_BUFFER, m_colorBuffer );
		glVertexAttribPointer( ATTRIB_COLOR, 
							   sizeof( colors.front() ) / sizeof( GL_FLOAT ),
							   GL_FLOAT, 
							   GL_FALSE, 
							   0, 
							   (void*)0 );
		// Draw call
		//
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

		// Disabling buffer arrays
		//
		glDisableVertexAttribArray( ATTRIB_POSITION );
		glDisableVertexAttribArray( ATTRIB_TEXCOORD );
		glDisableVertexAttribArray( ATTRIB_COLOR );

		glUseProgram( 0 ); // Unload the shader
		glEnable( GL_DEPTH_TEST );
	}

	void TextManager::InitializeTextVertexBuffer()
	{
		// Generate buffers
		//
		glGenBuffers( 1, &m_vertexBuffer );
		glGenBuffers( 1, &m_uvsBuffer );
		glGenBuffers( 1, &m_colorBuffer );

		// Loading shader
		//
		m_program = LoadShaders( Solidus::ResourceManager::TextShader );

		// Initialize uniforms' IDs
		//
		m_textureUniformSlot = glGetUniformLocation( m_program, "textureSampler" );
	}

	/*
	###########################
		SINGLETON
	###########################
	*/
	TextManager* TextManager::s_instance = nullptr;	

	void TextManager::Create()
	{
		if( !s_instance )
		{
			s_instance = new TextManager;
		}
	}

	void TextManager::Create( const BitmapFont& font )
	{
		if( !s_instance )
		{
			s_instance = new TextManager( font );
		}
	}

	void TextManager::Destroy()
	{		
		if( s_instance )
		{
			// Clearing the map
			s_instance->Cleanup();

			// Resetting the instance
			delete s_instance;
			s_instance = nullptr;
		}		
	}

	TextManager& TextManager::Instance()
	{
		if( !s_instance )
		{
			Create();
		}

		return *s_instance;
	}

	TextManager::TextManager( const BitmapFont& font )
		: m_currentFont( font )
		, m_padding( 0.3f )
	{
		InitializeTextVertexBuffer();
	}

	TextManager::~TextManager()
	{
		Cleanup();
	}

	void TextManager::Cleanup()
	{
		// Delete buffers
		//
		glDeleteBuffers( 1, &m_vertexBuffer );
		glDeleteBuffers( 1, &m_uvsBuffer );
		glDeleteBuffers( 1, &m_colorBuffer );

		// Delete texture
		//
		GLuint textureId = m_currentFont.GetTexture();
		glDeleteTextures( 1, &textureId );

		// Delete shader
		//
		glDeleteProgram( m_program );
	}
}