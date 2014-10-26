#pragma once

#ifndef _PROFILERCONFIG_H_
#define _PROFILERCONFIG_H_

//-----------------------------------------------------------------------------

#ifndef PROFILER
#define PROFILER				TRUE
#endif

#ifndef PROFILER_LOOKUPRATE
#define PROFILER_LOOKUPRATE	TRUE
#endif


//-----------------------------------------------------------------------------

#ifndef FALSE
#define FALSE	0x0
#endif

#ifndef TRUE
#define TRUE	0x1
#endif

#ifndef NULL
#define NULL	0
#endif


//-----------------------------------------------------------------------------

#define PROFILE_PLATFORM_WIN32	0x1
#define PROFILE_PLATFORM_POSIX	0x2

#if defined (_WIN32)
#   define PROFILE_PLATFORM	PROFILE_PLATFORM_WIN32

#else
//	ASSUME: POSIX-compliant OS
#   define PROFILE_PLATFORM	PROFILE_PLATFORM_POSIX
#endif


//-----------------------------------------------------------------------------

#define PROFILE_COMPILER_MSVC		0x1
#define PROFILE_COMPILER_GNUC		0x2
#define PROFILE_COMPILER_OTHER	0x3

#if defined (_MSC_VER)
#   define PROFILE_COMPILER	PROFILE_COMPILER_MSVC

#elif defined (__GNUG__)
#   define PROFILE_COMPILER	PROFILE_COMPILER_GNUC

#else
#   define PROFILE_COMPILER	PROFILE_COMPILER_OTHER
#endif

#endif
