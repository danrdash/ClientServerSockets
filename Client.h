//this module holds the declaration for all functions client side
#pragma once
#include "Thread.h"




typedef struct Client_Arguments {
	char logfile[MAX_PATH_FILE];
	int s_port;
	char username[MAX_USERNAME];
	char mode[6];
	const char file[MAX_PATH_FILE];
	HANDLE *MutexRcv;
	char buffer[256];
	FILE *f_pointer;
}c_args;


void MainClient(c_args *c_item);
static DWORD SendDataThread(LPVOID itemA);
static DWORD RecvDataThread(LPVOID itemA);
c_args *InitClientArgs(char logfile[], char server_port[], char username[], char mode[], char file[]);
static DWORD ListenThread(LPVOID itemA);
