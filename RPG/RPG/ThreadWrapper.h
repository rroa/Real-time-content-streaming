//#pragma once
//
//#ifndef _THREADWRAPPER_H_
//#define _THREADWRAPPER_H_
//
//#include "Thread.h"
//
//namespace Solidus
//{
//	extern "C" 
//		unsigned __stdcall
//		ThreadWrapper( void* args )
//	{
//		Solidus::Thread* t = (Solidus::Thread*)ptr;
//
//		if ( !TlsSetValue( self_tls_index, ptr) )
//		{
//			return 1;
//		}
//
//		//
//		// Now invoke the thread function with the given argument.
//		//
//		if ( t->m_fnvoid != NULL) 
//		{
//			(*t->m_fnvoid)( t->m_threadArg );
//			Solidus::Thread::Exit();
//		}
//
//		if ( t->m_fnret != NULL) 
//		{
//			void* return_value = (*t->m_fnret)( t->m_threadArg );
//			Solidus::Thread::Exit( return_value );
//		}
//
//		return 0;
//	}
//
//}
//
//#endif