//---------------------------------------------------------------------------//
//		   _____  ____  _      _____ _____  _    _  _____					 //
//		  / ____|/ __ \| |    |_   _|  __ \| |  | |/ ____|					 //
//		 | (___ | |  | | |      | | | |  | | |  | | (___  					 //
//		  \___ \| |  | | |      | | | |  | | |  | |\___ \ 					 //
//		  ____) | |__| | |____ _| |_| |__| | |__| |____) |					 //
//		 |_____/ \____/|______|_____|_____/ \____/|_____/ 					 //
//						Ra�l Roa - Solidus Game Development Engine			 //
//																			 //
//---------------------------------------------------------------------------//
//                                                                           //
// Name : sdMutex.hpp		                                                 //
//                                                                           //
// Desc : Threading platform independent wrapper for mutual exclusion		 //
//        (MutEx) object representation										 //
//                                                                           //
//---------------------------------------------------------------------------//
//      Copyright (c) 2013 Ra�l Roa. All Rights Reserved.					 //
//---------------------------------------------------------------------------//

#pragma once
#if !defined( _SGE_SDMUTEX_H_ )
#define _SGE_SDMUTEX_H_

#include <windows.h>

//-----------------------------------------------------------------------------
// Solidus Namespace
//-----------------------------------------------------------------------------
namespace Solidus
{		
	class sdMutex
	{
		friend class sdMutexLock;
	public:
		/* =============================================================
		 * CTOR
		 * ============================================================= */

		sdMutex();
		~sdMutex();
	private:
		/* =============================================================
		 * PRIVATE FUNCTIONS
		 * ============================================================= */

		void Acquire();		
		void Release();
		
		/* =============================================================
		 * MEMBERS
		 * ============================================================= */

		CRITICAL_SECTION m_critSection;
		int				 m_recurseCount;
	};
}

#endif // !_SGE_SDMUTEX_H_