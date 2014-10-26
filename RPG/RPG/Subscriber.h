#pragma once

#ifndef _SUBSCRIBER_H_
#define _SUBSCRIBER_H_

#include "NamedProperties.h"

namespace Solidus
{
	template< typename T >
	class Subscriber: public SubscriberBase
	{
		typedef void( T::*ObjMemberFuncType )( NamedProperties& args );
	public:
		Subscriber( T* obj , ObjMemberFuncType func )
			: m_subscriberObject( obj )
			, m_callbackMethod( func )
		{}

		virtual void CallRegisteredFunction( NamedProperties& eventArgs )
		{
			( m_subscriberObject->*m_callbackMethod )( eventArgs );
		}
	
		T * m_subscriberObject;
		ObjMemberFuncType m_callbackMethod;
	};
}

#endif