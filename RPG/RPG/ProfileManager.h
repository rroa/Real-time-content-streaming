#pragma once

#ifndef _PROFILEMANAGER_H_
#define _PROFILEMANAGER_H_

#include "ProfileZone.h"
#include "ProfileEntry.h"
#include "ProfileEntryPool.h"
#include "ProfileHelpers.h"
#include "ProfileManagerOutput.h"

#include <iostream>

#if PROFILER
namespace Solidus 
{
	struct ProfileManager 
	{	
		/* =============================================================
		 * MEMBERS
		 * ============================================================= */

		enum TABLE_SIZE 
		{
			TABLE_SIZE_INIT = 256
		};

		tick_t					  LastTick;
		ProfileEntry*			  CurrentEntry;
		uint32_t				  TableMask;
		ProfileProfileEntryTable* ProfileEntryTable;

#if PROFILER_LOOKUPRATE
		uint64_t				  LookupCount;
		uint64_t				  LookupSuccessCount;
#endif

		uint32_t				  TableSize;
		uint32_t				  EntryCount;
		uint32_t				  ProfileZoneCount;

		ProfileZone*			  LastProfileZone;

		ProfileNodePool*		  LastProfileEntryPool;
		ProfileNodePool*		  FirstProfileEntryPool;

		ProfileEntry			  rootEntry;
		ProfileZone				  rootZone;

		bool					  m_initialized;
		bool					  m_firstUpdate;

		static ProfileEntry*	  DummyProfileEntryTable[];

		static ProfileManager	  Instance;

		/* =============================================================
		 * INLINE FUNCTIONS
		 * ============================================================= */

		FORCE_INLINE void AppendTicksToCurrentEntry() 
		{
			register tick_t curTick;
			GetTicks( &curTick );

			CurrentEntry->AppendTicks( curTick - LastTick );
			LastTick = curTick;
		}

		ProfileEntry* LookupEntry( ProfileNodeCache* a_cache, ProfileZone* a_zone );
		void		  CreateProfileEntryTable( uint32_t a_count );
		void		  ResizeProfileEntryTable( uint32_t a_count );					  
		void		  CreateProfileEntryPool( uint32_t a_count );
		void		  ResizeProfileEntryPool( uint32_t a_count );

		ProfileEntry* CreateProfileEntry( ProfileNodeCache* a_cache, ProfileZone* a_pZone );
		void		  InsertProfileEntry( ProfileEntry* a_pNode );

		void Init(void) 
		{
			m_initialized = true;

			rootEntry.BeginEntry();
			GetTicks( &LastTick );
		}

		void UnInit() 
		{
			m_initialized = false;

			rootEntry.Clear();
			rootEntry.Parent = &rootEntry;
			rootEntry.Zone = &rootZone;
		}

#if PROFILER_LOOKUPRATE
		FORCE_INLINE void _incLookup() { LookupCount++; }
		FORCE_INLINE void _incLookupSuccess() { LookupSuccessCount++; }
		FORCE_INLINE float lookupSuccessRate() const { return ((float) LookupSuccessCount) / ((float) LookupCount); }

#else
		FORCE_INLINE void _incLookup() {}
		FORCE_INLINE void _incLookupSuccess() {}
		FORCE_INLINE float lookupSuccessRate() const { return -1; }
#endif

		void ResetZones();
		void DestroyEntries();

		FORCE_INLINE float tableUsage() const { return ((float) EntryCount) / ((float) TableSize); }

		uint32_t staticMemInBytes() 
		{			
			return sizeof(Instance) + sizeof(DummyProfileEntryTable[0]) + sizeof(ProfileEntry::_dummy)
				 + (ProfileZoneCount - 1) * (sizeof(ProfileZone) + sizeof(ProfileNodeCache));
		}

		uint32_t allocMemInBytes() 
		{
			return TableSize * sizeof( ProfileEntry*  )
				 + ( FirstProfileEntryPool ) ? FirstProfileEntryPool->MemoryUsageChain() : 0;
		}

		FORCE_INLINE void BeginEntry(ProfileNodeCache* a_cache, ProfileZone* a_zone) 
		{
			if (CurrentEntry != (*a_cache)->Parent)
				*a_cache = LookupEntry(a_cache, a_zone);

			BeginEntry(*a_cache);
		}

		FORCE_INLINE void BeginEntry(ProfileEntry* a_node) 
		{
			a_node->BeginEntry();
			AppendTicksToCurrentEntry();
			CurrentEntry = a_node;
		}

		FORCE_INLINE void EndCurrentEntry() 
		{
			AppendTicksToCurrentEntry();
			CurrentEntry = CurrentEntry->Parent;
		}

		//

		void preLoad();

		void updateClean();
		void update(float a_damping = 0.9f);

		void clear();
		void destroy();

		bool output(const char *a_filename);
		bool output(std::ostream &a_ostream = std::cout);

		FORCE_INLINE std::string outputNodesAsString() { return OutputNodesAsString(&rootEntry, EntryCount); }
		FORCE_INLINE std::string outputZonesAsString() { return OutputZonesAsString(&rootZone, ProfileZoneCount); }

		//

		static void enumerateNodes(void (*a_func)(const ProfileEntry*),
			const ProfileEntry* a_node = &Instance.rootEntry)
		{
			a_func(a_node);

			if (a_node->FirstChild) enumerateNodes(a_func, a_node->FirstChild);
			if (a_node->NextSibling) enumerateNodes(a_func, a_node->NextSibling);
		}

		template <class T>
		static void enumerateNodes(T* a_this, void (T::*a_func)(const ProfileEntry*),
			const ProfileEntry* a_node = &Instance.rootEntry)
		{
			(a_this->*a_func)(a_node);

			if (a_node->FirstChild) enumerateNodes(a_this, a_func, a_node->FirstChild);
			if (a_node->NextSibling) enumerateNodes(a_this, a_func, a_node->NextSibling);
		}

		static void enumerateZones(void (*a_func)(const ProfileZone*),
			const ProfileZone* a_zone = &Instance.rootZone)
		{
			a_func(a_zone);

			if (a_zone->next) enumerateZones(a_func, a_zone->next);
		}

		template <class T>
		static void enumerateZones(T* a_this, void (T::*a_func)(const ProfileZone*),
			const ProfileZone* a_zone = &Instance.rootZone)
		{
			(a_this->*a_func)(a_zone);

			if (a_zone->next) enumerateZones(a_this, a_func, a_zone->next);
		}
	};


//-----------------------------------------------------------------------------

	class ProfileAutoEndEntry 
	{
	public:

		FORCE_INLINE ~ProfileAutoEndEntry() 
		{
			ProfileManager::Instance.EndCurrentEntry();
		}
	};

} // namespace Solidus

#endif

#endif
