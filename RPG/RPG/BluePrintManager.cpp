#include "stdafx.h"
#include "BluePrintManager.h"

namespace Solidus
{
	BluePrintManager* BluePrintManager::s_instance = nullptr;

	BluePrintManager::BluePrintManager()
	{}

	void BluePrintManager::Create()
	{
		if( !s_instance )
		{
			s_instance = new BluePrintManager;
		}
	}

	void BluePrintManager::Destroy()
	{
		if( s_instance )
		{					
			// Resetting the instance
			for(auto it = s_instance->m_blueprints.begin(); it != s_instance->m_blueprints.end(); ++it)
			{
				delete &it;
			}

			delete s_instance;
			s_instance = nullptr;
		}		
	}

	BluePrintManager& BluePrintManager::Instance()
	{
		if( !s_instance )
		{
			Create();
		}

		return *s_instance;
	}
}