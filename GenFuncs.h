//this module holds the declaration for all general functions of client side and server side
#pragma once
#include "Thread.h"
#include "Client.h"
#include "Server.h"

int ErrorPlan(char *outpath);
int ReadUserFromInput(char dest[], char filepath[]);
int AquireUsername(char dest[], char filepath[], char mode[]);
void PrintBoard(int board[][BOARD_WIDTH], HANDLE consoleHandle);
int UpdateBoard(int board[][BOARD_WIDTH], char buff[]);
char *BreakMsg(char *string);
char *OpenMsg(char *string);
void WriterFunc(c_args *item, char *string, int send_rcv);



int ErrorPlan(char *outpath);
int check_name(char name[MAX_USERNAME], char names[NUM_OF_WORKER_THREADS][MAX_USERNAME], int idx);
int divide_message_for_server(char **message_to_divide, char **parameters, char **message_to_output);
int check_sent_msg(TransferResult_t SendRes, SOCKET *server);
int play_the_game(int board[NUM_OF_ROWS][NUM_OF_COLUMNS], int max_row_4_clmns[NUM_OF_COLUMNS], int column, int color);
int check_for_winning_pattern(int board[NUM_OF_ROWS][NUM_OF_COLUMNS]);
int check_for_game_result(int result, int *Done, clients *client, int max_row_for_column[NUM_OF_COLUMNS], char args_rcvd[MAX_MSG_SIZE], int color);
int check_message(char message[MAX_MSG_SIZE]);
int receive_msg(char *message, char *outmsg, char name[MAX_USERNAME]);
int send_screen(int board[NUM_OF_ROWS][NUM_OF_COLUMNS]);
int initialization(int board[NUM_OF_ROWS][NUM_OF_COLUMNS], int max_row_for_column[NUM_OF_COLUMNS], clients **client, char **message, char **args_rcvd);
void initialize_mutex(int idx);
