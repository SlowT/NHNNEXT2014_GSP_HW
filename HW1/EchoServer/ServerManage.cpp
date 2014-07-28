#include "stdafx.h"
#include "ServerManage.h"
#include <stdio.h>
#include <vector>

typedef struct
{
	SOCKET socket;
	char recvBuffer[MAX_BUFFER_SIZE + 1]; // 받기 버퍼
	char sendBuffer[MAX_BUFFER_SIZE + 1]; // 쓰기 버퍼
	int recvBytes; // 받은 데이터 크기
	int sendBytes; // 쓰기 데이터 크기
} SOCKET_INFO;

std::vector<SOCKET_INFO*> clientList;

SOCKET listenSocket = INVALID_SOCKET;

bool InitServer( HWND hWnd )
{
	AllocConsole();
	FILE *consoleOut;
	freopen_s( &consoleOut, "CONOUT$", "wt", stdout );

// winsock 초기화
	WSADATA wsaData;
	int errorCheck;
	errorCheck = WSAStartup( 0x0202, &wsaData );
	if( errorCheck ) {
		assert( false );
		return 1;
	}

// 서버(listen) 소켓 생성
	listenSocket = socket( PF_INET, SOCK_STREAM, 0 );
	if( listenSocket == INVALID_SOCKET ){
		WSACleanup();
		assert( false );
		return 1;
	}

// WSAAsyncSelect 설정
	errorCheck = WSAAsyncSelect( listenSocket, hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE );
	if( errorCheck == SOCKET_ERROR )
	{
		closesocket( listenSocket );
		WSACleanup();
		assert( false );
		return 1;
	}

// 포트에 바인드
	SOCKADDR_IN serverSocketAddr;
	serverSocketAddr.sin_family = AF_INET;
	serverSocketAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	serverSocketAddr.sin_port = htons( SERVER_PORT );

	errorCheck = bind( listenSocket, (SOCKADDR *)&serverSocketAddr, sizeof( serverSocketAddr ) );
	if( errorCheck == SOCKET_ERROR ){
		closesocket( listenSocket );
		WSACleanup();
		assert( false );
		return 1;
	}

// listen 시작
	errorCheck = listen( listenSocket, SOMAXCONN );
	if( errorCheck == SOCKET_ERROR ){
		closesocket( listenSocket );
		WSACleanup();
		assert( false );
		return 1;
	}

	printf_s( "서버 시작\n" );

	clientList.reserve( 1000 );
	return 0;
}

void CloseServer()
{
	for( auto& client : clientList ){
		closesocket( client->socket );
		delete client;
	}
	closesocket( listenSocket );
	WSACleanup();
	printf_s( "서버 종료\n" );
	FreeConsole();
}

LRESULT CALLBACK SocketProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	message = WSAGETSELECTEVENT( lParam );

	switch( message )
	{
	case FD_ACCEPT:
		if( ClientAccept( hWnd ) )
			printf_s( "클라이언트 접속 실패 : %d\n", wParam );
		else
			printf_s( "클라이언트 접속 : %d\n", wParam );
		break;
	case FD_CLOSE:
		if( ClientClose( wParam ) )
			printf_s( "클라이언트 접속 종료 실패 : %d\n", wParam );
		else
			printf_s( "클라이언트 접속 종료 : %d\n", wParam );
		break;
	case FD_READ:
		if( ClientRead( hWnd, wParam ) )
			printf_s( "클라이언트 데이터 읽기 실패 : %d\n", wParam );
		else
			printf_s( "클라이언트 데이터 읽음 : %d\n", wParam );
		break;
	case FD_WRITE:
		if( ClientWrite( wParam ) )
			printf_s( "클라이언트 데이터 쓰기 실패 : %d\n", wParam );
		else
			printf_s( "클라이언트 데이터 쓰기 : %d\n", wParam );
		break;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}

bool ClientAccept( HWND hWnd )
{
	SOCKADDR_IN clientAddr;
	int addrLength = sizeof( SOCKADDR_IN );
	SOCKET clientSocket(INVALID_SOCKET);

	clientSocket = accept( listenSocket, (SOCKADDR*)&clientAddr, &addrLength );
	if( clientSocket == INVALID_SOCKET ){
		if( WSAGetLastError() == WSAEWOULDBLOCK )
			return 0;
		else
			return 1;
	}

	int errorChk = WSAAsyncSelect( clientSocket, hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE );
	if( errorChk == SOCKET_ERROR ){
		closesocket( clientSocket );
		return 1;
	}
	else{
		SOCKET_INFO* pSockInfo( new SOCKET_INFO );
		if( pSockInfo == nullptr )
			return 1;
		pSockInfo->socket = clientSocket;
		clientList.push_back( pSockInfo );
		return 0;
	}
}

bool ClientClose( SOCKET clientSocket )
{
	for( auto& iter = clientList.begin(); iter != clientList.end(); ++iter ){
		if( (*iter)->socket == clientSocket ){
			clientList.erase( iter );
			break;
		}
	}
	closesocket( clientSocket );
	return 0;
}

bool ClientRead( HWND hWnd, SOCKET clientSocket )
{
	SOCKET_INFO* pSockInfo( nullptr );
	for( auto& client : clientList ){
		if( client->socket == clientSocket ){
			pSockInfo = client;
			break;
		}
	}
	if( pSockInfo == nullptr )
		return 1;

	int recvBytes = recv( clientSocket, pSockInfo->recvBuffer, MAX_BUFFER_SIZE, 0 );
	if( recvBytes == SOCKET_ERROR )
	{
		if( WSAGetLastError() == WSAEWOULDBLOCK )
			return 0;
		else{
			ClientClose( clientSocket );
			return 1;
		}
	}

	pSockInfo->recvBytes = recvBytes;
	pSockInfo->recvBuffer[recvBytes] = '\0';

	memcpy( pSockInfo->sendBuffer, pSockInfo->recvBuffer, recvBytes );
	pSockInfo->sendBytes = recvBytes;

	PostMessage( hWnd, WM_SOCKET, clientSocket, FD_WRITE );
	return 0;
}

bool ClientWrite( SOCKET clientSocket )
{
	SOCKET_INFO* pSockInfo( nullptr );
	for( auto& client : clientList ){
		if( client->socket == clientSocket ){
			pSockInfo = client;
			break;
		}
	}
	if( pSockInfo == nullptr )
		return 1;

	if( pSockInfo->sendBytes <= 0 )
		return 0;

	int sendBytes = send( clientSocket, pSockInfo->sendBuffer, pSockInfo->sendBytes, 0 );
	if( sendBytes == SOCKET_ERROR ){
		if( WSAGetLastError() == WSAEWOULDBLOCK )
			return 0;
		else{
			ClientClose( clientSocket );
			return 1;
		}
	}

	pSockInfo->sendBytes = 0;
	return 0;
}
