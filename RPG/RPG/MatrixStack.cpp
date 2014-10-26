#include "stdafx.h"
#include "MatrixStack.h"

namespace Solidus
{
	MatrixStack::MatrixStack()
	{
		m_stack.push_back( Matrix44::MakeIdentity() );
	}

	MatrixStack::~MatrixStack()
	{
		m_stack.clear();
	}

	void MatrixStack::PushMatrix()
	{
		const int index = m_stack.size() - 1;
		m_stack.push_back( m_stack[ index ] );
	}

	void MatrixStack::PopMatrix()
	{
		m_stack.pop_back();
		if( m_stack.empty() )
		{
			m_stack.push_back( Matrix44::MakeIdentity() );
		}
	}

	void MatrixStack::LoadIdentity()
	{
		//const int index = m_stack.size() - 1;
		m_stack.push_back( Matrix44::MakeIdentity() );
	}

	void MatrixStack::Rotate( float angle, float x, float y, float z )
	{
		const int index = m_stack.size() - 1;
		Matrix44 old = m_stack[ index ];
		Vector3 rotationVector( x, y, z );
		m_stack[ index ] = Matrix44::Rotate( old, angle, rotationVector );
	}

	void MatrixStack::Translate( float x, float y, float z )
	{
		const int index = m_stack.size() - 1;
		Matrix44 old = m_stack[ index ];
		Vector3 translationVector( x, y, z );
		m_stack[ index ] = Matrix44::Translate( old, translationVector );
	}

}