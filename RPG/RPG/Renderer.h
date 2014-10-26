#pragma once

#ifndef _OBJMODEL_OPENGL_H_
#define _OBJMODEL_OPENGL_H_

#include "Camera.h"
#include "Material.h"
#include "Light.h"

namespace Solidus
{
	enum {
		ATTRIB_POSITION,
		ATTRIB_TEXCOORDS,
		ATTRIB_NORMAL,
		ATTRIB_TANGENT,
		ATTRIB_BITANGENT,
		NUM_ATTRIBUTES };

	class ModelLoaderOBJ;
	class Renderer
	{
	public:
		void BindGeometry( ModelLoaderOBJ& _model );
		void DrawGeometry( ModelLoaderOBJ& _model, 
								  Material& _material, 
								  const Camera& _camera, 
								  const std::vector< Light >& _lights,
								  float _time,
								  bool _useFog,
								  bool _useCustomEffect,
								  bool _useParallax );
	private:
		GLuint FramebufferName;
		GLuint renderedTexture;
		GLuint depthrenderbuffer;
		GLuint quad_vertexbuffer;
		GLuint quad_programID;
		GLuint texID;
		GLuint timeID;
	};
}

#endif