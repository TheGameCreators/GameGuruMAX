#ifndef _H_GGTHREAD
#define _H_GGTHREAD

#include "windows.h"
#include "process.h"

namespace GGThread
{

class threadCondition;

class threadLock 
{
	protected:
		CRITICAL_SECTION m_cs;

		friend threadCondition;

	public:

		threadLock() { InitializeCriticalSectionAndSpinCount( &m_cs, 4000 ); }
		~threadLock() { DeleteCriticalSection( &m_cs ); }

		bool Acquire()
		{
			EnterCriticalSection( &m_cs );
			return true;
		}

		void Release() { LeaveCriticalSection( &m_cs );	}
};

class threadCondition 
{
	protected:
		CONDITION_VARIABLE condition;
		CRITICAL_SECTION m_cs;
			
	public:
		threadCondition()
		{
			InitializeConditionVariable( &condition );
			InitializeCriticalSection( &m_cs );
		}

		~threadCondition() { DeleteCriticalSection( &m_cs ); }
		
		void Wait() 
		{ 
			EnterCriticalSection( &m_cs );
			SleepConditionVariableCS( &condition, &m_cs, INFINITE ); 
			LeaveCriticalSection( &m_cs );
		}
		void WaitCustom( threadLock& lock ) // lock must be acquired before and released after this function
		{
			SleepConditionVariableCS( &condition, &lock.m_cs, INFINITE ); 
		}
		void Signal() { WakeConditionVariable( &condition ); }
		void Broadcast() { WakeAllConditionVariable( &condition ); }
};

class threadAutoLock
{
	protected:
		threadLock *m_pLock;

	public:
		threadAutoLock( threadLock *lock )
		{
			if ( !lock ) return;
			m_pLock = lock;
			m_pLock->Acquire();
		}

		~threadAutoLock()
		{
			m_pLock->Release();
		}
};

class GGThread
{
	private:

		HANDLE hThread;
		uint32_t iThreadID;
		volatile bool bRunning;

	public:
		static uint32_t __stdcall EntryPoint( void *pParams )
		{
			if ( !pParams ) return 0;
			GGThread* pThis = (GGThread*) pParams;
			uint32_t result = 0;
			if ( !pThis->bTerminate ) 
			{
				result = pThis->Run();
			}
			MemoryBarrier();
			pThis->Finally();
			return result;
		}

		// when a thread stops for whatever reason this function should be called
		void Finally()
		{
			bRunning = false;
		}

	protected:

		// variables to tell the thread to stop, it must watch these and stop appropriately
		volatile bool bTerminate;
		void* pStop; 

		// this must be overridden by the inheriting class, contains all code that the thread will run.
		virtual uint32_t Run( ) = 0;
			
	public:

		GGThread( )
		{
			hThread = 0;
			iThreadID = 0;
			bRunning = false;
			bTerminate = false;
			pStop = CreateEvent( NULL, FALSE, FALSE, NULL );
		}

		virtual ~GGThread( )
		{
			// tell it to stop and wait for it to finish
			Stop();
			Join();

			// clean up
			if ( pStop ) CloseHandle( pStop );
			if ( hThread ) CloseHandle( hThread );
			hThread = 0;
		}

		bool IsRunning() { return bRunning; }
		
		// starts the thread and calls Run() (if not already running)
		virtual void Start( )
		{
			if ( bRunning ) return;
			bTerminate = false;
			bRunning = true;
			ResetEvent( pStop );
			if ( hThread ) CloseHandle( hThread );
			hThread = (HANDLE)_beginthreadex( NULL, 0, EntryPoint, (void*) this, 0, &iThreadID );
			SetThreadPriority( hThread, THREAD_PRIORITY_BELOW_NORMAL );
		}

		// tells the thread to stop and returns immediately, the thread is not guaranteed to stop
		virtual void Stop()
		{
			bTerminate = true;
			SetEvent( pStop );
		}

		// sleep for a specified time but will wake early if the thread is told to stop, check bTerminate after this command
		void SleepSafe( uint32_t milliseconds )
		{
			WaitForSingleObject( pStop, milliseconds );
		}

		// waits for the thread to stop then returns, can wait forever. If thread is already stopped or 
		// not yet started returns immediately
		void Join( )
		{
			if ( !IsRunning() ) return;
			WaitForSingleObject( hThread, INFINITE );
		}
};

} // GGThread namespace

#endif // _H_GGTHREAD