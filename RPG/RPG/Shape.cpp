#include "stdafx.h"
#include "Shape.h"

namespace Solidus
{
	Shape::Shape()
		: m_idVBO( 0 )
	{}

	Shape::~Shape()
	{
		m_vertices.clear();
		m_indices.clear();
	}
}