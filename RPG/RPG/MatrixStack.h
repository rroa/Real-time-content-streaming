#pragma once

#ifndef _MATRIX_STACK_H_
#define _MATRIX_STACK_H_

#include <vector>
#include "Matrix44.h"

namespace Solidus
{
	class MatrixStack
	{
	public:
		MatrixStack();
		~MatrixStack();
		void PushMatrix();// copy the last element and push it
		void PopMatrix(); //if there's two or more, pop if not set the first element to identity
		void Rotate( float angle, float x, float y, float z );		
		void Translate( float x, float y, float z );		
		void LoadIdentity();
		void GetCurrentMatrix();
	private:
		std::vector< Matrix44 > m_stack;
	};
}

#endif