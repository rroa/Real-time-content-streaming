#pragma once

#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <vector>
#include "Vertex.h"

namespace Solidus
{
	class Shape
	{
	public:
		Shape();
		~Shape();
	protected:
		GLuint	  			  m_idVBO;
		std::vector< Vertex > m_vertices;
		std::vector< int >    m_indices;
	};
}

#endif