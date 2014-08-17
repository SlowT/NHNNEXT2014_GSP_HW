// EduServer_IOCP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Exception.h"
#include "MemoryPool.h"
#include "EduClient_IOCP.h"
#include "ClientSession.h"
#include "SessionManager.h"
#include "IocpManager.h"


__declspec(thread) int LThreadType = -1;

int _tmain(int argc, _TCHAR* argv[])
{
	LThreadType = THREAD_MAIN;

	/// for dump on crash
	SetUnhandledExceptionFilter(ExceptionFilter);

	/// Global Managers
	GMemoryPool = new MemoryPool;
	GSessionManager = new SessionManager;
	GIocpManager = new IocpManager;

// 	USHORT port = DEFAULT_PORT;
// 	if( argc >= 3 ){
// 		port = _wtoi(argv[2]);
// 	}

	if (false == GIocpManager->Initialize())
		return -1;

	if (false == GIocpManager->StartIoThreads())
		return -1;

	
	printf_s("Start Clients\n");


	GIocpManager->StartConnects(MAX_CONNECTION); ///< block here...

	printf_s( "Total Send Bytes : %d\n", GIocpManager->mSendCount );
	printf_s( "Total Recv Bytes : %d\n", GIocpManager->mRecvCount );

	GIocpManager->Finalize();

	printf_s("End Clients\n");

	delete GIocpManager;
	delete GSessionManager;
	delete GMemoryPool;

	return 0;
}

