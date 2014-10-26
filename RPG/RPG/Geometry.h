#pragma once
#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "Vector3.h"

namespace
{
	template <typename fn_t >
	void forEachDirection( const fn_t & fn )
	{
		// 0 <=> x, 1 <=> y, 2 <=> z
		//
		for( int axis = 0; axis < 3; ++axis ) 
		{
			for( int sign = -1; sign <= 1; sign +=2 )
			{
				Vector3 normal; // Init to 0, 0, 0
				normal[ axis ] = static_cast< float >( sign );
				fn( axis, sign, normal );
			}
		}
	}

	void DrawCubeFace( int axis, int sign, const Vector3& normal, const int& skeleton = GL_TRIANGLE_FAN /*TODO: Make this non-gl specific*/ )
	{
		glLineWidth( 2.0f );

		const bool reverseWinding = normal[ axis ] > 0;
		int otherAxisA = ( axis + 1 ) % 3;
		int otherAxisB = ( axis + 2 ) % 3;

		if( reverseWinding )
		{
			std::swap( otherAxisA, otherAxisB );
		}

		Vector3 p;
		p[ axis ] = sign * 0.5f;

		glBegin( skeleton );
			for( int a = -1; a <= 1; a += 2 )
			{
				int b = a;
				for( ; b >=-1 && b <= 1 ; b += a > 0 ? -2 : +2 )
				{
					p[ otherAxisA ] = a * 0.5f;
					p[ otherAxisB ] = b * 0.5f;

					glVertex3fv( &( p.x ) );
				}
			}
		glEnd();
	}
}
namespace Solidus
{
	// FUNCTIONS
	//
	void CommonGLEnables  ();
	void CleanTextureSlots();
	void CommonGLDisables ();
	void ResetColor		  ();	
	void DrawOrigin		  ( float lineLength = 10.0f );
	void DrawCrosshair	  ( float lineLength = 0.03f );
	void DrawCube		  ( const Vector3& position, const Vector3 color = Vector3( 1.0f, 0.0f, 0.0f ), const Vector3 scale = Vector3( 1.0f, 1.0f, 1.0f ));
	void DrawSolidSphere(GLdouble radius, GLint slices, GLint stacks, const Vector3& position, const Vector3 color = Vector3( 1.0f, 0.0f, 0.0f ));
	void DrawWireSphere (GLdouble radius, GLint slices, GLint stacks, const Vector3& position, const Vector3 color = Vector3( 1.0f, 0.0f, 0.0f ));
	void DrawQuad( int sizeX ,int sizeY,
		const Vector2& uv1 = Vector2( 0.f, 0.f ),
		const Vector2& uv2 = Vector2( 1.f, 0.f ),
		const Vector2& uv3 = Vector2( 0.f, 1.f ),
		const Vector2& uv4 = Vector2( 1.f, 1.f )
		);

	struct Rectangle
	{		
		Rectangle( void )
			: left( 0 )
			, top( 0 )
			, right( 0 )
			, bottom( 0 )
		{}

		// DATA
		//
		int left;
		int right;
		int top;
		int bottom;
	};
}

#endif