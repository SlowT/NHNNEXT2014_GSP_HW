#include "stdafx.h"
#include "ThreadLocal.h"
#include "Exception.h"
#include "SyncExecutable.h"
#include "Timer.h"
#include "GrandCentralExecuter.h"


Timer::Timer()
{
	LTickCount = GetTickCount64();
}


void Timer::PushTimerJob(SyncExecutablePtr owner, const TimerTask& task, uint32_t after)
{
	CRASH_ASSERT(LThreadType == THREAD_IO_WORKER);

	//TODO: mTimerJobQueue�� TimerJobElement�� push..
	//LTickCount = GetTickCount64();
	//TimerJobElement tmp( owner, task, LTickCount + after );
	//mTimerJobQueue.push( tmp );

	///# ����� ���� �� ���̷��� �̷���
	int64_t dueTimeTick = after + LTickCount;
	mTimerJobQueue.push(TimerJobElement(owner, task, dueTimeTick));

	
}


void Timer::DoTimerJob()
{
	/// thread tick update
	LTickCount = GetTickCount64();

	while (!mTimerJobQueue.empty())
	{
// 		const TimerJobElement& timerJobElem = mTimerJobQueue.top(); 
		// ������ �𸣰����� ���� �и� mTimerJobQueue���� top���� �ִµ� timerJobElem�� �ΰ��� �ȴ�.
		if( LTickCount < mTimerJobQueue.top().mExecutionTick )
			break;

		mTimerJobQueue.top().mOwner->EnterLock();
		
		mTimerJobQueue.top().mTask();

		mTimerJobQueue.top().mOwner->LeaveLock();

// 		GGrandCentralExecuter->DoDispatch( timerJobElem.mTask );

		mTimerJobQueue.pop();
	}


}

