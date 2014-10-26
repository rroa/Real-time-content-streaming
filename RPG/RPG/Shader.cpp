#include "stdafx.h"
#include "Shader.h"

#include "Utilities.h"
#include "StringExtensions.h"

namespace Solidus
{
	Shader::Shader( const std::string& szShaderName )
		: m_alias( szShaderName )
	{
		Init();
	}

	Shader::~Shader()
	{
		Destroy();
	}

	void Shader::Init()
	{
		m_programId = LoadShaders( m_alias );
		GetUniforms();
	}

	void Shader::GetUniforms()
	{

		GLint numUniforms	   = 0;
		GLint uniformMaxLength = 0;

		// Retrieving shader uniforms meta-data
		//
		glGetProgramiv( m_programId, GL_ACTIVE_UNIFORMS, &numUniforms );		
		glGetProgramiv( m_programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength );

		// Retrievable uniform info
		//
		GLint size	     = -1;
		GLsizei length	 = 0;
		GLenum type		 = 0;
		GLint   location = -1;
		GLchar* uniformName = new GLchar[ uniformMaxLength ];
		
		for (int i = 0; i < numUniforms; ++i)  
		{			
			// Get uniform main data
			glGetActiveUniform( m_programId, i, uniformMaxLength, &length, &size, &type, uniformName);
			
			// Get uniform slot location
			location = glGetUniformLocation( m_programId, uniformName );

			// Create & push GLUniform
			GLUniform uniform( uniformName, type, size, location );
			m_uniforms.insert( std::pair< std::string, GLUniform > ( uniform.Name, uniform ));			
		}
	}

	void Shader::Destroy()
	{
		// Clearing member variables
		//
		m_programId = -1;
		m_alias		= StringExtensions::EmptyString;

		// Clearing data containers
		//
		m_uniforms.clear();
	}

	void Shader::SetAsCurrentProgram()
	{
		glUseProgram( m_programId );
	}

	void Shader::Clear()
	{
		Destroy();
	}

	void Shader::InvalidateCurrentProgram()
	{
		glUseProgram( 0 );
	}

	bool Shader::GetUniform( const std::string& name, GLUniform& outUniform )
	{
		//return m_uniforms.find( name )->second;
		std::map< std::string, GLUniform >::iterator uniformIterator;
		uniformIterator = m_uniforms.find( name );
		if( uniformIterator != m_uniforms.end() )
		{
			outUniform = uniformIterator->second;
			return true;
		}
		return false;		
	}
}