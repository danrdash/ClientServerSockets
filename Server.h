//this module consists of the header file of the server side, all definitions and declarations and worthy includes included

#pragma once
#ifndef SERVER_H
#define SERVER_H
#include "Thread.h"




/* external variables and constants */

#define NUM_OF_WORKER_THREADS 2
#define MAX_LOOPS 3
#define SEND_STR_SIZE 35

#define BLACK  15
#define RED    4
#define YELLOW 5
#define NUM_OF_ROWS 6
#define NUM_OF_COLUMNS 7
#define WIN 1
#define LOSE 0
#define ILLEGAL_MOVE -2
#define INDENTATION 4
#define NOT_YET_WON -1
#define USR_RQST 0
#define PLAY_RQST 1
#define SEND_MSG 2
#define RCV_MSG 3
#define ILLEGAL_COMMAND -1
#define TOT_NO_TURNS 42

int play_buff[TOT_NO_TURNS];




//char clients_names[NUM_OF_WORKER_THREADS][MAX_USERNAME];

typedef struct Server_Arguments {
	char logfile[MAX_PATH_FILE];
	int s_port;
}s_args;

typedef struct Clients_Connected
{
	SOCKET server;
	char ***clients_names;
	char log_file_addr[MAX_MSG_SIZE];
	int idx;
}clients;

HANDLE mutex_for_player[NUM_OF_WORKER_THREADS];
clients clients_connected[NUM_OF_WORKER_THREADS];

int FindFirstUnusedThreadSlot(clients **clients_connected);
void CleanupWorkerThreads();
DWORD ServiceThread(SOCKET *t_socket);
void MainServer(s_args *s_item);
s_args *InitServerArgs(char logfile[], char server_port[]);

#endif

