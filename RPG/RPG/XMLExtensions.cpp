#include "stdafx.h"
#include "XMLExtensions.h"
#include "StringExtensions.h"
#include "Utilities.h"

#include <algorithm>
#include <Windows.h>

namespace Solidus
{
	int GetIntXMLAttribute( XMLNode xmlNode, std::string attributeName, const int& defaultValue )
	{
		pugi::xml_attribute attr = xmlNode.attribute( attributeName.c_str() );
		if( attr.empty() )
		{
			return defaultValue;
		}

		int attributeValue = attr.as_int();

		return attributeValue;
	}

	bool GetBoolXMLAttribute( XMLNode xmlNode, std::string attributeName, const bool& defaultValue )
	{
		pugi::xml_attribute attr = xmlNode.attribute( attributeName.c_str() );
		if( attr.empty() )
		{
			return defaultValue;
		}

		bool attributeValue = attr.as_bool();

		return attributeValue;
	}

	float GetFloatXMLAttribute( XMLNode xmlNode, std::string attributeName, const float& defaultValue )
	{
		pugi::xml_attribute attr = xmlNode.attribute( attributeName.c_str() );
		if( attr.empty() )
		{
			return defaultValue;
		}

		float attributeValue = attr.as_float();

		return attributeValue;
	}

	std::string GetStringXMLAttribute( XMLNode xmlNode, std::string attributeName, const std::string& defaultValue )
	{
		pugi::xml_attribute attr = xmlNode.attribute( attributeName.c_str() );
		if( attr.empty() )
		{
			return defaultValue;
		}

		return attr.as_string();;
	}

	Vector2 GetVector2XMLAttribute( XMLNode xmlNode, std::string attributeName, const Vector2& defaultValue )
	{
		pugi::xml_attribute attr = xmlNode.attribute( attributeName.c_str() );
		if( attr.empty() )
		{
			return defaultValue;
		}

		std::string attributeValue = attr.as_string();
		std::vector< std::string > splittedValues = Solidus::StringExtensions::Split( attributeValue.c_str(), "," );
		if( splittedValues.size() != 2 )
		{
			DEBUG( "Setting the default value for: " << xmlNode.name() << " attribute:  " << attributeName );
			return defaultValue;
		}

		float x = static_cast< float >( std::atof( splittedValues[0].c_str()));
		float y = static_cast< float >( std::atof( splittedValues[1].c_str()));

		return Vector2( x, y );
	}

	const unsigned int GetRGBAXMLAttribute( XMLNode xmlNode, std::string attributeName, const unsigned int& defaultValue )
	{
		pugi::xml_attribute attr = xmlNode.attribute( attributeName.c_str() );
		if( attr.empty() )
		{
			return defaultValue;
		}

		std::string attributeValue = attr.as_string();
		std::vector< std::string > splittedValues = Solidus::StringExtensions::Split( attributeValue.c_str(), "," );
		if( splittedValues.size() >= 3 && splittedValues.size() <= 4 )
		{
			float red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 1.0f;
			// RGB format
			if( splittedValues.size() == 3 )
			{			
				red	  = static_cast< float >( std::atof( splittedValues[ 0 ].c_str() ));
				green = static_cast< float >( std::atof( splittedValues[ 1 ].c_str() ));
				blue  = static_cast< float >( std::atof( splittedValues[ 2 ].c_str() ));
				return ToRGBA( red, green, blue, alpha );
			}

			// RGBA format
			red	  = static_cast< float >( std::atof( splittedValues[ 0 ].c_str()));
			green = static_cast< float >( std::atof( splittedValues[ 1 ].c_str()));
			blue  = static_cast< float >( std::atof( splittedValues[ 2 ].c_str()));
			alpha = static_cast< float >( std::atof( splittedValues[ 3 ].c_str()));
			return ToRGBA( red, green, blue, alpha );
		}

		DEBUG( "Setting the default value for: " << xmlNode.name() << " attribute:  " << attributeName );
		return defaultValue;
	}

	const Vector4 GetRGBAXMLAttribute( XMLNode xmlNode, std::string attributeName, const Vector4& defaultValue )
	{
		pugi::xml_attribute attr = xmlNode.attribute( attributeName.c_str() );
		if( attr.empty() )
		{
			return defaultValue;
		}

		std::string attributeValue = attr.as_string();
		std::vector< std::string > splittedValues = Solidus::StringExtensions::Split( attributeValue.c_str(), "," );
		if( splittedValues.size() >= 3 && splittedValues.size() <= 4 )
		{
			float red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 1.0f;
			// RGB format
			if( splittedValues.size() == 3 )
			{			
				red	  = static_cast< float >( std::atof( splittedValues[ 0 ].c_str() ));
				green = static_cast< float >( std::atof( splittedValues[ 1 ].c_str() ));
				blue  = static_cast< float >( std::atof( splittedValues[ 2 ].c_str() ));
				return Vector4( red, green, blue, alpha );
			}

			// RGBA format
			red	  = static_cast< float >( std::atof( splittedValues[ 0 ].c_str()));
			green = static_cast< float >( std::atof( splittedValues[ 1 ].c_str()));
			blue  = static_cast< float >( std::atof( splittedValues[ 2 ].c_str()));
			alpha = static_cast< float >( std::atof( splittedValues[ 3 ].c_str()));
			return Vector4( red, green, blue, alpha );
		}

		DEBUG( "Setting the default value for: " << xmlNode.name() << " attribute:  " << attributeName );
		return defaultValue;
	}

	bool ValidateXMLAttributes( XMLNode xmlNode, std::string requiredAttributes, std::string optionalAttributes )
	{
		std::vector< std::string > splittedRequiredAttributes = Solidus::StringExtensions::Split( requiredAttributes, ",", true );
		std::vector< std::string > splittedOptionalAttributes = Solidus::StringExtensions::Split( optionalAttributes, ",", true );

		std::vector< std::string > attributes;

		for( auto attr = xmlNode.attributes_begin(); attr != xmlNode.attributes_end(); ++attr )
		{
			attributes.push_back( Solidus::StringExtensions::ToLower( attr->name() ));
		}

		// Checking for required attributes
		//
		for( auto requiredAttribute = splittedRequiredAttributes.begin(); requiredAttribute != splittedRequiredAttributes.end(); ++requiredAttribute )
		{
			auto found = std::find( attributes.begin(), attributes.end(), *requiredAttribute );
			if ( found != attributes.end() )
			{
				// found it
			} 
			else 
			{
				DEBUG( "Could not find the required attribute = " << *requiredAttribute );			
				return false;
			}
		}

		// Checking for optional attributes
		//
		for( auto optionalAttribute = splittedOptionalAttributes.begin(); optionalAttribute != splittedOptionalAttributes.end(); ++optionalAttribute )
		{
			auto found = std::find( attributes.begin(), attributes.end(), *optionalAttribute );
			if ( found != attributes.end() )
			{
				// found it
			} 
			else 
			{
				DEBUG( "Could not find the optional attribute = " << *optionalAttribute );
				return false;
			}
		}

		return true;
	}

	bool ValidateXMLChildElements( XMLNode xmlNode, std::string requiredChildElements, std::string optionalChildElements )
	{
		std::vector< std::string > splittedRequiredChildElements = Solidus::StringExtensions::Split( requiredChildElements, ",", true );
		std::vector< std::string > splittedOptionalChildElements = Solidus::StringExtensions::Split( optionalChildElements, ",", true );

		std::vector< std::string > children;

		for( auto child = xmlNode.begin(); child != xmlNode.end(); ++child )
		{
			children.push_back( Solidus::StringExtensions::ToLower( child->name() ));
		}

		// Checking for required attributes
		//
		for( auto requiredChild = splittedRequiredChildElements.begin(); requiredChild != splittedRequiredChildElements.end(); ++requiredChild )
		{
			auto found = std::find( children.begin(), children.end(), *requiredChild );
			if ( found != children.end() )
			{
				// found it
			} 
			else 
			{
				DEBUG( "Could not find the required child = " << *requiredChild );
				return false;
			}
		}

		// Checking for optional attributes
		//
		for( auto optionalChild = splittedOptionalChildElements.begin(); optionalChild != splittedOptionalChildElements.end(); ++optionalChild )
		{
			auto found = std::find( children.begin(), children.end(), *optionalChild );
			if ( found != children.end() )
			{
				// found it
			} 
			else 
			{
				DEBUG( "Could not find the optional child = " << *optionalChild );
				return false;
			}
		}
		return true;
	}
}