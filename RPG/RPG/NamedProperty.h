#pragma once

#ifndef _NAMEDPROPERTY_H_
#define _NAMEDPROPERTY_H_

#include "NamedPropertyBase.h"

namespace Solidus
{
	template< typename T >
	class NamedProperty : public NamedPropertyBase
	{
	public:	
		T Data;
		NamedProperty( const T& data )
			: Data( data )
		{}

		virtual NamedPropertyBase* Clone() const
		{
			return new NamedProperty< T >( Data );
		}	
	};
}

#endif