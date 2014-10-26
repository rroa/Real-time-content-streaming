#pragma once

#ifndef _SHADER_H_
#define	_SHADER_H_

#include "GLUniform.h"

#include <map>

namespace Solidus
{
	struct Material;
	class Shader
	{
	public:
		// CTOR
		//
		explicit Shader( const std::string& szShaderName );
		~Shader();

		// FUNCTIONS
		//
		void		  SetAsCurrentProgram();		
		void		  Clear();
		void		  InvalidateCurrentProgram();
		bool		  GetUniform( const std::string& name, GLUniform& outUniform );

		// INLINE FUNCTIONS
		//
		inline size_t GetNumberOfUniforms() const { return m_uniforms.size(); }

		// FRIENDS
		friend struct Material;
	private:
		// FUNCTIONS
		//
		void GetUniforms();
		void Init();
		void Destroy();

		// MEMBERS
		//
		std::string						   m_alias;
		int								   m_programId;
		std::map< std::string, GLUniform > m_uniforms;		
	};
}

#endif