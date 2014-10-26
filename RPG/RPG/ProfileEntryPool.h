#pragma once

#ifndef _PROFILEENTRYPOOL_H_
#define _PROFILEENTRYPOOL_H_

#include "ProfileEntry.h"

#if PROFILER
namespace Solidus 
{
	struct ProfileNodePool 
	{
		typedef ProfileEntry T;

		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */

		T* FirstItem() { return &_items[0]; }
		T* NewItem() { return _nextItem++; }
		const T* UnusedItem() const { return _nextItem; }

		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */

		static ProfileNodePool* CreateNodePool( uint32_t a_items );
		uint32_t				MemoryUsageChain();
		void					Destroy();

		/* =============================================================
		 * MEMBERS
		 * ============================================================= */
		
		ProfileNodePool* nextPool;
		T*				 _nextItem;
		T*				 endOfItems;
		T				 _items[1];
	};
}
#endif

#endif
