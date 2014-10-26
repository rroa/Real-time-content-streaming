#pragma once

#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include <string>

namespace Solidus
{
	class ResourceManager
	{
	public:
		//SHADERS
		static const std::string TextShader;
		static const std::string StandardShader;
		static const std::string NormalMappingShader;
		static const std::string SpecularShader;
		static const std::string BasicShader;
		static const std::string ToonShader;
	};
}

#endif