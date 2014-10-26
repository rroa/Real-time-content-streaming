#pragma once

#ifndef _XMLEXTENSIONS_H_
#define _XMLEXTENSIONS_H_

#include <string>

#include "pugixml.hpp"
#include "Vector2.h"
#include "Vector4.h"
#include "ReportErrors.h"

namespace Solidus
{
	/* =============================================================
	 * TYPEDEFS
	 * ============================================================= */
	typedef pugi::xml_node XMLNode;

	/* =============================================================
	 * PROTOTYPES
	 * ============================================================= */
	int				   GetIntXMLAttribute( XMLNode xmlNode, std::string attributeName, const int& defaultValue );
	bool			   GetBoolXMLAttribute( XMLNode xmlNode, std::string attributeName, const bool& defaultValue );
	float			   GetFloatXMLAttribute( XMLNode xmlNode, std::string attributeName, const float& defaultValue );
	std::string		   GetStringXMLAttribute( XMLNode xmlNode, std::string attributeName, const std::string& defaultValue = "" );
	Vector2			   GetVector2XMLAttribute( XMLNode xmlNode, std::string attributeName, const Vector2& defaultValue );
	const unsigned int GetRGBAXMLAttribute( XMLNode xmlNode, std::string attributeName, const unsigned int& defaultValue );
	const Vector4	   GetRGBAXMLAttribute( XMLNode xmlNode, std::string attributeName, const Vector4& defaultValue );
	bool			   ValidateXMLAttributes( XMLNode xmlNode, std::string requiredAttributes, std::string optionalAttributes = "" );
	bool			   ValidateXMLChildElements( XMLNode xmlNode, std::string requiredChildElements, std::string optionalChildElements = "" );	
}

#endif