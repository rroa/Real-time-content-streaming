#pragma once

#ifndef _BLUEPRINT_MANAGER_H_
#define	_BLUEPRINT_MANAGER_H_

#include <string>
#include <map>

#include "BluePrintBase.h"
#include "BluePrint.h"
#include "XMLExtensions.h"
#include <typeinfo.h>

namespace Solidus
{
	enum BlueprintGetResult
	{
		BLUEPRINT_GET_SUCCESS,
		BLUEPRINT_GET_FAILED_NO_SUCH_BLUEPRINT,
		BLUEPRINT_GET_FAILED_WRONG_TYPE

	};

	class BluePrintManager
	{	
	public:		
		template< typename T >
		void CreateBluePrint( std::string name, const XMLNode& node )
		{										
			auto t = new BluePrint< T >( node );
			m_blueprints[ name ] = t;						
		}

		template< typename T >
		BlueprintGetResult GetBluePrint( const std::string& bluePrintName, T& outData )
		{
			auto found = m_blueprints.find( bluePrintName );
			if( found != m_blueprints.end() )
			{
				BluePrintBase* npBase = found->second;
				
				auto bluePrint = dynamic_cast< T >( npBase );
				if( bluePrint )
				{					
					outData = bluePrint;					
					return BLUEPRINT_GET_SUCCESS;
				}
				else
				{
					return BLUEPRINT_GET_FAILED_WRONG_TYPE;
				}
			}
			else
			{
				return BLUEPRINT_GET_FAILED_NO_SUCH_BLUEPRINT;
			}
		}

		// SINGLETON FUNCTIONS
		//
		static void Create();
		static void Destroy();	
		static BluePrintManager& Instance();
	private:
		std::map< std::string, BluePrintBase* > m_blueprints;
		
		// Setting the entity as non-copyable
		//
		BluePrintManager();
		BluePrintManager( BluePrintManager const& ) {};
		BluePrintManager& operator=( BluePrintManager const& ) {};

		// STATIC
		static BluePrintManager*	s_instance;
	};
}
#endif