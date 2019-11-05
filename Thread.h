//this header consists of main definitions and includes important libraries, the MOTHER header. included in everything. Thread is an arbitrary name
#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#define MAX_PATH_FILE 30
#define MAX_USERNAME 100
#define MAX_MSG_SIZE 100


// board defines
#define RED_PLAYER 4
#define YELLOW_PLAYER 5
#define FULL 1
#define EMPTY 0
#define BOARD_HEIGHT 6
#define BOARD_WIDTH  7
#define SEND 1
#define RECIEVE 2

#define BLACK  15
//#define RED    204
//#define YELLOW 238


#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <stdbool.h>
#include <Windows.h>
#include "SocketExampleShared.h"
#include "SocketSendRecvTools.h"
