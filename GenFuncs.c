//this module takes care of general funtions needed for client and server sides.
#include "GenFuncs.h"
#include "Thread.h"
#include "Client.h"
#include <tchar.h>


//Description - this function writes to logfile, and takes care of mutex
//Parameters - c_args structure pointer strin to write and a flag for whom sent this message
//Return - NULL
void WriterFunc(c_args *item, char *string, int send_rcv)
{
	FILE *fp = NULL;
	WaitForSingleObject(*(item->MutexRcv), INFINITE);
	fopen_s(&fp, item->logfile, "a");
	if (send_rcv == SEND)
		fprintf(fp, "Sent to server: %s\n", string);
	else if (send_rcv == RECIEVE)
		fprintf(fp, "Recieved from server: %s\n", string);
	else
		fprintf(fp, string);
	fclose(fp);
	ReleaseMutex(*(item->MutexRcv));
	return NULL;
}
//Description - when a message is inserted it breaks the message into the format told by the project exercize
//Parameters - string
//Return - string
char *BreakMsg(char *string)
{
	char *output = (char*)malloc(256 * sizeof(char)), *buffer = (char*)malloc(256 * sizeof(char));
	char *token = NULL;
	int i = 0;
	strcpy(buffer, string);
	token = strtok(buffer, " ");
	strcpy(output, "");
	strcat(output, "SEND_MESSAGE:");
	
	//strcat(output, token);
	while (token != NULL)
	{
		token = strtok(NULL, " ");
		if (token == NULL)
			break;
		if (i!=0)
		strcat(output, "; ;");
		strcat(output, token);
		i++;
	}
	//printf("%s", output);
	free(buffer);
	return output;

}
//Description - when a message is inserted it breaks the message into the format told by the project exercize in REVERSE to BREAKMSG above
//Parameters - string
//Return - string
char *OpenMsg(char * string) // puts a msg into  ; ; ; ; ;; type msg
{
	char *output = (char*)malloc(256 * sizeof(char)), *buffer = (char*)malloc(256 * sizeof(char));
	char *token = NULL, *p;
	int i = 0;
	strcpy(output, "");
	strcpy(buffer, string);
	
	token = strtok(buffer, ":");
	token = strtok(NULL, ";"); //now the token is the username
	strcat(output, token);
	strcat(output, ": "); // we got username
	while (token != NULL)
	{
		token = strtok(NULL, ";");
		if (token == NULL)
		{
			break;
		}
		i++;
		//strcat(output, " ");
		strcat(output, token);
		
	}
	//printf("%s", output);
	free(buffer);
	return output;
}
//Description - prints the board
//Parameters - double int array for board, and a handle for console
//Return - NULL
void PrintBoard(int board[][BOARD_WIDTH], HANDLE consoleHandle)
{
	WORD saved_attributes;
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	/* Save current attributes */
	

	int row, column;
	GetConsoleScreenBufferInfo(consoleHandle, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;
	//Draw the board
	for (row = 0; row < BOARD_HEIGHT; row++)
	{
		for (column = 0; column < BOARD_WIDTH; column++)
		{
			printf("| ");
			if (board[row][column] == RED_PLAYER)
				SetConsoleTextAttribute(consoleHandle, 204);

			else if (board[row][column] == YELLOW_PLAYER)
				SetConsoleTextAttribute(consoleHandle, 238);

			printf("O");

			SetConsoleTextAttribute(consoleHandle, BLACK);
			printf(" ");
		}
		printf("\n");

		//Draw dividing line between the rows
		for (column = 0; column < BOARD_WIDTH; column++)
		{
			printf("----");
		}
		printf("\n");
	}
	SetConsoleTextAttribute(consoleHandle, saved_attributes);
	return NULL;

}
//Description - updates the board according to board_view message.
//Parameters - double array int board and the board view message
//Return - 0
int UpdateBoard(int board[][BOARD_WIDTH], char buff[])
{
	char *tok = NULL;
	int row = 0, col = 0, player = 0, i=0;
	tok = strtok(buff, ":");
	tok = strtok(NULL, ";\0");
	row = atoi(tok);
	tok = strtok(NULL, ";\0");
	col = atoi(tok);
	tok = strtok(NULL, ";\0");
	player = atoi(tok);
	board[6-row][col] = player;
	return 0;
}
//Description - writes to file that a program error happened and exits
//Parameters - string for logfile path
//Return - 0
int ErrorPlan(char *outpath)
{
	FILE *fp = NULL;
	fopen_s(&fp, outpath, "w");
	fprintf(fp, "An error occured, program had to exit with exitCode 1. Thanks for running our code, Dan&Ori - xoxo <3");
	fclose(fp);
	exit(1);
	return 0;
}
//Description - aquires username from human or from file depending on the arguments we recieved from user
//Parameters - destination to put the file into, logfile path and wether we are human or file mode
//Return - 0
int AquireUsername(char dest[], char filepath[], char mode[]) {
	if (strcmp(mode, "human") == 0) // this is human mode
	{
		printf("Please enter your username:\n");
		scanf("%s", dest);
		getchar(stdin);
		
		//SEND username to server
	}
	else if (strcmp(mode, "file") == 0) //read from file
	{

		ReadUserFromInput(dest, filepath);

		//SEND username to server
	}
	return 0;
}
//Description - reads the username from file mode
//Parameters - destination for username, and file to read from
//Return - 0
int ReadUserFromInput(char dest[], char filepath[])
{
	FILE *fp = NULL;
	char *p = NULL;
	fopen_s(&fp, filepath, "r");
	fgets(dest, MAX_USERNAME+20, fp);
	p = strchr(dest, '\n');
	*p = '\0';
	printf("%s\n", dest);
	fseek(fp, 0, SEEK_SET);
	fclose(fp);
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Description - Check whether the requested name is available
Parameters - A string of the requested client's name and the array of the already exist names
Return - returns 0 if succeeded
*/
int check_name(char name[MAX_USERNAME], char names[NUM_OF_WORKER_THREADS][MAX_USERNAME], int idx)
{
	//if (!strcmp(names[idx], "")) //this field may be initialized to garbage and we should not worry about it
	if (!strcmp(name, names[NUM_OF_WORKER_THREADS - idx - 1]))
		return 1; //The name already exists and should be changed
	return 0;
}

/*
Description - Divide a string to the message type and the message's arguments
Parameters - An input parameter of the string to divide, an output parameter of the parameters, and an output parameter of the message type
Return - returns 0 if succeeded
*/
int divide_message_for_server(char **message_to_divide, char **parameters, char **message_to_output)
{
	char delim[2] = ":";
	char *token;
	char temp_message[MAX_MSG_SIZE] = "";
	token = strtok(*message_to_divide, delim);

	//*message_to_output = &temp_message;
	if (NULL != token)
		strcpy(*message_to_output, token);
	if (NULL != token)
	{
		token = strtok(NULL, delim);
		if (NULL != token)
			strcpy(*parameters, token);
	}
	//*parameters = &temp_message;
	return 0;
}

/*
A function from the example
*/
int check_sent_msg(TransferResult_t SendRes, SOCKET *server)
{
	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(server);
		return 1;
	}
}

/*
Description - This function adds a color to the requested column on the screen
Parameters - The board's representation as a matrix, a one dimensional array which states the maximal row filled on a particular column, an integer which represents the requested column, and an integer which represents the color of the player
Return - returns the result of check_for_winning_pattern
*/
int play_the_game(int board[NUM_OF_ROWS][NUM_OF_COLUMNS], int max_row_4_clmns[NUM_OF_COLUMNS], int column, int color)
{
	int winner;
	char msg[MAX_MSG_SIZE];
	TransferResult_t SendRes;
	msg[0] = '\0';
	if (column < NUM_OF_COLUMNS && column >= 0) {
		if (max_row_4_clmns[column] < NUM_OF_ROWS)
		{
			board[NUM_OF_ROWS - max_row_4_clmns[column] - 1][column] = color; //set the color in the appropriate entry
			max_row_4_clmns[column]++; //increase the row in this column by 1;
			winner = check_for_winning_pattern(board);
			return winner;
		}
		else
			return ILLEGAL_MOVE;
	}
	else
		return ILLEGAL_MOVE;
}

/*
Description - This function checks on every turn if a winning pattern appears on screen
Parameters - The matrix representation of the board
Return - Returns the color of the player in case of winning, and a value of -1 represented by NOT_YET_WON, to indicate that there is no winner on this stage
*/
int check_for_winning_pattern(int board[NUM_OF_ROWS][NUM_OF_COLUMNS])
{
	int row;
	int column;

	for (row = 0; row < NUM_OF_ROWS; row++) {
		for (column = 0; column < NUM_OF_COLUMNS; column++)
		{
			/* CHECK WITHIN THE ROWS */
			if ((board[row][column] == RED || board[row][column] == YELLOW) && (board[row][column] == board[row][column + 1]) && (board[row][column + 1] == board[row][column + 2]) && (board[row][column + 2] == board[row][column + 3]))
			{
				return board[row][column];
			}
			/* CHECK WITHIN THE COLUMNS */
			if (row < NUM_OF_ROWS - INDENTATION + 1)
			{
				if ((board[row][column] == RED || board[row][column] == YELLOW) && (board[row][column] == board[row + 1][column]) && (board[row + 1][column] == board[row + 2][column]) && (board[row + 2][column] == board[row + 3][column]))
					return board[row][column];
			}
			/* CHECK WITHIN THE DIAGONALS */
			if ((row + INDENTATION <= NUM_OF_ROWS) && (column + INDENTATION <= NUM_OF_COLUMNS))
			{
				if ((board[row][column] == RED || board[row][column] == YELLOW) && (board[row][column] == board[row + 1][column + 1]) && (board[row + 1][column + 1] == board[row + 2][column + 2]) && (board[row + 2][column + 2] == board[row + 3][column + 3]))
					return board[row][column];
			}
			/* CHECK WITHIN THE SECONDARY DIAGONALS */
			if ((row - INDENTATION >= 0) && (column + INDENTATION <= NUM_OF_COLUMNS))
			{
				if ((board[row][column] == RED || board[row][column] == YELLOW) && (board[row][column] == board[row - 1][column + 1]) && (board[row - 1][column + 1] == board[row - 2][column + 2]) && (board[row - 2][column + 2] == board[row - 3][column + 3]))
					return board[row][column];
			}
		}
	}
	return NOT_YET_WON;
}

/*
Description - This function checks the result of the game after a player made a move.
Parameters - The result of the function play_the_game, the state of the condition of exiting the loop within ServiceThread (output parameter),
the instance of the data structure of the client which last played, an array ehich states the highest full row on a given column,
the arguments received from the client (the requested column), and the color of the current player.
Return - Returns the color of the winner if there is a winner, NOT_YET_WON if there is no winner as of yet, and ILLEGAL_MOVE if an illegal move was made.
*/
int check_for_game_result(int result, int *Done, clients *client, int max_row_for_column[NUM_OF_COLUMNS], char args_rcvd[MAX_MSG_SIZE], int color)
{
	extern int player;
	extern int turn;
	extern clients clients_connected[NUM_OF_WORKER_THREADS];
	char SendStr[SEND_STR_SIZE];
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	if (ILLEGAL_MOVE == result) {
		strcpy(SendStr, "Error: Illegal move");
		SendRes = SendString(SendStr, client->server);
		return ILLEGAL_MOVE;
	}
	else
	{
		//PLAY_DECLINED
		strcpy(SendStr, "PLAY_ACCEPTED");
		SendRes = SendString(SendStr, clients_connected[turn].server);
		sprintf(SendStr, "BOARD_VIEW:%d;%d;%d", max_row_for_column[atoi(args_rcvd)], atoi(args_rcvd), color); //Store the index to be changed within SendStr in te format "row;column;color". At the beginning there is no change in the board
		SendRes = SendString(SendStr, clients_connected[turn].server);
		SendRes = SendString(SendStr, clients_connected[NUM_OF_WORKER_THREADS - turn - 1].server);
		if (RED == result) {
			*Done = TRUE;
			return RED;
		}
		else if (YELLOW == result) {
			*Done = TRUE;
			return YELLOW;
		}
		else if (NOT_YET_WON == result)
			return NOT_YET_WON;
	}
	return 0;
}

/*
Description - This function checks the type of the message sent by a client.
Parameters - A string input parameter which holds the message of the client.
Return - An int indicating the type of the message
*/
int check_message(char message[MAX_MSG_SIZE])
{
	if (!strcmp(message, "NEW_USER_REQUEST"))
		return USR_RQST;
	else if (!strcmp(message, "PLAY_REQUEST"))
		return PLAY_RQST;
	else if (!strcmp(message, "SEND_MESSAGE"))
		return SEND_MSG;
	else if (!strcmp(message, "RECEIVE_MESSAGE"))
		return RCV_MSG;
	else
		return ILLEGAL_COMMAND;
}


//int get_message_from_client(char *args_rcvd, char **message)
//{
//	int counter = 0;
//	char delim[2] = ";";
//	char *token = NULL;
//	char temp_message[MAX_MSG_SIZE];
//	token = strtok(args_rcvd, delim);
//
//	while (token != NULL) {
//		strcpy(temp_message, token);
//		if (0 != counter)
//			strcat(*message, " ");
//		strcat(*message, temp_message);
//		token = strtok(NULL, delim);
//		counter++;
//	}
//	return 0;
//}

/*
Description - This function prepares the message that should be sent to the other client, it concatenates "RECEIVE_MESSAGE:" with the name of the player and the actual message to be delivered to the other client.
Parameters - Two input parameters in the form of a string containing the message to be sent and the name of the client, and one output parameter which holds the actual message after concatenation.
Return - 0 if successful
*/
int receive_msg(char *message, char **outmsg, char name[MAX_USERNAME])
{
	char temp_str[MAX_MSG_SIZE] = "";
	strcat(*outmsg, "RECEIVE_MESSAGE:");
	strcat(*outmsg, name);
	strcat(*outmsg, ";");
	strcat(*outmsg, message);
	return 0;
}


/*
Description - This function initializes the board to zeroes and the maximal row which holds a value for a given column, the clients data structure, and allocates memory to both message and args_rcvd. 
Parameters - The array of the board, the array of the maximal row for each column, the array of clients data structure, a pointer to the message, and a pointer to the string args_rcvd
Return - 0 if successful
*/
int initialization(int board[NUM_OF_ROWS][NUM_OF_COLUMNS], int max_row_for_column[NUM_OF_COLUMNS], clients **client, char **message, char **args_rcvd)
{
	/* Initialize the board to an array of zeroes */
	int idx1, idx2;
	for (idx1 = 0; idx1 < NUM_OF_ROWS; idx1++)
		for (idx2 = 0; idx2 < NUM_OF_COLUMNS; idx2++)
			board[idx1][idx2] = 0;
	/* Initialize the array whiche stores the maximal row in use for each column to zeroes */
	for (idx1 = 0; idx1 < NUM_OF_COLUMNS; idx1++)
		max_row_for_column[idx1] = 0;
	(*client)->clients_names = (char**)malloc(NUM_OF_WORKER_THREADS * sizeof(char*));
	for (idx1 = 0; idx1 < NUM_OF_WORKER_THREADS; idx1++) {
		((*client)->clients_names)[idx1] = (char*)malloc(MAX_MSG_SIZE * sizeof(char));
		((*client)->clients_names)[idx1] = "";
	}
	(*message) = (char *)malloc(MAX_MSG_SIZE * sizeof(char));
	(*args_rcvd) = (char *)malloc(MAX_MSG_SIZE * sizeof(char));
	(*message)[0] = '\0';
	(*args_rcvd)[0] = '\0';
	return 0;
}

/*
Description - This function initializes a mutex for each thread on the server side
Parameters - An integer which holds the index of the mutex that should be initialized
Return - 0 if successful
*/
void initialize_mutex(int idx)
{
	extern HANDLE mutex_for_player[NUM_OF_WORKER_THREADS];

	mutex_for_player[idx] = CreateMutex(
		NULL,   /* default security attributes */
		FALSE,	/* don't lock mutex immediately */
		_T("%d", idx)	/* Set the mutex name to the specified index */
	);

}
