#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "Vector3.h"
#include "Vector2.h"

bool loadOBJ(
	const char * path, 
	std::vector<Vector3> & out_vertices, 
	std::vector<Vector2> & out_uvs, 
	std::vector<Vector3> & out_normals
);

#endif