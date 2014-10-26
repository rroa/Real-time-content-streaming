#pragma once

#ifndef _MATERIAL_DATA_H_
#define	_MATERIAL_DATA_H_

#include <string>

namespace Solidus
{
	struct MaterialData
	{
		float Ambient[4];
		float Diffuse[4];
		float Specular[4];
		float Shininess; 
		float Alpha;
		std::string Name;
		std::string DiffuseFilename;
		std::string BumpMapFilename;
		std::string SpecularFilename;
		std::string EmissiveFilename;
		std::string HeightMapFilename;
	};
}

#endif