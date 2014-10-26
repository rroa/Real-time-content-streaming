#pragma once

#ifndef _AI_H_
#define _AI_H_

#include <map>
#include <vector>

#include "NamedProperties.h"
#include "StringExtensions.h"
#include "Vector2.h"

namespace Solidus
{
	enum BehaviorType
	{ 
		NO_BEHAVIOR	 = 0,
		FEAR		 = 1,
		INDIFFERENCE = 2,
		HATE		 = 4
	};
	
	class Entity;
	class AI
	{		
	public:
		/* =============================================================
		 * CTOR
		 * ============================================================= */

		AI( const BehaviorType& behavior )
			: m_behavior( behavior )
		{}

		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */

		virtual void FindTarget( NamedProperties& eventArgs ) = 0;

		/* =============================================================
		 * INLINE
		 * ============================================================= */

		inline const BehaviorType Behavior() const { return m_behavior; }

		/* =============================================================
		 * STATIC
		 * ============================================================= */

		static const BehaviorType GetBehaviorFromString( const std::string& behavior )
		{				
			if( StringExtensions::CompareStrings( behavior, "fear" ))
			{
				return FEAR;
			}
			else if( StringExtensions::CompareStrings( behavior, "indifference" ))
			{
				return INDIFFERENCE;
			}
			else if( StringExtensions::CompareStrings( behavior, "hate" ))
			{
				return HATE;
			}

			return NO_BEHAVIOR;
		}

		static const std::string GetStringFromBehavior( const BehaviorType& behavior )
		{
			switch(  behavior )
			{
			case FEAR:
				return "FEAR";
				break;
			case INDIFFERENCE:
				return "INDIFFERENCE";
				break;
			case HATE:
				return "HATE";
				break;
			default:
				return "NONE";
			}			
		}
	protected:
		/* =============================================================
		 * MEMBERS
		 * ============================================================= */

		BehaviorType		   m_behavior;
		std::vector< Vector2 > m_walkPath;
	};
}

#endif