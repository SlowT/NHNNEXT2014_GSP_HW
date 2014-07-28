#pragma once
#include <winsock2.h>
#include <assert.h>

//환경 변수
const unsigned int SERVER_PORT		= 9001;
const unsigned int MAX_BUFFER_SIZE	= 2048;
const unsigned int WM_SOCKET		= WM_USER + 1;

bool InitServer( HWND hWnd );
void CloseServer();
LRESULT CALLBACK SocketProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
bool ClientAccept( HWND hWnd );
bool ClientClose( SOCKET clientSocket );
bool ClientRead( HWND hWnd, SOCKET clientSocket );
bool ClientWrite( SOCKET clientSocket );