#pragma once

#ifndef _GLUNIFORM_H_
#define	_GLUNIFORM_H_

#include <string>
#include "StringExtensions.h"

namespace Solidus
{
	enum GLUniformType
	{
		U_INVALID,
		U_FLOAT,
		U_VEC2,
		U_VEC3,
		U_VEC4,
		U_DOUBLE,
		U_DVEC2,
		U_DVEC3,
		U_DVEC4,
		U_INT,
		U_IVEC2,
		U_IVEC3,
		U_IVEC4,
		U_UNSIGNED_INT,
		U_UVEC2,
		U_UVEC3,
		U_UVEC4,
		U_BOOL,
		U_BVEC2,
		U_BVEC3,
		U_BVEC4,
		U_MAT2,
		U_MAT3,
		U_MAT4,
		U_MAT2X3,
		U_MAT2X4,
		U_MAT3X2,
		U_MAT3X4,
		U_MAT4X2,
		U_MAT4X3,
		U_DMAT2,
		U_DMAT3,
		U_DMAT4,
		U_DMAT2X3,
		U_DMAT2X4,
		U_DMAT3X2,
		U_DMAT3X4,
		U_DMAT4X2,
		U_DMAT4X3,
		U_SAMPLER1D,
		U_SAMPLER2D,
		U_SAMPLER3D,
		U_SAMPLERCUBE,
		U_SAMPLER1DSHADOW,
		U_SAMPLER2DSHADOW,
		U_SAMPLER1DARRAY,
		U_SAMPLER2DARRAY,
		U_SAMPLER1DARRAYSHADOW,
		U_SAMPLER2DARRAYSHADOW,
		U_SAMPLER2DMS,
		U_SAMPLER2DMSARRAY,
		U_SAMPLERCUBESHADOW,
		U_SAMPLERBUFFER,
		U_SAMPLER2DRECT,
		U_SAMPLER2DRECTSHADOW,
		U_ISAMPLER1D,
		U_ISAMPLER2D,
		U_ISAMPLER3D,
		U_ISAMPLERCUBE,
		U_ISAMPLER1DARRAY,
		U_ISAMPLER2DARRAY,
		U_ISAMPLER2DMS,
		U_ISAMPLER2DMSARRAY,
		U_ISAMPLERBUFFER,
		U_ISAMPLER2DRECT,
		U_USAMPLER1D,
		U_USAMPLER2D,
		U_USAMPLER3D,
		U_USAMPLERCUBE,
		U_USAMPLER2DARRAY,		
		U_USAMPLER2DMS,
		U_USAMPLER2DMSARRAY,
		U_USAMPLERBUFFER,
		U_USAMPLER2DRECT,
		U_IMAGE1D,
		U_IMAGE2D,
		U_IMAGE3D,
		U_IMAGE2DRECT,
		U_IMAGECUBE,
		U_IMAGEBUFFER,
		U_IMAGE1DARRAY,
		U_IMAGE2DARRAY,
		U_IMAGE2DMS,
		U_IMAGE2DMSARRAY,
		U_IIMAGE1D,
		U_IIMAGE2D,
		U_IIMAGE3D,
		U_IIMAGE2DRECT,
		U_IIMAGECUBE,
		U_IIMAGEBUFFER,
		U_IIMAGE1DARRAY,
		U_IIMAGE2DARRAY,
		U_IIMAGE2DMS,
		U_IIMAGE2DMSARRAY,
		U_UIMAGE1D,
		U_UIMAGE2D,
		U_UIMAGE3D,
		U_UIMAGE2DRECT,
		U_UIMAGECUBE,
		U_UIMAGEBUFFER,
		U_UIMAGE1DARRAY,
		U_UIMAGE2DARRAY,
		U_UIMAGE2DMS,
		U_UIMAGE2DMSARRAY,
		U_ATOMIC_UINT,
		NUM_GLUNIFORM_TYPES
	};

	struct GLUniform
	{
		// CTOR
		//
		GLUniform()
			: Name	  ( StringExtensions::EmptyString )			
			, Size	  ( -1 )
			, Location( -1 )
		{}

		GLUniform( const char * _name, GLenum _type, GLint _size, GLint _location )
			: Name	  ( _name )			
			, Size	  ( _size )
			, Location( _location )
		{
			Type = TranslateEnum( _type );
		}

		// MEMBERS
		//
		std::string	  Name;
		GLUniformType Type;
		GLint		  Size;							
		GLint		  Location;
	private:
		GLUniformType TranslateEnum( GLenum _gltype )
		{
			switch ( _gltype )
			{
			case	GL_FLOAT									:	return U_FLOAT;
			case	GL_FLOAT_VEC2								:	return U_VEC2;
			case	GL_FLOAT_VEC3								:	return U_VEC3;
			case	GL_FLOAT_VEC4								:	return U_VEC4;
			case	GL_DOUBLE									:	return U_DOUBLE;
			case	GL_DOUBLE_VEC2								:	return U_DVEC2;
			case	GL_DOUBLE_VEC3								:	return U_DVEC3;
			case	GL_DOUBLE_VEC4								:	return U_DVEC4;
			case	GL_INT										:	return U_INT;
			case	GL_INT_VEC2									:	return U_IVEC2;
			case	GL_INT_VEC3									:	return U_IVEC3;
			case	GL_INT_VEC4									:	return U_IVEC4;
			case	GL_UNSIGNED_INT								:	return U_UNSIGNED_INT;
			case	GL_UNSIGNED_INT_VEC2						:	return U_UVEC2;
			case	GL_UNSIGNED_INT_VEC3						:	return U_UVEC3;
			case	GL_UNSIGNED_INT_VEC4						:	return U_UVEC4;
			case	GL_BOOL										:	return U_BOOL;
			case	GL_BOOL_VEC2								:	return U_BVEC2;
			case	GL_BOOL_VEC3								:	return U_BVEC3;
			case	GL_BOOL_VEC4								:	return U_BVEC4;
			case	GL_FLOAT_MAT2								:	return U_MAT2;
			case	GL_FLOAT_MAT3								:	return U_MAT3;
			case	GL_FLOAT_MAT4								:	return U_MAT4;
			case	GL_FLOAT_MAT2x3								:	return U_MAT2X3;
			case	GL_FLOAT_MAT2x4								:	return U_MAT2X4;
			case	GL_FLOAT_MAT3x2								:	return U_MAT3X2;
			case	GL_FLOAT_MAT3x4								:	return U_MAT3X4;
			case	GL_FLOAT_MAT4x2								:	return U_MAT4X2;
			case	GL_FLOAT_MAT4x3								:	return U_MAT4X3;
			case	GL_DOUBLE_MAT2								:	return U_DMAT2;
			case	GL_DOUBLE_MAT3								:	return U_DMAT3;
			case	GL_DOUBLE_MAT4								:	return U_DMAT4;
			case	GL_DOUBLE_MAT2x3							:	return U_DMAT2X3;
			case	GL_DOUBLE_MAT2x4							:	return U_DMAT2X4;
			case	GL_DOUBLE_MAT3x2							:	return U_DMAT3X2;
			case	GL_DOUBLE_MAT3x4							:	return U_DMAT3X4;
			case	GL_DOUBLE_MAT4x2							:	return U_DMAT4X2;
			case	GL_DOUBLE_MAT4x3							:	return U_DMAT4X3;
			case	GL_SAMPLER_1D								:	return U_SAMPLER1D;
			case	GL_SAMPLER_2D								:	return U_SAMPLER2D;
			case	GL_SAMPLER_3D								:	return U_SAMPLER3D;
			case	GL_SAMPLER_CUBE								:	return U_SAMPLERCUBE;
			case	GL_SAMPLER_1D_SHADOW						:	return U_SAMPLER1DSHADOW;
			case	GL_SAMPLER_2D_SHADOW						:	return U_SAMPLER2DSHADOW;
			case	GL_SAMPLER_1D_ARRAY							:	return U_SAMPLER1DARRAY;
			case	GL_SAMPLER_2D_ARRAY							:	return U_SAMPLER2DARRAY;
			case	GL_SAMPLER_1D_ARRAY_SHADOW					:	return U_SAMPLER1DARRAYSHADOW;
			case	GL_SAMPLER_2D_ARRAY_SHADOW					:	return U_SAMPLER2DARRAYSHADOW;
			case	GL_SAMPLER_2D_MULTISAMPLE					:	return U_SAMPLER2DMS;
			case	GL_SAMPLER_2D_MULTISAMPLE_ARRAY				:	return U_SAMPLER2DMSARRAY;
			case	GL_SAMPLER_CUBE_SHADOW						:	return U_SAMPLERCUBESHADOW;
			case	GL_SAMPLER_BUFFER							:	return U_SAMPLERBUFFER;
			case	GL_SAMPLER_2D_RECT							:	return U_SAMPLER2DRECT;
			case	GL_SAMPLER_2D_RECT_SHADOW					:	return U_SAMPLER2DRECTSHADOW;
			case	GL_INT_SAMPLER_1D							:	return U_ISAMPLER1D;
			case	GL_INT_SAMPLER_2D							:	return U_ISAMPLER2D;
			case	GL_INT_SAMPLER_3D							:	return U_ISAMPLER3D;
			case	GL_INT_SAMPLER_CUBE							:	return U_ISAMPLERCUBE;
			case	GL_INT_SAMPLER_1D_ARRAY						:	return U_ISAMPLER1DARRAY;
			case	GL_INT_SAMPLER_2D_ARRAY						:	return U_ISAMPLER2DARRAY;
			case	GL_INT_SAMPLER_2D_MULTISAMPLE				:	return U_ISAMPLER2DMS;
			case	GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY			:	return U_ISAMPLER2DMSARRAY;
			case	GL_INT_SAMPLER_BUFFER						:	return U_ISAMPLERBUFFER;
			case	GL_INT_SAMPLER_2D_RECT						:	return U_ISAMPLER2DRECT;
			case	GL_UNSIGNED_INT_SAMPLER_1D					:	return U_USAMPLER1D;
			case	GL_UNSIGNED_INT_SAMPLER_2D					:	return U_USAMPLER2D;
			case	GL_UNSIGNED_INT_SAMPLER_3D					:	return U_USAMPLER3D;
			case	GL_UNSIGNED_INT_SAMPLER_CUBE				:	return U_USAMPLERCUBE;
			case	GL_UNSIGNED_INT_SAMPLER_1D_ARRAY			:	return U_USAMPLER2DARRAY;
			case	GL_UNSIGNED_INT_SAMPLER_2D_ARRAY			:	return U_USAMPLER2DARRAY;
			case	GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE		:	return U_USAMPLER2DMS;
			case	GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:	return U_USAMPLER2DMSARRAY;
			case	GL_UNSIGNED_INT_SAMPLER_BUFFER				:	return U_USAMPLERBUFFER;
			case	GL_UNSIGNED_INT_SAMPLER_2D_RECT				:	return U_USAMPLER2DRECT;
			case	GL_IMAGE_1D									:	return U_IMAGE1D;
			case	GL_IMAGE_2D									:	return U_IMAGE2D;
			case	GL_IMAGE_3D									:	return U_IMAGE3D;
			case	GL_IMAGE_2D_RECT							:	return U_IMAGE2DRECT;
			case	GL_IMAGE_CUBE								:	return U_IMAGECUBE;
			case	GL_IMAGE_BUFFER								:	return U_IMAGEBUFFER;
			case	GL_IMAGE_1D_ARRAY							:	return U_IMAGE1DARRAY;
			case	GL_IMAGE_2D_ARRAY							:	return U_IMAGE2DARRAY;
			case	GL_IMAGE_2D_MULTISAMPLE						:	return U_IMAGE2DMS;
			case	GL_IMAGE_2D_MULTISAMPLE_ARRAY				:	return U_IMAGE2DMSARRAY;
			case	GL_INT_IMAGE_1D								:	return U_IIMAGE1D;
			case	GL_INT_IMAGE_2D								:	return U_IIMAGE2D;
			case	GL_INT_IMAGE_3D								:	return U_IIMAGE3D;
			case	GL_INT_IMAGE_2D_RECT						:	return U_IIMAGE2DRECT;
			case	GL_INT_IMAGE_CUBE							:	return U_IIMAGECUBE;
			case	GL_INT_IMAGE_BUFFER							:	return U_IIMAGEBUFFER;
			case	GL_INT_IMAGE_1D_ARRAY						:	return U_IIMAGE1DARRAY;
			case	GL_INT_IMAGE_2D_ARRAY						:	return U_IIMAGE2DARRAY;
			case	GL_INT_IMAGE_2D_MULTISAMPLE					:	return U_IIMAGE2DMS;
			case	GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY			:	return U_IIMAGE2DMSARRAY;
			case	GL_UNSIGNED_INT_IMAGE_1D					:	return U_UIMAGE1D;
			case	GL_UNSIGNED_INT_IMAGE_2D					:	return U_UIMAGE2D;
			case	GL_UNSIGNED_INT_IMAGE_3D					:	return U_UIMAGE3D;
			case	GL_UNSIGNED_INT_IMAGE_2D_RECT				:	return U_UIMAGE2DRECT;
			case	GL_UNSIGNED_INT_IMAGE_CUBE					:	return U_UIMAGECUBE;
			case	GL_UNSIGNED_INT_IMAGE_BUFFER				:	return U_UIMAGEBUFFER;
			case	GL_UNSIGNED_INT_IMAGE_1D_ARRAY				:	return U_UIMAGE1DARRAY;
			case	GL_UNSIGNED_INT_IMAGE_2D_ARRAY				:	return U_UIMAGE2DARRAY;
			case	GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE		:	return U_UIMAGE2DMS;
			case	GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY	:	return U_UIMAGE2DMSARRAY;
			case	GL_UNSIGNED_INT_ATOMIC_COUNTER				:	return U_ATOMIC_UINT;
			default: return U_INVALID;
			}
		}
	};
}

#endif