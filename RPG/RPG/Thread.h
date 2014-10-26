#pragma once

#ifndef _THREAD_H_
#define _THREAD_H_

#include <windows.h>

#include "sdMutex.hpp"

namespace Solidus
{
	extern "C" 
		unsigned __stdcall ThreadWrapper( void* args );

	static unsigned int self_tls_index = 0;
	class Thread;

	// Global typedefs
	typedef unsigned int ThreadId;
	typedef void  (*NoReturnThreadFunc)( void* args );
	typedef void* (*ThreadFunc )( void* args );

	//-----------------------------------------------------------------------------
	// Global Enumerations
	//-----------------------------------------------------------------------------
	namespace Priority
	{
		enum Base
		{
			Idle = 0,
			Lowest,
			Low,
			Normal,
			High,
			Highest,
			Critical
		};
	}

	class Thread
	{		
		
	public:
		/* =============================================================
		 * ENUMERATIONS
		 * ============================================================= */

		enum ThreadState
		{
			Initialized,
			Running,
			Terminated,
			Suspended
		};

		/* =============================================================
		 * CTOR
		 * ============================================================= */

		Thread( NoReturnThreadFunc fn, void* arg = NULL, Priority::Base priority = Priority::Normal );
		Thread( ThreadFunc fn, void* arg = NULL, Priority::Base priority = Priority::Normal );

		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */
		bool			           Start			 ();
		void			           SetPriority		 ( const Priority::Base& priority );
		ThreadState		           GetThreadState    () const;
		void					   Suspend           ();
		void					   Resume            ();
		bool					   SuspendedWait     ();
		void					   Terminate         ();
		void					   Join              ();

		/* =============================================================
		 * STATIC FUNCTIONS
		 * ============================================================= */

		static void				   ThreadYield		  ();
		static void				   Sleep			  ( unsigned long milliseconds );
		static ThreadId			   CurrentThreadID	  ();
		static Thread*			   Self				  ();
		static void				   Exit				  ( void* retval = NULL );

	private:
		/* =============================================================
		 * MEMBERS
		 * ============================================================= */

		HANDLE					   m_thread;
		ThreadId				   m_threadId;
		NoReturnThreadFunc		   m_fnvoid;
		ThreadFunc				   m_fnret;
		ThreadState				   m_state;
		Priority::Base			   m_priority;
		void*					   m_threadArg;
		volatile unsigned long	   m_suspended;
		sdMutex					   m_mutex;
		bool					   m_detached;
		void*					   m_returnValue;

		friend unsigned __stdcall  ThreadWrapper( void* args );
	};
}
#endif