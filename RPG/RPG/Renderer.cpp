#include "stdafx.h"
#include "Renderer.h"

#include "ModelLoaderOBJ.h"
#include "Vertex.h"
#include "Utilities.h"
#include "Matrix44.h"
#include "FogParameters.h"
#include "Geometry.h"
#include "ParallaxParameters.h"

namespace Solidus
{
	void Renderer::BindGeometry( ModelLoaderOBJ& _model )
	{				
		//const Solidus::TriangleSet  *pMesh = 0;
		//const Solidus::MaterialData *pMaterial = 0;
		//const Solidus::Vertex *pVertices = 0;
		std::map<std::string, GLuint>::const_iterator iter;

		for (int i = 0; i < _model.GetNumberOfTriangleSets(); ++i)
		{
			// Load VBO into buffers
			//						
			glGenBuffers( 1, &_model.vertexbuffer );
			glBindBuffer( GL_ARRAY_BUFFER, _model.vertexbuffer );
			glBufferData( GL_ARRAY_BUFFER, _model.GetVertexSize() * _model.GetNumberOfVertices(), 
				_model.GetVertexBuffer() + i, GL_STATIC_DRAW );

			// Generate a buffer for the indices as well
			//					
			glGenBuffers( 1, &_model.elementbuffer );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _model.elementbuffer );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, _model.GetNumberOfIndices() * _model.GetIndexSize(), 
				_model.GetIndexBuffer() , GL_STATIC_DRAW );

			glFlush();
		}

		// ---------------------------------------------
		// Render to Texture - specific code begins here
		// ---------------------------------------------

		// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
		//GLuint FramebufferName = 0;
		glGenFramebuffers(1, &FramebufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

		// The texture we're going to render to
		//GLuint renderedTexture;
		glGenTextures(1, &renderedTexture);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, renderedTexture);

		// Give an empty image to OpenGL ( the last "0" means "empty" )
		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, 1920, 1080, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

		// Poor filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// The depth buffer
		//GLuint depthrenderbuffer;
		glGenRenderbuffers(1, &depthrenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1920, 1980);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

		//// Alternative : Depth texture. Slower, but you can sample it later in your shader
		//GLuint depthTexture;
		//glGenTextures(1, &depthTexture);
		//glBindTexture(GL_TEXTURE_2D, depthTexture);
		//glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT24, 1024, 768, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Set "renderedTexture" as our colour attachement #0
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

		//// Depth texture alternative : 
		//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, renderedTexture, 0);


		// Set the list of draw buffers.
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

		// Always check that our framebuffer is ok
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return;


		// The fullscreen quad's FBO
		static const GLfloat g_quad_vertex_buffer_data[] = { 
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			1.0f,  1.0f, 0.0f,
		};

		//GLuint quad_vertexbuffer;
		glGenBuffers(1, &quad_vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

		// Create and compile our GLSL program from the shaders
		/*GLuint*/ quad_programID = LoadShaders( "WobblyTexture" );
		/*GLuint*/ texID = glGetUniformLocation(quad_programID, "renderedTexture");
		/*GLuint*/ timeID = glGetUniformLocation(quad_programID, "time");

	}

	void Renderer::DrawGeometry( ModelLoaderOBJ& _model, 
										Material& _material, 
										const Camera& _camera, 
										const std::vector< Light >& _lights,
										float _time,
										bool _useFog,
										bool _useCustomEffect,
										bool _useParallax )
	{	
		// Render to our framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glViewport(0,0,1920,1080); // Render on the whole framebuffer, complete from the lower left corner to the upper right
		/*
		############################################################################
		MATERIAL SETUP
		############################################################################
		*/

		_material.CurrentShader->SetAsCurrentProgram();		

		GLUniform u_time;
		if( _material.CurrentShader->GetUniform( "u_time", u_time ))
		{
			glUniform1f( u_time.Location, _time );
		}

		// Setup
		//
		_material.SetupMatrices( _camera );
		_material.SetupLights( _lights );	
		_material.SetupTextures();
		_material.SetupParallax( g_ParallaxParameters.Bias, g_ParallaxParameters.Scale );

		GLUniform u_UseFog;
		if( _material.CurrentShader->GetUniform( "u_UseFog", u_UseFog ))
		{
			glUniform1i( u_UseFog.Location, _useFog );
		}

		if( _useFog )
		{
			_material.SetupFog( g_FogParameters.Density, g_FogParameters.Start, 
				g_FogParameters.End, g_FogParameters.Color, g_FogParameters.Mode );
		}

		GLUniform u_UseCustomEffect;
		if( _material.CurrentShader->GetUniform( "u_UseSpecialEffect", u_UseCustomEffect ))
		{
			glUniform1i( u_UseCustomEffect.Location, _useCustomEffect );
		}		

		GLUniform u_UseNormalTextureSampler;
		if( _material.CurrentShader->GetUniform( "test", u_UseNormalTextureSampler ))
		{
			glUniform1i( u_UseNormalTextureSampler.Location, _useParallax );
		}	

		/*
		############################################################################
		BUFFER BINDING MAGIC!
		############################################################################
		*/							

		glBindBuffer( GL_ARRAY_BUFFER, _model.vertexbuffer );

		glEnableVertexAttribArray( ATTRIB_POSITION );			
		glVertexAttribPointer(
			ATTRIB_POSITION, 
			3,               
			GL_FLOAT,        
			GL_FALSE,        
			_model.GetVertexSize(),
			(void*) offsetof( Solidus::Vertex, position )
			);

		glEnableVertexAttribArray( ATTRIB_TEXCOORDS );			
		glVertexAttribPointer(
			ATTRIB_TEXCOORDS,                     
			2,                     
			GL_FLOAT,              
			GL_FALSE,              
			_model.GetVertexSize(),
			(void*) offsetof( Solidus::Vertex, texCoords )
			);

		glEnableVertexAttribArray( ATTRIB_NORMAL );			
		glVertexAttribPointer(
			ATTRIB_NORMAL,
			3,            
			GL_FLOAT,     
			GL_FALSE,     
			_model.GetVertexSize(),                 
			(void*) offsetof( Solidus::Vertex, normals )
			);

		glEnableVertexAttribArray( ATTRIB_TANGENT );			
		glVertexAttribPointer(
			ATTRIB_TANGENT,
			4,             
			GL_FLOAT,      
			GL_FALSE,      
			_model.GetVertexSize(),
			(void*) offsetof( Solidus::Vertex, tangent )
			);

		glEnableVertexAttribArray( ATTRIB_BITANGENT );			
		glVertexAttribPointer(
			ATTRIB_BITANGENT,      
			3,                     
			GL_FLOAT,              
			GL_FALSE,              
			_model.GetVertexSize(),
			(void*) offsetof( Solidus::Vertex, bitangent )
			);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _model.elementbuffer);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,      // mode
			_model.GetNumberOfIndices(),    // count
			GL_UNSIGNED_INT,   // type
			(void*) 0           // element array buffer offset
			);

		glDisableVertexAttribArray( ATTRIB_POSITION );
		glDisableVertexAttribArray( ATTRIB_TEXCOORDS );
		glDisableVertexAttribArray( ATTRIB_NORMAL );
		glDisableVertexAttribArray( ATTRIB_TANGENT );
		glDisableVertexAttribArray( ATTRIB_BITANGENT );

		// Invalidating current shader
		//
		_material.CurrentShader->InvalidateCurrentProgram();

		CleanTextureSlots();

		// Render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0,1920,1080); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		

		// Use our shader
		glUseProgram(quad_programID);
		glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, renderedTexture);
		// Set our "renderedTexture" sampler to user Texture Unit 0
		glUniform1i(texID, 9);

		glUniform1f(timeID, (float)(_time*10.0f) );

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		// Draw the triangles !
		glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

		glDisableVertexAttribArray(0);
	}
}