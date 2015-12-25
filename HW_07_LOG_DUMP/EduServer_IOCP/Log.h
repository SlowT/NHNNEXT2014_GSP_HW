#pragma once

#include <iostream>
#include "Exception.h"
#include "ThreadLocal.h"

class ThreadCallHistory
{
public:
	ThreadCallHistory(int tid) : mThreadId(tid)
	{
		memset(mHistory, 0, sizeof(mHistory));
	}

	inline void Append(const char* funsig)
	{
		mHistory[mCounter++ & (MAX_HISTORY - 1)] = funsig;
	}

	void DumpOut(std::ostream& ost = std::cout);

private:
	enum
	{
		MAX_HISTORY = 1024
	};

	uint64_t	mCounter = 0;
	int			mThreadId = -1;
	const char*	mHistory[MAX_HISTORY];
};


#define TRACE_THIS	\
	__if_exists (this)	\
	{	\
		LRecentThisPointer = (void*)this;	\
	}	\
	if (LThreadType != THREAD_MAIN)	\
	{	\
		/*todo: 스레드로컬에 함수 호출(__FUNCSIG__) 기록남기기*/ \
		LThreadCallHistory->Append(__FUNCSIG__);	\
		TRACE_PERF;	\
	}	
	


class ThreadCallElapsedRecord
{
public:
	ThreadCallElapsedRecord(int tid) : mThreadId(tid)
	{
		memset(mElapsedFuncSig, 0, sizeof(mElapsedFuncSig));
		memset(mElapsedTime, 0, sizeof(mElapsedTime));
	}

	inline void Append(const char* funcsig, int64_t elapsed)
	{
		//todo: mElapsedFuncSig, mElapsedTime에 정보(funcsig, elapsed) 남기기
		mElapsedFuncSig[mCounter & (MAX_ELAPSED_RECORD - 1)] = funcsig;
		mElapsedTime[mCounter++ & (MAX_ELAPSED_RECORD - 1)] = elapsed;
	}

	void DumpOut(std::ostream& ost = std::cout);

private:
	enum
	{
		MAX_ELAPSED_RECORD = 512
	};

	uint64_t	mCounter = 0;
	int			mThreadId = -1;
	const char*	mElapsedFuncSig[MAX_ELAPSED_RECORD];
	int64_t		mElapsedTime[MAX_ELAPSED_RECORD];
};

class ScopeElapsedCheck
{
public:
	ScopeElapsedCheck(const char* funcsig) : mFuncSig(funcsig)
	{
		/* FYI
		 * 10~16 ms 해상도로 체크하려면 GetTickCount 사용
		 * 1 us 해상도로 체크하려면  QueryPerformanceCounter 사용
		*/ 
		mStartTick = GetTickCount64();
	}

	~ScopeElapsedCheck()
	{
		if (LThreadType != THREAD_MAIN)
		{
			//todo: LThreadCallElapsedRecord에 함수 수행 시간 남기기
			LThreadCallElapsedRecord->Append( mFuncSig, GetTickCount64() - mStartTick );
			///# LThreadCallElapsedRecord
		}
	}

private:

	const char*	mFuncSig;
	int64_t	mStartTick = 0;
};

#define TRACE_PERF	\
	ScopeElapsedCheck __scope_elapsed_check__(__FUNCSIG__);


namespace LoggerUtil
{

	struct LogEvent
	{
		int mThreadId = -1;
		int	mAdditionalInfo = 0;
		const char* mMessage = nullptr; 
	};

	#define MAX_LOG_SIZE  65536   ///< Must be a power of 2

	extern LogEvent gLogEvents[MAX_LOG_SIZE];
	extern __int64 gCurrentLogIndex;

	inline void EventLog(const char* msg, int info)
	{
		__int64 index = _InterlockedIncrement64(&gCurrentLogIndex) - 1;
		
		//todo: gLogEvents에 LogEvent정보 남기기
		/*
		gLogEvents[index % MAX_LOG_SIZE].mThreadId = LWorkerThreadId;
		gLogEvents[index % MAX_LOG_SIZE].mAdditionalInfo = info;
		if( nullptr != gLogEvents[index % MAX_LOG_SIZE].mMessage ){
			delete[] gLogEvents[index % MAX_LOG_SIZE].mMessage; ///# 이거 무서운거다...
			gLogEvents[index % MAX_LOG_SIZE].mMessage = nullptr;
		}
		gLogEvents[index % MAX_LOG_SIZE].mMessage = msg;////////todo:일단 혹시나 되나해서...
		*/ ///## 앗;; 이거 고친다는거 깜박하고 그냥 커밋을... new로 만들어서 copy헤야할지 그냥 포인터만 넘길지 고민하다 이도저도 아닌채 방치됐던 코드...

		LogEvent& event = gLogEvents[index & (MAX_LOG_SIZE - 1)]; ///## %연산은 느리니 2의배수일 경우 이렇게 가능하다고 한다.
		event.mThreadId = LWorkerThreadId;
		event.mMessage = msg;
		event.mAdditionalInfo = info;

	}

	void EventLogDumpOut(std::ostream& ost = std::cout);
}

#define EVENT_LOG(x, info) LoggerUtil::EventLog(x, info)

