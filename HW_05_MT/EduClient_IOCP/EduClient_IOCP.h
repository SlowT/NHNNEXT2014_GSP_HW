#pragma once

#define SERVER_ADDR		"127.0.0.1"
#define SERVER_PORT		9001
#define MAX_CONNECTION	200
#define SEND_SIZE		1024
#define CONNECT_TIME	10000

enum THREAD_TYPE
{
	THREAD_MAIN,
	THREAD_IO_WORKER
};

extern __declspec(thread) int LThreadType;