#include "stdafx.h"
#include "ProfileManager.h"

#include <fstream>
#include <memory.h>
#include <stdio.h>

#if PROFILER
namespace Solidus 
{


//-----------------------------------------------------------------------------

	ProfileManager ProfileManager::Instance = 
	{
		/* _lastTick = */ 0,
		 &Instance.rootEntry,
		/* _tableMask = */ 0,
		/* _ProfileEntryTable = */ ProfileManager::DummyProfileEntryTable,
#if PROFILER_LOOKUPRATE
		/* _lookupCount = */ 0,
		/* _lookupSuccessCount = */ 0,
#endif
		 1,
		1,
		1,
		&Instance.rootZone,
		NULL,
		/* _firstNodePool = */ NULL,
		/* rootNode = */ {
			/* _last = */ { 0, 0 },
			/* zone = */ &Instance.rootZone,
			/* parent = */ &Instance.rootEntry,
			/* nextSibling = */ NULL,
			/* firstChild = */ NULL,
			/* lastChild = */ NULL,
			/* childCount = */ 0,
			/* entryLevel = */ 0,
			/* _cache = */ NULL,
			/* data = */ { { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		/* rootZone = */ {
			/* next = */ NULL,
			/* _state = */ ProfileZone::STATE_HIDDEN,
			/* name = */ "<root>",
			/* data = */ { { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		/* _initialized = */ false,
		/* _firstUpdate = */ true
	};

	ProfileEntry* ProfileManager::DummyProfileEntryTable[] = { NULL };


//-----------------------------------------------------------------------------
	// primary hash function
	FORCE_INLINE uint32_t hash_value(ProfileEntry* a_pParent, ProfileZone* a_pZone) 
	{
		uint32_t a = ptr32(a_pParent) + ptr32(a_pZone);

		a = (a+0x7ed55d16) + (a<<12);
		a = (a^0xc761c23c) ^ (a>>19);
		return a;
	}

	// secondary hash used as index offset: force it to be odd
	// so it's relatively prime to the power-of-two table size
	FORCE_INLINE uint32_t hash_offset(uint32_t a) 
	{
		return ((a << 8) + (a >> 4)) | 1;
	}


//-----------------------------------------------------------------------------

	void ProfileManager::preLoad(void) 
	{
		if (!m_initialized) {
			Init();

			CreateProfileEntryTable(TABLE_SIZE_INIT);
			CreateProfileEntryPool(TABLE_SIZE_INIT / 2);
		}
	}


//-----------------------------------------------------------------------------

	void ProfileManager::update(float a_damping) 
	{
		AppendTicksToCurrentEntry();

		if (!m_firstUpdate) {
			rootZone.PreUpdateChain();
			rootEntry.UpdateTree(a_damping);
			rootZone.UpdateChain(a_damping);

		} else {
			m_firstUpdate = false;
			rootZone.PreUpdateChain();
			rootEntry.UpdateTree(0);
			rootZone.UpdateChain(0);
		}
	}


//-----------------------------------------------------------------------------

	void ProfileManager::clear(void)
	{
		destroy();
		preLoad();
	}


//-----------------------------------------------------------------------------

	void ProfileManager::destroy(void) 
	{
		ResetZones();
		DestroyEntries();
		UnInit();
	}


//-----------------------------------------------------------------------------

	ProfileEntry* ProfileManager::LookupEntry(ProfileNodeCache* a_cache, ProfileZone* a_zone) 
	{
		uint32_t nHash = hash_value(CurrentEntry, a_zone);
		uint32_t nIndex = nHash & TableMask;
		ProfileEntry* pNode = ProfileEntryTable[nIndex];

		_incLookup();
		_incLookupSuccess();

		if (pNode) {
			if (pNode->IsEqual(CurrentEntry, a_zone)) return pNode; // found it!
			
			// hash collision:

			// compute a secondary hash function for stepping
			uint32_t nStep = hash_offset(nHash);

			for (;;) {
				_incLookup();

				nIndex = (nIndex + nStep) & TableMask;
				pNode = ProfileEntryTable[nIndex];

				if (!pNode) break;
				else if (pNode->IsEqual(CurrentEntry, a_zone)) return pNode;
			}

			// loop is guaranteed to end because the hash table is never full
		}

		if (!a_zone->IsInitialized()) { // zone is not initialized
			a_zone->Init(LastProfileZone);

			LastProfileZone = a_zone;
			ProfileZoneCount++;

			if (m_initialized == false) { // first time init
				Init();

				CreateProfileEntryTable(TABLE_SIZE_INIT);
				CreateProfileEntryPool(TABLE_SIZE_INIT / 2);

				// initialization has invalidated nIndex
				// we must compute nIndex again
				return CreateProfileEntry(a_cache, a_zone);
			}
		}

		// YES nodeCount is not updated
		// but it includes rootNode so it adds up.

		// check if we need to grow the table
		// we keep it at most 1/2 full to be very fast
		if (TableSize < 2 * EntryCount) {

			ResizeProfileEntryTable(2 * TableSize);
			ResizeProfileEntryPool(EntryCount - 1);

			// expansion has invalidated nIndex
			// we must compute nIndex again
			return CreateProfileEntry(a_cache, a_zone);
		}
		
		EntryCount++;

		ProfileEntry* pNewNode = LastProfileEntryPool->NewItem();
		pNewNode->Init(CurrentEntry, a_zone, a_cache);

		ProfileEntryTable[nIndex] = pNewNode;
		return pNewNode;
	}


//-----------------------------------------------------------------------------

	ProfileEntry* ProfileManager::CreateProfileEntry(ProfileNodeCache* a_cache, ProfileZone* a_pZone) 
	{
		ProfileEntry* pNewNode = LastProfileEntryPool->NewItem();
		pNewNode->Init(CurrentEntry, a_pZone, a_cache);

		EntryCount++;
		InsertProfileEntry(pNewNode);
		return pNewNode;
	}


//-----------------------------------------------------------------------------

	void ProfileManager::InsertProfileEntry(ProfileEntry* a_pNode) 
	{
		uint32_t nHash = hash_value(a_pNode->Parent, a_pNode->Zone);
		uint32_t nIndex = nHash & TableMask;

		if (ProfileEntryTable[nIndex]) {
			uint32_t nStep = hash_offset(nHash);

			while (ProfileEntryTable[nIndex])
				nIndex = (nIndex + nStep) & TableMask;
		}

		ProfileEntryTable[nIndex] = a_pNode;
	}


//-----------------------------------------------------------------------------

	void ProfileManager::CreateProfileEntryPool(uint32_t a_nCount) 
	{
		FirstProfileEntryPool = ProfileNodePool::CreateNodePool(a_nCount);
		LastProfileEntryPool = FirstProfileEntryPool;
	}


//-----------------------------------------------------------------------------

	void ProfileManager::ResizeProfileEntryPool(uint32_t a_nCount) 
	{
		ProfileNodePool* pPool = ProfileNodePool::CreateNodePool(a_nCount);
		LastProfileEntryPool->nextPool = pPool;
		LastProfileEntryPool = pPool;
	}


//-----------------------------------------------------------------------------

	void ProfileManager::CreateProfileEntryTable(uint32_t a_nCount) 
	{
		TableSize = a_nCount;
		TableMask = a_nCount - 1;

		ProfileEntryTable = static_cast<ProfileProfileEntryTable*>(
			malloc(sizeof(ProfileEntry) * a_nCount));

		memset(ProfileEntryTable, 0, a_nCount * sizeof(ProfileEntry*));
	}


//-----------------------------------------------------------------------------

	void ProfileManager::ResizeProfileEntryTable(uint32_t a_nCount) 
	{
		ProfileNodePool* pPool;

		free(ProfileEntryTable);
		CreateProfileEntryTable(a_nCount);

		pPool = FirstProfileEntryPool;
		while (pPool) {

			ProfileEntry *pIter = pPool->FirstItem();

			while (pIter != pPool->UnusedItem())
				InsertProfileEntry(pIter++);

			pPool = pPool->nextPool;
		}
	}


//-----------------------------------------------------------------------------

	void ProfileManager::ResetZones(void) 
	{
		ProfileZone *pZone, *pNextZone;

		pZone = &rootZone;

		for(;;) {
			pZone->UnInit();

			pNextZone = pZone->next;
			pZone->next = NULL;
			
			if (!pNextZone) break;
			pZone = pNextZone;
		}

		LastProfileZone = &rootZone;
		ProfileZoneCount = 1;
	}


//-----------------------------------------------------------------------------

	void ProfileManager::DestroyEntries(void) 
	{
		if (FirstProfileEntryPool) {
			FirstProfileEntryPool->Destroy();
			FirstProfileEntryPool = NULL;
		}

		if (ProfileEntryTable != Instance.DummyProfileEntryTable) 
		{
			free(ProfileEntryTable);

			ProfileEntryTable = Instance.DummyProfileEntryTable;
			TableSize = 1;
			TableMask = 0;
		}

		CurrentEntry = &rootEntry;
		EntryCount = 1;

		Init();
	}


//-----------------------------------------------------------------------------

	bool ProfileManager::output(const char *a_filename) 
	{
		std::ofstream file(a_filename, std::ios_base::out);

		if (!file.is_open()) return false;
		else return output(file);
	}


//-----------------------------------------------------------------------------

	bool ProfileManager::output(std::ostream &a_ostream) 
	{
		a_ostream << outputZonesAsString().c_str()
				  << "\n\n"
				  << outputNodesAsString().c_str()
				  << "\n\n"
				  << std::flush;

		return true;
	}


}

#endif
