#pragma once

#ifndef _TILE_TYPE_H_
#define _TILE_TYPE_H_

#include <string>

#include "Vector4.h"
#include "StringExtensions.h"

namespace Solidus
{	
	struct TileType
	{
		enum TileDamageBehavior { NONE, HURT, SHRINK, TOTAL_TILE_DAMAGE_BEHAVIOR };

		/* =============================================================
		 * ENUM TRANSFORMATIONS
		 * ============================================================= */

		static const TileDamageBehavior GetBehaviorFromString( const std::string& behavior )
		{				
			if( StringExtensions::CompareStrings( behavior, "hurt" ))
			{
				return HURT;
			}
			else if( StringExtensions::CompareStrings( behavior, "shrink" ))
			{
				return SHRINK;
			}

			return NONE;
		}

		static const std::string GetStringFromBehavior( const TileDamageBehavior& behavior )
		{
			switch(  behavior )
			{
			case HURT:
				return "HURT";
				break;
			case SHRINK:
				return "SHRINK";
				break;
			default:
				return "NONE";
			}			
		}

		/* =============================================================
		 * CTOR
		 * ============================================================= */

		TileType();
		TileType( const std::string& name, const float& health = 1500.0f );			
						
		/* =============================================================
		 * MEMBERS
		 * ============================================================= */

		bool			   IsSolid;
		float			   MaxHealth;
		Vector4			   Color;	
		std::string		   Name;
		int				   DamageSpeed;
		float			   DamageRatePerSecond;
		TileDamageBehavior DamageBehavior;
		std::string		   DoesNotWorkAgainst;
	};
}

#endif