#include "stdafx.h"
#include "Thread.h"
#include <process.h>
#include "sdMutexLock.hpp"

namespace Solidus
{	
	extern "C" 
		unsigned __stdcall ThreadWrapper( void* args )
	{
		Solidus::Thread* t = ( Solidus::Thread* ) args;

		if ( !TlsSetValue( self_tls_index, args ))
		{
			return 1;
		}

		//
		// Now invoke the thread function with the given argument.
		//
		if ( t->m_fnvoid != NULL) 
		{
			(*t->m_fnvoid)( t->m_threadArg );
			Solidus::Thread::Exit();
		}

		if ( t->m_fnret != NULL) 
		{
			void* return_value = (*t->m_fnret)( t->m_threadArg );
			Solidus::Thread::Exit( return_value );
		}

		return 0;
	}

	Thread::Thread( NoReturnThreadFunc fn, void* arg, Priority::Base priority )
		: m_threadArg( arg )
		, m_suspended( true )
		, m_detached( true )
		, m_priority( priority )
	{		
		m_fnvoid	 = fn;
		m_fnret		 = nullptr;
	}

	Thread::Thread( ThreadFunc fn, void* arg, Priority::Base priority )
		: m_threadArg( arg )
		, m_suspended( true )
		, m_detached( false )
		, m_priority( priority )
	{		
		m_fnvoid	 = nullptr;
		m_fnret		 = fn;
	}

	bool Thread::Start()
	{
		m_thread = 
			( HANDLE )_beginthreadex(NULL, 0, ThreadWrapper, (LPVOID)this, CREATE_SUSPENDED, &m_threadId );		

		if ( m_thread )
		{
			// Set initial thread priority
			SetPriority( m_priority );
			
			// Change the thread state
			//
			m_state = Running;
			
			// Set the suspended flag to false
			//
			m_suspended = false;

			// Resume thread execution
			//
			ResumeThread( m_thread );

			return true;

		} // End if started

		return false;
	}

	void Thread::SetPriority( const Priority::Base& priority )
	{
		m_priority = priority;
		if ( m_thread )
		{
			int nPriority = THREAD_PRIORITY_NORMAL;
			switch ( priority )
			{
			case Priority::Idle:
				nPriority = THREAD_PRIORITY_IDLE;
				break;
			case Priority::Lowest:
				nPriority = THREAD_PRIORITY_LOWEST;				
				break;
			case Priority::Low:
				nPriority = THREAD_PRIORITY_BELOW_NORMAL;
				break;
			case Priority::High:
				nPriority = THREAD_PRIORITY_ABOVE_NORMAL;
				break;
			case Priority::Highest:
				nPriority = THREAD_PRIORITY_HIGHEST;
				break;
			case Priority::Critical:
				nPriority = THREAD_PRIORITY_TIME_CRITICAL;
				break;

			} // End switch priority
			SetThreadPriority( m_thread, nPriority );

		} // End if thread running
	}

	void Thread::ThreadYield()
	{
		::Sleep( 0 );
	}

	void Thread::Sleep( unsigned long milliseconds )
	{
		::Sleep( milliseconds );
	}

	Solidus::ThreadId Thread::CurrentThreadID()
	{
		return GetCurrentThreadId();
	}


	Solidus::Thread* Thread::Self()
	{
		return ( Thread* )TlsGetValue( self_tls_index );
	}

	void Thread::Exit( void* retval )
	{
		Thread* currentThread = Self();

		if ( currentThread ) // if an IThread
		{
			sdMutexLock lock( currentThread->m_mutex );
			currentThread->m_state = Terminated;

			if( currentThread->m_detached )
				delete currentThread;
			else
				currentThread->m_returnValue = retval;
		}

		//   _endthreadex() does not automatically closes the thread handle.
		//   The IThread dtor closes the thread handle.
		_endthreadex(0); // in exit

	}

	void Thread::Suspend()
	{
		// Wait for the suspended variable to be updated
		while( 1 != ::InterlockedExchange( &m_suspended, 1 ) )
		{
		}
	}

	Thread::ThreadState Thread::GetThreadState() const
	{
		// Not started yet?
		if ( !m_thread )
			return Terminated;

		// Thread is active?
		unsigned long nResult = ::WaitForSingleObject( m_thread, 0 );
		if ( nResult != WAIT_OBJECT_0 )
		{
			// Suspended?
			if ( m_suspended )
				return Suspended;
			else
				return Running;

		} // End if thread running
		else
			return Terminated;
	}

	void Thread::Resume()
	{
		// Wait for the suspended variable to be udpated
		while( 0 != ::InterlockedExchange( &m_suspended, 0 ) )
		{
		}
	}

	void Thread::Terminate()
	{
		if ( m_thread && !m_detached )
		{						
			Join();
			CloseHandle( m_thread );
		} // End if started
	}

	void Thread::Join()
	{
		if ( m_thread )
			WaitForSingleObject( m_thread, INFINITE );
	}
}