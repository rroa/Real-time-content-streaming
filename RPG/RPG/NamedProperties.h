#pragma once

#ifndef _NAMEDPROPERTIES_H_
#define _NAMEDPROPERTIES_H_

#include <string>
#include <map>

#include "NamedProperty.h"
#include "NamedPropertyBase.h"


namespace Solidus
{
	enum PropertyGetResult
	{
		PROPERTY_GET_SUCCESS,
		PROPERTY_GET_FAILED_NO_SUCH_PROPERTY,
		PROPERTY_GET_FAILED_WRONG_TYPE

	};

	class NamedProperties
	{
	private:
		std::map< std::string, NamedPropertyBase* > m_properties;
	public:		
		template< typename T >
		void SetProperty( const std::string& propertyName, const T& data )
		{
			auto found = m_properties.find( propertyName );
			if( found != m_properties.end() )
			{
				NamedPropertyBase* item = found->second;
				delete item;
			}
			m_properties[ propertyName ] = new NamedProperty< T >( data );
		}

		template< typename T >
		PropertyGetResult GetProperty( const std::string& propertyName, T& outData )
		{
			auto found = m_properties.find( propertyName );
			if( found != m_properties.end() )
			{
				NamedPropertyBase* npBase = found->second;
				NamedProperty< T >* typedProperty = dynamic_cast< NamedProperty< T >* >( npBase );
				if( typedProperty )
				{
					outData = typedProperty->Data;
					return PROPERTY_GET_SUCCESS;
				}
				else
				{
					return PROPERTY_GET_FAILED_WRONG_TYPE;
				}
			}
			else
			{
				return PROPERTY_GET_FAILED_NO_SUCH_PROPERTY;
			}
		}
	};
}

#endif