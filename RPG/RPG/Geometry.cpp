#include "stdafx.h"
#include "Geometry.h"
#include "MathUtilities.h"

namespace Solidus
{
	/*
	* Compute lookup table of cos and sin values forming a cirle
	*
	* Notes:
	*    It is the responsibility of the caller to free these tables
	*    The size of the table is (n+1) to form a connected loop
	*    The last entry is exactly the same as the first
	*    The sign of n can be flipped to get the reverse loop
	*/

	static void circleTable(double **sint,double **cost,const int n)
	{
		int i;

		/* Table size, the sign of n flips the circle direction */

		const int size = abs(n);

		/* Determine the angle between samples */

		const double angle = 2*PI/(double)n;

		/* Allocate memory for n samples, plus duplicate of first entry at the end */

		*sint = (double *) calloc(sizeof(double), size+1);
		*cost = (double *) calloc(sizeof(double), size+1);

		/* Bail out if memory allocation fails, fgError never returns */

		if (!(*sint) || !(*cost))
		{
			free(*sint);
			free(*cost);
			//fgError("Failed to allocate memory in circleTable");
		}

		/* Compute cos and sin around the circle */

		for (i=0; i<size; i++)
		{
			(*sint)[i] = sin(angle*i);
			(*cost)[i] = cos(angle*i);
		}

		/* Last sample is duplicate of the first */

		(*sint)[size] = (*sint)[0];
		(*cost)[size] = (*cost)[0];
	}

	void CommonGLEnables()
	{		
		glEnable(GL_DEPTH_TEST);
		glEnable( GL_TEXTURE_2D );
		glEnable( GL_BLEND );		
		glEnable(GL_CULL_FACE);
		glPopAttrib();
	}

	void CleanTextureSlots()
	{
		// Disabling texture channels
		//
		glActiveTexture( GL_TEXTURE8 );
		glDisable( GL_TEXTURE_2D );

		glActiveTexture( GL_TEXTURE4 );
		glDisable( GL_TEXTURE_2D );

		glActiveTexture( GL_TEXTURE3 );
		glDisable( GL_TEXTURE_2D );

		glActiveTexture( GL_TEXTURE2 );
		glDisable( GL_TEXTURE_2D );

		glActiveTexture( GL_TEXTURE1 );
		glDisable( GL_TEXTURE_2D );

		glActiveTexture( GL_TEXTURE0 );
		glDisable( GL_TEXTURE_2D );		
	}

	void CommonGLDisables()
	{		
		// Invalidating the current selected shaders
		//
		glUseProgram( 0 );
		glPushAttrib( GL_ENABLE_BIT );

		glDisable( GL_CULL_FACE );
		glDisable( GL_DEPTH_TEST );

		CleanTextureSlots();
	}

	void DrawOrigin( float lineLength )
	{		
		CommonGLDisables();		
		glBegin( GL_LINES );
		glColor3f(   1.0f,  0.0f,  0.0f );
		glVertex3f(  0.0f,  0.0f,  0.0f );

		glColor3f(   1.0f,  0.0f,  0.0f );
		glVertex3f(  lineLength,  0.0f,  0.0f );

		glColor3f(   0.0f,  1.0f,  0.0f );
		glVertex3f(  0.0f,  0.0f,  0.0f );

		glColor3f(   0.0f,  1.0f,  0.0f );
		glVertex3f(  0.0f,  lineLength,  0.0f );

		glColor3f(   0.0f,  0.0f,  1.0f );
		glVertex3f(  0.0f,  0.0f,  0.0f );

		glColor3f(   0.0f,  0.0f,  1.0f );
		glVertex3f(  0.0f,  0.0f,  lineLength );
		glEnd();

		// Resetting everything
		//
		CommonGLEnables();		

		ResetColor();
	}

	void DrawCrosshair( float lineLength )
	{
		CommonGLDisables();

		// PUSH
		//
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glBegin( GL_LINES );
		glColor3f(   1,  0,  0 );
		glVertex3f(  -lineLength * 0.5f,  0,  0 );
		glVertex3f(  lineLength * 0.5f,  0,  0 );

		glVertex3f(  0, -lineLength,  0 );
		glVertex3f(  0,  lineLength,  0 );
		glEnd();

		// POP
		//
		glMatrixMode( GL_PROJECTION );
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		CommonGLEnables();
		ResetColor();
	}

	void ResetColor()
	{
		glColor3f( 1.0f, 1.0f, 1.0f );
	}

	void DrawCube( const Vector3& position, const Vector3 color, const Vector3 scale )
	{
		CommonGLDisables();

		// PUSH
		//
		glPushMatrix();

		glColor3f( color.x, color.y, color.z ); // Setting the color
		glTranslatef( position.x, position.y, position.z );
		glScalef( scale.x, scale.y, scale.z );

		forEachDirection( []( int axis, int sign, const Vector3& normal ) 
		{
			DrawCubeFace( axis, sign, normal );
		});

		// POP
		//
		glPopMatrix();

		CommonGLEnables();
		ResetColor();
	}

	/*
	* Draws a solid sphere
	*/
	void DrawSolidSphere(GLdouble radius, GLint slices, GLint stacks, const Vector3& position, const Vector3 color )
	{
		CommonGLDisables();

		// PUSH
		//
		glPushMatrix();

		glColor3f( color.x, color.y, color.z ); // Setting the color
		glTranslatef( position.x, position.y, position.z );
		glScalef( 0.1f, 0.1f, 0.1f ); // TODO: Add this as a function param

		int i,j;

		/* Adjust z and radius as stacks are drawn. */

		double z0,z1;
		double r0,r1;

		/* Pre-computed circle */

		double *sint1,*cost1;
		double *sint2,*cost2;
		circleTable(&sint1,&cost1,-slices);
		circleTable(&sint2,&cost2,stacks*2);

		/* The top stack is covered with a triangle fan */

		z0 = 1.0;
		z1 = cost2[1];
		r0 = 0.0;
		r1 = sint2[1];

		glBegin(GL_TRIANGLE_FAN);

		glNormal3d(0,0,1);
		glVertex3d(0,0,radius);

		for (j=slices; j>=0; j--)
		{       
			glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
			glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
		}

		glEnd();

		/* Cover each stack with a quad strip, except the top and bottom stacks */

		for( i=1; i<stacks-1; i++ )
		{
			z0 = z1; z1 = cost2[i+1];
			r0 = r1; r1 = sint2[i+1];

			glBegin(GL_QUAD_STRIP);

			for(j=0; j<=slices; j++)
			{
				glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
				glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
				glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
				glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
			}

			glEnd();
		}

		/* The bottom stack is covered with a triangle fan */

		z0 = z1;
		r0 = r1;

		glBegin(GL_TRIANGLE_FAN);

		glNormal3d(0,0,-1);
		glVertex3d(0,0,-radius);

		for (j=0; j<=slices; j++)
		{
			glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
			glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
		}

		glEnd();

		/* Release sin and cos tables */

		free(sint1);
		free(cost1);
		free(sint2);
		free(cost2);

		// POP
		//
		glPopMatrix();

		CommonGLEnables();
		ResetColor();
	}

	void DrawWireSphere(GLdouble radius, GLint slices, GLint stacks, const Vector3& position, const Vector3 color )
	{
		CommonGLDisables();

		// PUSH
		//
		glPushMatrix();

		glColor3f( color.x, color.y, color.z ); // Setting the color
		glTranslatef( position.x, position.y, position.z );
		glScalef( 0.1f, 0.1f, 0.1f );

		int i,j;

		/* Adjust z and radius as stacks and slices are drawn. */

		double r;
		double x,y,z;

		/* Pre-computed circle */

		double *sint1,*cost1;
		double *sint2,*cost2;
		circleTable(&sint1,&cost1,-slices  );
		circleTable(&sint2,&cost2, stacks*2);

		/* Draw a line loop for each stack */

		for (i=1; i<stacks; i++)
		{
			z = cost2[i];
			r = sint2[i];

			glBegin(GL_LINE_LOOP);

			for(j=0; j<=slices; j++)
			{
				x = cost1[j];
				y = sint1[j];

				glNormal3d(x,y,z);
				glVertex3d(x*r*radius,y*r*radius,z*radius);
			}

			glEnd();
		}

		/* Draw a line loop for each slice */

		for (i=0; i<slices; i++)
		{
			glBegin(GL_LINE_STRIP);

			for(j=0; j<=stacks; j++)
			{
				x = cost1[i]*sint2[j];
				y = sint1[i]*sint2[j];
				z = cost2[j];

				glNormal3d(x,y,z);
				glVertex3d(x*radius,y*radius,z*radius);
			}

			glEnd();
		}

		/* Release sin and cos tables */

		free(sint1);
		free(cost1);
		free(sint2);
		free(cost2);

		// POP
		//
		glPopMatrix();

		CommonGLEnables();
		ResetColor();
	}

	void DrawQuad( int sizeX ,int sizeY, 
		const Vector2& uv1, 
		const Vector2& uv2,
		const Vector2& uv3,
		const Vector2& uv4
		)
	{
		glBegin( GL_TRIANGLE_STRIP );

			glTexCoord2f( uv1.x, uv1.y );
			glVertex2i( -sizeX,  sizeY );

			glTexCoord2f( uv2.x, uv2.y );
			glVertex2i(  sizeX,  sizeY );

			glTexCoord2f( uv3.x, uv3.y );
			glVertex2i( -sizeX, -sizeY);

			glTexCoord2f( uv4.x, uv4.y );
			glVertex2i(  sizeX, -sizeY );

		glEnd();
	}
}