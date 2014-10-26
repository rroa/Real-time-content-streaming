#pragma once

#ifndef _UPDATE_MANAGER_H_
#define _UPDATE_MANAGER_H_

namespace Solidus
{
	class UpdateManager
	{
	public:
		// FUNCTIONS
		//
		static void				  Create();		
		static void				  Destroy();
		static UpdateManager&	  Instance();
	private:
		// CTOR
		//
		UpdateManager() {}		
		UpdateManager( UpdateManager const& ) {};
		UpdateManager& operator=( UpdateManager const& ) {};
		~UpdateManager(){}

		// SINGLETON
		static UpdateManager* s_instance;
	};
}

#endif // ! _UPDATE_MANAGER_H_