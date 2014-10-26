#pragma once

#ifndef _PROFILERMACROS_H_
#define _PROFILERMACROS_H_

#include "ProfileManager.h"

#if PROFILER
//-----------------------------------------------------------------------------
// public preprocessors

#define PROFILER_UPDATE														\
	Solidus::ProfileManager::Instance.update

#define PROFILER_OUTPUT														\
	Solidus::ProfileManager::Instance.output

#define PROFILER_OUTPUT_TREE_STRING()										\
	Solidus::ProfileManager::Instance.outputNodesAsString()

#define PROFILER_OUTPUT_FLAT_STRING()										\
	Solidus::ProfileManager::Instance.outputZonesAsString()

#define PROFILER_DESTROY()													\
	Solidus::ProfileManager::Instance.destroy()


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_ROOT_DATA()													\
	Solidus::ProfileManager::Instance.rootZone.data


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_END()														\
	Solidus::ProfileManager::Instance._endCurNode()


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_BEGIN( name )												\
																			\
	static _PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE(name), #name);				\
	_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE(name));


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_BLOCK( name )												\
																			\
	_PROFILE_BLOCK_DEFINE(_PROFILE_ID_BLOCK());								\
	PROFILE_BEGIN(name);


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_FUNC()														\
																			\
	_PROFILE_BLOCK_DEFINE(_PROFILE_ID_BLOCK());								\
	static _PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_FUNC(), __FUNCTION__);		\
	_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_FUNC());


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_CODE( code )												\
	{																		\
		static _PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_CODE(), #code);		\
		_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_CODE());						\
		{ code; }															\
		PROFILE_END();														\
	}


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_EXTERN( name )										\
																			\
	_PROFILE_ZONE_DECLARE(extern, _PROFILE_ID_ZONE_SHARED(name));


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_STATIC( name )										\
																			\
	_PROFILE_ZONE_DECLARE(static, _PROFILE_ID_ZONE_SHARED(name));


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_DEFINE( name )										\
																			\
	_PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_SHARED(name), #name);


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_BEGIN( name )										\
																			\
	_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_SHARED(name));


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_BLOCK( name )										\
																			\
	_PROFILE_BLOCK_DEFINE(_PROFILE_ID_BLOCK());								\
	_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_SHARED(name));


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_DATA( name )											\
																			\
	_PROFILE_ID_ZONE_SHARED(name).data										\


//-----------------------------------------------------------------------------
// internal preprocessors

#define _PROFILE_ID_ZONE( name )			__ProfileZone_##name
#define _PROFILE_ID_ZONE_FUNC()				__ProfileZoneFunc
#define _PROFILE_ID_ZONE_CODE()				__ProfileZoneCode
#define _PROFILE_ID_ZONE_SHARED( name )		name##__ProfileZoneShared
#define _PROFILE_ID_BLOCK()					__ProfileBlock


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_ZONE_DEFINE( id, string )									\
																			\
	Solidus::ProfileZone id = {												\
		NULL, Solidus::ProfileZone::STATE_HIDDEN, string,						\
		{ { 0, 0 }, { 0, 0 }, { 0, 0 } }									\
	};


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_ZONE_DECLARE( prefix, id )									\
																			\
	prefix Solidus::ProfileZone id;


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_BLOCK_DEFINE( id )											\
																			\
	Solidus::ProfileAutoEndEntry PROFILE_UNUSED id;


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_ZONE_BEGIN( id )											\
	{																		\
		static Solidus::ProfileNodeCache cache =								\
			&Solidus::ProfileEntry::_dummy;									\
																			\
		Solidus::ProfileManager::Instance.BeginEntry(&cache, &id);			\
	}

//-----------------------------------------------------------------------------

#else

namespace Solidus {

	FORCE_INLINE ProfileData GetEmptyData() {
		ProfileData a = { { 0, 0 }, { 0, 0 }, { 0, 0 } };
		return a;
	}
	
	FORCE_INLINE void DummyUpdate(float a = 0.0f) { /* meditate */ }
	FORCE_INLINE bool DummyOutput(const char *a = NULL) { return false; }
	FORCE_INLINE bool DummyOutput(std::ostream &a) { return false; }
	
}

#define PROFILER_UPDATE					DummyUpdate
#define PROFILER_OUTPUT					DummyOutput
#define PROFILER_OUTPUT_TREE_STRING()	std::string()
#define PROFILER_OUTPUT_FLAT_STRING()	std::string()
#define PROFILER_DESTROY()
#define PROFILE_BEGIN(name)
#define PROFILE_BLOCK(name)
#define PROFILE_FUNC()
#define PROFILE_CODE(code)				{ code; }
#define PROFILE_SHARED_EXTERN(name)
#define PROFILE_SHARED_STATIC(name)
#define PROFILE_SHARED_DEFINE(name)
#define PROFILE_SHARED_BEGIN(name)
#define PROFILE_SHARED_BLOCK(name)
#define PROFILE_SHARED_DATA(name)		Solidus::GetEmptyData()
#define PROFILE_ROOT_DATA()				Solidus::GetEmptyData()

#endif

#endif