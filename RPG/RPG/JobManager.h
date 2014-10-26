#pragma once

#ifndef _JOBMANAGER_H_
#define _JOBMANAGER_H_

#include <queue>
#include <vector>

#include "sdMutex.hpp"
#include "BaseJobComparison.h"
#include "NamedProperties.h"

namespace Solidus
{	
	class BaseJob;
	class Thread;
	class JobManager
	{
		typedef std::priority_queue< BaseJob*,std::vector<BaseJob* >, BaseJobComparison > JobQueue;
	public:
		static void		   Create();		
		static void		   Destroy();		
		static JobManager& Instance();		
		static int		   CreateJob( const JobPriority::Base& priority = JobPriority::Average );
		static int		   CreateJob( const std::string& jobName, NamedProperties args, const JobPriority::Base& priority = JobPriority::Average );
		static int		   CreateDownloadJob( const std::string& fileName, const JobPriority::Base& priority = JobPriority::Average );
		static int		   CreateThread();
		static void		   SpawnWorkerThreads( const int& workerThreadsCount );
		static void		   SpawnJobs( const int& jobCount );

		BaseJob*		   GetNextJob();		
		void			   AddDoneJob( BaseJob* doneJob );
		void			   Update();				
		bool			   IsShutdown();		
		bool			   HasJobs();		
		void			   ShutDown();		

	private:
		JobManager()									  // Setting the entity as non-copyable
		{ 
			m_jobIndex = 0;		
			m_shutDown = false;
		}
		JobManager( JobManager const& ) {}				   // Setting the entity as non-copyable
		JobManager& operator=( JobManager const& ) {}	   // Setting the entity as non-copyable

		// Jobs
		JobQueue				m_waitingJobs;
		std::vector< BaseJob* >	m_doneJobs;
		int						m_jobIndex;

		// Threading
		std::vector< Thread* >  m_threads;
		sdMutex					m_waitingJobsMutex;
		sdMutex					m_doneJobsMutex;

		// Status
		bool					m_shutDown;

		static JobManager*		s_instance;
	};
}

#endif