#include "stdafx.h"
#include "UpdateManager.h"

namespace Solidus
{
	UpdateManager* UpdateManager::s_instance = nullptr;	

	void UpdateManager::Create()
	{
		if( !s_instance )
		{
			s_instance = new UpdateManager;
		}
	}

	void UpdateManager::Destroy()
	{
		if( s_instance )
		{			
			delete s_instance;
			s_instance = nullptr;
		}		
	}

	UpdateManager& UpdateManager::Instance()
	{
		if( !s_instance )
		{
			Create();
		}

		return *s_instance;
	}
}