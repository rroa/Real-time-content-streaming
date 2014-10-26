#pragma once

#ifndef _VERTEX_H_
#define _VERTEX_H_

namespace Solidus
{
	struct Vertex
	{
		// TODO: Change this to vectors maybe?
		float position [3];
		float texCoords[2];
		float normals  [3];
		float tangent  [4];
		float bitangent[3];
		float rgba;
	};

	struct Vertex2
	{
		float x, y;
		float u, v;
		float r, g, b, a;
	};
}

#endif