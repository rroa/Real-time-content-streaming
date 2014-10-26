#pragma once

#ifndef _PROFILEENTRY_H_
#define _PROFILEENTRY_H_

#include "ProfileData.h"
#include "ProfileHelpers.h"

#if PROFILER
namespace Solidus 
{
	struct ProfileEntry 
	{				
		/* =============================================================
		 * INLINE FUNCTIONS
		 * ============================================================= */
		
		void Init( ProfileEntry* a_parent, ProfileZone* a_zone, ProfileNodeCache* a_cache ) 
		{			
			Zone = a_zone;
			Parent = a_parent;

			EntryLevel = a_parent->EntryLevel + 1;
			a_parent->AddChild(this);

			_cache = a_cache;
		}

		void AddChild( ProfileEntry* a_child ) 
		{
			if (ChildCount++) 
			{
				LastChild->NextSibling = a_child;
				LastChild = a_child;

			} 
			else 
			{
				LastChild = a_child;
				FirstChild = a_child;
			}
		}		

		bool IsEqual( const ProfileEntry* a_parent, const ProfileZone* a_zone ) const 
		{
			return ( Parent == a_parent && Zone == a_zone );
		}

		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */

		bool				IsRoot() const { return ( EntryLevel == 0 ); }
		bool				IsDummy() const { return ( this == &_dummy ); }
		const ProfileEntry* FindNextInTree() const;
		void				UpdateTree( float a_damping );
		void				Destroy() { *_cache = &_dummy; }
		void				Clear();

		/* =============================================================
		 * MEMBERS
		 * ============================================================= */

		ProfileLastData		_last;
		ProfileZone*		Zone;
		ProfileEntry*		Parent;
		ProfileEntry*		NextSibling;
		ProfileEntry*		FirstChild;
		ProfileEntry*		LastChild;
		uint32_t			ChildCount;
		uint32_t			EntryLevel;
		ProfileNodeCache*	_cache;
		ProfileData			Data;
		static ProfileEntry _dummy;

		FORCE_INLINE void AppendTicks( tick_t a_elapsedTicks ) { _last.SelfTicks += a_elapsedTicks; }
		FORCE_INLINE void BeginEntry() { _last.EntryCount++; }

	};

}
#endif

#endif
