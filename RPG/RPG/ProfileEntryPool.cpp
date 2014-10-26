#include "stdafx.h"
#include "ProfileEntryPool.h"
#include "ProfileHelpers.h"

#include <memory.h>
#include <malloc.h>

#if PROFILER
namespace Solidus 
{
	ProfileNodePool* ProfileNodePool::CreateNodePool( uint32_t a_items ) 
	{
		ProfileNodePool* pPool = static_cast<ProfileNodePool*>(
			malloc( sizeof( ProfileNodePool ) + sizeof( T ) * ( a_items - 1 )));

		pPool->nextPool = NULL;
		pPool->_nextItem = &pPool->_items[ 0 ];
		pPool->endOfItems = &pPool->_items[ a_items ];

		memset( &pPool->_items[ 0 ], 0, a_items * sizeof( T ));
		return pPool;
	}

	uint32_t ProfileNodePool::MemoryUsageChain() 
	{
		uint32_t bytes = ptr32(
			reinterpret_cast<void*>(
				  reinterpret_cast<char*>( endOfItems )
				- reinterpret_cast<char*>( this )));

		if ( nextPool ) bytes += nextPool->MemoryUsageChain();
		return bytes;
	}

	void ProfileNodePool::Destroy() 
	{
		T* pItem = FirstItem();

		while ( pItem != UnusedItem() )
			( pItem++ )->Destroy();

		if ( nextPool ) nextPool->Destroy();
		free( this );
	}

}
#endif
