#pragma once

#ifndef _EVENTSYSTEM_H_
#define _EVENTSYSTEM_H_

#include <map>

#include "Subscriber.h"
#include "NamedProperties.h"

#include <vector>
#include <string>
#include "SubscriberBase.h"


namespace Solidus
{
	class EventSystem
	{
	private:
		std::map< std::string, std::vector< SubscriberBase* >> m_registry;
	public:
		EventSystem(){}


		template< typename T, typename ObjMemberFuncType >
		void Register( T* object, ObjMemberFuncType func, const std::string& eventName )
		{
			if( m_registry.size() > 0 )
			{
				auto found = m_registry.find( eventName );
				if( found != m_registry.end() )
				{
					std::vector< SubscriberBase* >& recipients = found->second;
					Subscriber< T > * subscriberToAdd = new Subscriber< T >( object, func );					
					recipients.push_back( subscriberToAdd );
				}
				else
				{
					std::vector< SubscriberBase* > recipients;
					Subscriber< T > * subscriberToAdd = new Subscriber< T >( object, func );
					recipients.push_back( subscriberToAdd );
					m_registry[ eventName ] = recipients;
				}
			}
			else
			{
				std::vector< SubscriberBase* > recipients;
				Subscriber< T > * s = new Subscriber< T >( object, func );
				recipients.push_back( s );
				m_registry[ eventName ] = recipients;
			}
		}

		template< typename T, typename ObjMemberFuncType >
		void UnRegister( T* object, ObjMemberFuncType func, const std::string& eventName )
		{
			std::map< std::string, std::vector< SubscriberBase* > >::iterator found = m_registry.find( eventName );

			if( found != m_registry.end() )
			{
				std::vector< SubscriberBase* >& recipients = found->second;

				std::vector< SubscriberBase* >::iterator currentRecipient;
				for( currentRecipient = recipients.begin(); currentRecipient != recipients.end(); ++currentRecipient )
				{
					Subscriber< T >*currentSubscriber = dynamic_cast< Subscriber< T >* >( *currentRecipient );
					if( currentSubscriber )
					{
						if( currentSubscriber->m_subscriberObject == object && currentSubscriber->m_callbackMethod == func )
						{
							break;
						}
					}
				}

				if( currentRecipient != recipients.end() )
				{
					recipients.erase( currentRecipient );
				}
			}
		}


		void FireEvent( const std::string& eventName, NamedProperties& eventArgs )
		{
			auto found = m_registry.find( eventName );
			if( found != m_registry.end() )
			{
				auto recipients = found->second;
				for( auto iter = recipients.begin(); iter != recipients.end(); ++iter )
				{
					SubscriberBase* subs = *iter;
					subs->CallRegisteredFunction( eventArgs );					
				}
			}
		}
	};
}

extern Solidus::EventSystem g_EventManager;

#endif