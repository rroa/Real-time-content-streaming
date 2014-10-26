#include "stdafx.h"
#include "JobManager.h"
#include "Thread.h"
#include "BaseJob.h"
#include "sdMutexLock.hpp"

#include <sstream>
#include "ReportErrors.h"
#include "LoadFileJob.h"
#include "HashBufferJob.h"
#include "GetPlayerColorJob.h"
#include "CheckTextureChange.h"
#include "CheckForUpdatesJob.h"
#include "DownloadFileJob.h"
#include "ManifestDownload.h"
#include "SetManifestJob.h"

#define MAX_THREADS 7

namespace Solidus
{
	void WorkerThreadFunction( void* args )
	{
		if(args != nullptr)
		{
			trace("Args is not empty");
		}

		for(;;)
		{
			BaseJob* currentJob = JobManager::Instance().GetNextJob();

			if( currentJob )
			{
				trace( "Thread Id: " << GetCurrentThreadId() << " " << currentJob->Name() << " executing." );
				currentJob->Execute();

				JobManager::Instance().AddDoneJob( currentJob );
				currentJob = nullptr;
			}

			if( JobManager::Instance().IsShutdown() )
				break;

			Thread::ThreadYield();
		}
	}

	JobManager* JobManager::s_instance = nullptr;

	void JobManager::Create()
	{
		if( !s_instance )
		{
			s_instance = new JobManager;

			//SpawnJobs( 10 );
			SpawnWorkerThreads( 1 );
		}
	}

	void JobManager::Destroy()
	{
		if( s_instance )
		{			
			//  Cleanup
			s_instance->m_waitingJobs = JobQueue();
			s_instance->m_doneJobs.clear();

			s_instance->ShutDown();

			for( auto threadIter = s_instance->m_threads.begin(); 
				threadIter != s_instance->m_threads.end(); ++threadIter )
			{
				( *threadIter )->Terminate(); 
			}

			// Resetting the instance
			delete s_instance;
			s_instance = nullptr;
		}		
	}

	JobManager& JobManager::Instance()
	{
		if( !s_instance )
		{
			Create();
		}

		return *s_instance;
	}

	int JobManager::CreateJob( const JobPriority::Base& priority )
	{
		//Create();

		std::stringstream name;
		name << "Job ";
		name << ++s_instance->m_jobIndex;

		sdMutexLock lock( s_instance->m_waitingJobsMutex );
		
		/*srand( GetTickCount() );
		int x = rand() % 100;

		if( x <= 30 )
		{
			s_instance->m_waitingJobs.push( new LoadFileJob( name.str(), "test.txt", priority ));
		}
		else if( x > 30 && x <= 60 )
		{
			s_instance->m_waitingJobs.push( new HashBufferJob( name.str(), "test.txt", priority ));
		}
		else
		{
			s_instance->m_waitingJobs.push( new BaseJob( name.str(), priority ));
		}*/

		//s_instance->m_waitingJobs.push( new GetPlayerColorJob( name.str(), priority ));
		s_instance->m_waitingJobs.push( new CheckForUpdateJob( name.str(), priority ));
		//s_instance->m_waitingJobs.push( new CheckTextureChangeJob( name.str(), priority ));
				
		return s_instance->m_jobIndex;
	}

	int JobManager::CreateJob( const std::string& jobName, NamedProperties args, const JobPriority::Base& priority )
	{
		std::stringstream name;
		name << "Job ";
		name << ++s_instance->m_jobIndex;

		sdMutexLock lock( s_instance->m_waitingJobsMutex );

		if( jobName == "SetManifest" )
		{			
			s_instance->m_waitingJobs.push( new SetManifestJob( name.str(), priority ));
		}

		if( jobName == "DownloadManifest" )
		{
			std::string fileName;
			args.GetProperty( "fileName", fileName );
			s_instance->m_waitingJobs.push( new ManifestDownload( name.str(), fileName, priority ));
		}

		return s_instance->m_jobIndex;
	}

	int JobManager::CreateDownloadJob( const std::string& fileName, const JobPriority::Base& priority )
	{	
		std::stringstream name;
		name << "Job ";
		name << ++s_instance->m_jobIndex;

		sdMutexLock lock( s_instance->m_waitingJobsMutex );
			
		s_instance->m_waitingJobs.push( new DownloadFileJob( name.str(), fileName, priority ));
						
		return s_instance->m_jobIndex;
	}

	int JobManager::CreateThread()
	{
		Create();

		if(  s_instance->m_threads.size() >= MAX_THREADS ) 
			return s_instance->m_threads.size();

		Thread* newThread = new Thread( WorkerThreadFunction );
		
		s_instance->m_threads.push_back( newThread );

		newThread->Start();

		return s_instance->m_threads.size();
	}

	BaseJob* JobManager::GetNextJob()
	{
		sdMutexLock lock( m_waitingJobsMutex );
		if( !m_waitingJobs.empty() )
		{
			BaseJob* topJob = m_waitingJobs.top();
			m_waitingJobs.pop();
			return topJob;
		}

		return nullptr;
	}

	void JobManager::AddDoneJob( BaseJob* doneJob )
	{
		sdMutexLock lock( m_doneJobsMutex );
		m_doneJobs.push_back( doneJob );
	}

	void JobManager::Update()
	{
		//if( !HasJobs() ) return;

		if( static_cast< int >( m_threads.size() ) == 0 )
		{
			sdMutexLock lock( m_waitingJobsMutex );		
			BaseJob* currentJob = JobManager::Instance().GetNextJob();

			if( currentJob )
			{
				trace( "Thread Id: " << GetCurrentThreadId() << " " << currentJob->Name() << " executing." );
				currentJob->Execute();

				JobManager::Instance().AddDoneJob( currentJob );
				currentJob = nullptr;
			}
			
			Thread::ThreadYield();
		}

		sdMutexLock lock( m_doneJobsMutex );		

		std::vector< BaseJob * > outJobs;
		{
			//Lock the completed job queue mutex
			sdMutexLock lock( m_doneJobsMutex );

			//Find the jobs that have the same thread id as this current thread and remove them

			unsigned long thisThreadID = GetCurrentThreadId();

			for( size_t jobIndex = 0; jobIndex < m_doneJobs.size(); ++jobIndex )
			{
				BaseJob* thisJob = m_doneJobs[ jobIndex ];

				if( thisJob->ThreadId() == thisThreadID )
				{
					outJobs.push_back( thisJob );

					m_doneJobs.erase( m_doneJobs.begin() + jobIndex );
					--jobIndex;
				}
			}
		}

		//Run the callback function on the jobs that have the same thread ID as this current thread
		//Delete the job object

		for( auto jobIter = outJobs.begin(); jobIter != outJobs.end(); ++jobIter )
		{
			BaseJob* theJob = *jobIter;
			theJob->CallBack();
			delete theJob;
		}		
	}

	bool JobManager::IsShutdown()
	{
		return ( m_shutDown == true );
	}

	bool JobManager::HasJobs()
	{
		return ( m_waitingJobs.size() > 0 );
	}

	void JobManager::ShutDown()
	{
		m_shutDown = true;
	}

	void JobManager::SpawnWorkerThreads( const int& workerThreadsCount )
	{
		int count = workerThreadsCount;
		if( workerThreadsCount > MAX_THREADS ) 
			count = MAX_THREADS;

		for( int i = 0; i < count; ++i )
		{
			CreateThread();
		}
	}

	void JobManager::SpawnJobs( const int& jobCount )
	{		
		srand(  GetTickCount() );

		for( int i = 0; i < jobCount; ++i )
		{
			Solidus::JobPriority::Base priority;
			int x = rand() % 100;

			if( x >= 75 )
			{
				priority = Solidus::JobPriority::High;
			}
			else if( x < 75 && x > 25 )
			{
				priority = Solidus::JobPriority::Average;
			}
			else
			{
				priority = Solidus::JobPriority::Low;
			}

			Solidus::JobManager::CreateJob( priority );
		}

	}

}