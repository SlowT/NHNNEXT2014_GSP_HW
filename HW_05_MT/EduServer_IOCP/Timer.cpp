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

	//TODO: mTimerJobQueue에 TimerJobElement를 push..
	//LTickCount = GetTickCount64();
	//TimerJobElement tmp( owner, task, LTickCount + after );
	//mTimerJobQueue.push( tmp );

	///# 비용을 조금 더 줄이려면 이렇게
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
		// 왜인지 모르겠지만 종종 분명 mTimerJobQueue에는 top값이 있는데 timerJobElem이 널값이 된다.
		if( LTickCount < mTimerJobQueue.top().mExecutionTick )
			break;

		mTimerJobQueue.top().mOwner->EnterLock();
		
		mTimerJobQueue.top().mTask();

		mTimerJobQueue.top().mOwner->LeaveLock();

// 		GGrandCentralExecuter->DoDispatch( timerJobElem.mTask );

		mTimerJobQueue.pop();
	}


}

