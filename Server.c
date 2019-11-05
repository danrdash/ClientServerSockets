#include "Server.h"
#include "GenFuncs.h"

HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
SOCKET ThreadInputs[NUM_OF_WORKER_THREADS];

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int player = 0;
int turn = 0;
char clients_names[NUM_OF_WORKER_THREADS][MAX_USERNAME];

/*
Description - The main function of the server side. It allows the connection of the clients to the server and opens thread for both threads
Parameters - Two pointers to the strings which indicate the path of the logfile and the port of the server.
Return - Nothing
*/
void MainServer(char *logfilepath, char *port)

{
	int Ind;
	int Loop;
	SOCKET MainSocket = INVALID_SOCKET;
	unsigned long Address;
	SOCKADDR_IN service;
	int bindRes;
	int ListenRes;
	extern char clients_names[NUM_OF_WORKER_THREADS][MAX_USERNAME];
	int idx;
	for (idx = 0; idx < NUM_OF_WORKER_THREADS; idx++)
		clients_names[idx][0] = '\0';

	extern clients clients_connected[NUM_OF_WORKER_THREADS];

	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (StartupRes != NO_ERROR)
	{
		printf("error %ld at WSAStartup( ), ending program.\n", WSAGetLastError());
		// Tell the user that we could not find a usable WinSock DLL.                                  
		return 1;
	}
	/* The WinSock DLL is acceptable. Proceed. */


	// Create a socket.    
	MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (MainSocket == INVALID_SOCKET)
	{
		printf("Error at socket( ): %ld\n", WSAGetLastError());
		goto server_cleanup_1;
	}

	// Bind the socket.
	// Create a sockaddr_in object and set its values.
	// Declare variables

	Address = inet_addr(SERVER_ADDRESS_STR);
	if (Address == INADDR_NONE)
	{
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n",
			SERVER_ADDRESS_STR);
		goto server_cleanup_2;
	}

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = Address;
	service.sin_port = htons(atoi(port)); //The htons function converts a u_short from host to TCP/IP network byte order 
										  //( which is big-endian ).

										  // Call the bind function, passing the created socket and the sockaddr_in structure as parameters. 
										  // Check for general errors.
	bindRes = bind(MainSocket, (SOCKADDR*)&service, sizeof(service));
	if (bindRes == SOCKET_ERROR)
	{
		printf("bind( ) failed with error %ld. Ending program\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	// Listen on the Socket.
	ListenRes = listen(MainSocket, SOMAXCONN);
	if (ListenRes == SOCKET_ERROR)
	{
		printf("Failed listening on socket, error %ld.\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	// Initialize all thread handles to NULL, to mark that they have not been initialized
	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
		ThreadHandles[Ind] = NULL;

	printf("Waiting for a client to connect...\n");

	for (Loop = 0; Loop < NUM_OF_WORKER_THREADS; Loop++)
	{
		SOCKET AcceptSocket = accept(MainSocket, NULL, NULL);
		if (AcceptSocket == INVALID_SOCKET)
		{
			printf("Accepting connection with client failed, error %ld\n", WSAGetLastError()); //Perhaps we should change this error message
			goto server_cleanup_3;
		}

		printf("Client Connected.\n"); //Perhaps we should change this message

		Ind = FindFirstUnusedThreadSlot(&clients_connected);

		if (Ind == NUM_OF_WORKER_THREADS) //no slot is available
		{
			printf("No slots available for client, dropping the connection.\n");
			closesocket(AcceptSocket); //Closing the socket, dropping the connection.
		}
		else
		{
			ThreadInputs[Ind] = AcceptSocket; // shallow copy: don't close 
											  // AcceptSocket, instead close 
											  // ThreadInputs[Ind] when the
											  // time comes.
			clients_connected[Ind].server = AcceptSocket;
			strcpy(clients_connected[Ind].log_file_addr, logfilepath);
			//clients_connected[Ind].clients_names = &clients_names;
			clients_connected[Ind].idx = Ind;
			ThreadHandles[Ind] = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)ServiceThread,
				&(clients_connected[Ind]),
				0,
				NULL
			);
		}
	}
	WaitForMultipleObjects(NUM_OF_WORKER_THREADS, ThreadHandles, TRUE, INFINITE);


server_cleanup_3:

	CleanupWorkerThreads();

server_cleanup_2:
	if (closesocket(MainSocket) == SOCKET_ERROR)
		printf("Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());

server_cleanup_1:
	if (WSACleanup() == SOCKET_ERROR)
		printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());
	//We should transfer this command to free the allocated space to a separate function
	CloseHandle(ThreadHandles[0]);
	CloseHandle(ThreadHandles[1]);

	/* Trial */
	/*for(idx4 = 0; idx4 < 2; idx4++)
	free(clients_names[idx4]);*/

}

/*
Description - Finds the index of the client which connects to the server
Parameters - A pointer to the array of clients which holds the clients array structure.
Return - An integer which holds the index of the client
*/
static int FindFirstUnusedThreadSlot(clients **clients_connected)
{
	int Ind;

	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		if (ThreadHandles[Ind] == NULL)
			break;
		else
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject(ThreadHandles[Ind], 0);

			if (Res == WAIT_OBJECT_0) // this thread finished running
			{
				clients_connected[Ind]->clients_names = NULL;
				clients_connected[Ind]->idx = 0;
				closesocket(clients_connected[Ind]->server);
				CloseHandle(ThreadHandles[Ind]);
				ThreadHandles[Ind] = NULL;
				break;
			}
		}
	}

	return Ind;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

/*
Description - This function closes sockets and handles to threads
Parameters - Nothing
Return - Nothing
*/
static void CleanupWorkerThreads()
{
	int Ind;

	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		if (ThreadHandles[Ind] != NULL)
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject(ThreadHandles[Ind], INFINITE);

			if (Res == WAIT_OBJECT_0)
			{
				closesocket(ThreadInputs[Ind]);
				CloseHandle(ThreadHandles[Ind]);
				ThreadHandles[Ind] = NULL;
				break;
			}
			else
			{
				printf("Waiting for thread failed. Ending program\n");
				break;
			}
		}
	}
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Service thread is the thread that opens for each successful client connection and "talks" to the client.

/*
Description - This is the method of the threads. It waits for two threads to connect and starts the game by calling the function "Actual_Play"
Parameters - An instance of the data structure 'clients'
Return - 0 if successful
*/
static DWORD ServiceThread(clients *client)
{
	extern clients clients_connected[NUM_OF_WORKER_THREADS];
	extern char clients_names[NUM_OF_WORKER_THREADS][MAX_USERNAME];
	extern HANDLE mutex_for_player[NUM_OF_WORKER_THREADS];
	extern int player;
	extern int turn;
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	BOOL Done = FALSE;
	static int board[NUM_OF_ROWS][NUM_OF_COLUMNS];// This array stores the board's state
	static int max_row_for_column[NUM_OF_COLUMNS];
	static int idx4red;
	static int idx4yellow;
	static int SndScrn = 1;
	int color;
	int result;
	int state;
	int identifier;
	static int free_args = 1;
	char SendStr[SEND_STR_SIZE];
	char *message = NULL;
	char *args_rcvd = NULL;
	char message_from_player[MAX_MSG_SIZE];
	char send_msg[MAX_MSG_SIZE];
	char outmsg[MAX_MSG_SIZE] = "";
	char *AcceptedStr = NULL;
	char temp_char[MAX_MSG_SIZE];

	/* Initialize the board, max_row_for_column, client, messagem, and args_rcvd */
	initialization(board, max_row_for_column, &client, &message, &args_rcvd);
	initialize_mutex(client->idx);


	RecvRes = ReceiveString(&AcceptedStr, client->server);
	if (RecvRes == TRNS_FAILED)
	{
		printf("Service socket error while reading, closing thread.\n");
		closesocket(client->server);
		return 1;
	}
	else if (RecvRes == TRNS_DISCONNECTED)
	{
		printf("Connection closed while reading, closing thread.\n");
		closesocket(client->server);
		return 1;
	}

	/* Get the player's name */
	divide_message_for_server(&AcceptedStr, &args_rcvd, &message); // store the actual message from the client within message and the message's arguments within args_rcvd
	state = check_message(message); //Check the type of the message
	if (USR_RQST == state)
	{
		if (!check_name(args_rcvd, clients_names, client->idx))
		{
			WaitForSingleObject(mutex_for_player[client->idx], 0);
			player++;
			strcpy(clients_names[client->idx], args_rcvd);
			sprintf(SendStr, "NEW_USER_ACCEPTED:%d", player);
			if (player == 1) //if the player is the first client it gets the color red, otherwise (if he is the second client) he gets the color yellow
			{
				color = RED;
				idx4red = client->idx;
			}
			else
			{
				color = YELLOW;
				idx4yellow = client->idx;
			}
			ReleaseMutex(mutex_for_player[client->idx]);
		}
		else
		{
			strcpy(SendStr, "NEW_USER_DECLINED"); //In this case perhaps we should erase every item which will ultimately prevent from the client to try and connect to the server
			SendRes = SendString(SendStr, client->server);
		}
	}
	else
	{
		strcpy(SendStr, "Error: Illegal command\n"); //In this stage any other command is illegal - we should verify that
		SendRes = SendString(SendStr, client->server);
	}
	/* Send the appropriate message to the client */
	SendRes = SendString(SendStr, client->server);// we should add a function which checks every possible error
	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(client->server);
		return 1;
	}

	/* Wait for both clients to connect */
	while (player < NUM_OF_WORKER_THREADS)//change to semaphore!!!!!!!!!! - Maybe?!?!?!?!?!?
		;
	/* Inform all palyers that the game started */
	strcpy(SendStr, "GAME_STARTED");
	SendRes = SendString(SendStr, client->server);
	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(client->server);
		return 1;
	}


	strcpy(SendStr, "BOARD_VIEW:0;0;0");
	SendRes = SendString(SendStr, client->server);

	/* Inform both clients that it is the first player's turn */
	sprintf(SendStr, "TURN_SWITCH:%s", clients_names[turn]); //Inform all threads that it's the first player's turn!!!
	SendRes = SendString(SendStr, client->server);

	/* Practically, start the game! */
	Actual_Play(client, color, board, max_row_for_column, idx4red, idx4yellow);

	printf("Conversation ended.\n");


	WaitForSingleObject(mutex_for_player[client->idx], INFINITE);
	if (free_args) {
		free_args--;
		closesocket(clients_connected[0].server);
		closesocket(clients_connected[1].server);
		free(AcceptedStr);
		free(message);
		free(args_rcvd);
		free(client->clients_names);
	}
	ReleaseMutex(mutex_for_player[client->idx]);
	return 0;
}

/*
Description - This function manages the game.
Parameters - An instance of the data structure 'clients', the client's corresponding color, the board array, the array which holds the maximal row for each column, the serial number of the red player and the serial number of the yellow player.
Return - 0 if successful
*/
int Actual_Play(clients *client, int color, int board[NUM_OF_ROWS][NUM_OF_COLUMNS], int max_row_for_column[NUM_OF_COLUMNS], int idx4red, int idx4yellow)
{
	extern char clients_names[NUM_OF_WORKER_THREADS][MAX_USERNAME];
	extern HANDLE mutex_for_player[NUM_OF_WORKER_THREADS];
	static BOOL Done = FALSE;
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	char *AcceptedStr = NULL;
	AcceptedStr = (char*)malloc(MAX_MSG_SIZE * sizeof(char));
	char *message = NULL;
	message = (char*)malloc(MAX_MSG_SIZE * sizeof(char));
	message[0] = '\0';
	char *args_rcvd = NULL;
	args_rcvd = (char*)malloc(MAX_MSG_SIZE * sizeof(char));
	args_rcvd[0] = '\0';

	int result;
	int state;
	char SendStr[SEND_STR_SIZE];
	//char send_msg[MAX_MSG_SIZE];
	char *send_msg = NULL;
	send_msg = (char*)malloc(MAX_MSG_SIZE * sizeof(char));
	send_msg[0] = '\0';
	//char outmsg[MAX_MSG_SIZE];
	char *outmsg = NULL;
	outmsg = (char*)malloc(MAX_MSG_SIZE * sizeof(char));
	outmsg[0] = '\0';
	char message_from_player[MAX_MSG_SIZE];
	int play_buff[TOT_NO_TURNS];
	static buff_idx = 0;
	extern int turn;
	int accumulated_turns = 1;
	int success = 0;
	FILE *pointA = NULL;
	static int close = 1;

	while (!Done) {
		/* Receive a message from a client and check wheather it matches any predefined type of message */
		AcceptedStr = NULL;
		RecvRes = ReceiveString(&AcceptedStr, client->server); //Receive the column to which the server should add the corresponding color
		if (RecvRes == TRNS_FAILED)
			break;
		if (RecvRes == TRNS_DISCONNECTED)
		{
			//write to logfile
			fopen_s(&pointA, client->log_file_addr, "a");
			fprintf(pointA, "Player disconnected. Ending communication."); //we should write to the log file "Player disconnected. Ending communication."
			fclose(pointA);
			break;
		}

		divide_message_for_server(&AcceptedStr, &args_rcvd, &message);
		state = check_message(message);
		if (SEND_MSG == state)
		{
			//get_message_from_client(args_rcvd, &send_msg);
			receive_msg(args_rcvd, &outmsg, clients_names[client->idx]); // The current client sent a message which should be sent to the other client
			SendRes = SendString(outmsg, clients_connected[((client->idx) + 1) % 2].server); //Send the message to the other client
			outmsg[0] = '\0';
			args_rcvd[0] = '\0';
			message[0] = '\0';
			continue;
		}

		/* The client should play if it is his/her turn */
		else if (PLAY_RQST == state) {
			if (turn != client->idx) {
				strcpy(SendStr, "Error: Not your turn");
				SendRes = SendString(SendStr, client->server);
				continue;
			}
			else
			{
				WaitForSingleObject(mutex_for_player[client->idx], 0);
				result = play_the_game(board, max_row_for_column, atoi(args_rcvd), color);
				state = check_for_game_result(result, &Done, &client, max_row_for_column, args_rcvd, color);
				ReleaseMutex(mutex_for_player[client->idx]);
				if ((NOT_YET_WON != state) && (ILLEGAL_MOVE != state))
				{
					if (RED == state)
						sprintf(SendStr, "GAME_ENDED:%s", clients_names[idx4red]); //we should implement something which tells all threads that the game is over with a winner
					else
						sprintf(SendStr, "GAME_ENDED:%s", clients_names[idx4yellow]);
					//SendRes = SendString(SendStr, client->server);
					SendRes = SendString(SendStr, clients_connected[turn].server);
					if (TRNS_SUCCEEDED == SendRes)
						success++;
					SendRes = SendString(SendStr, clients_connected[NUM_OF_WORKER_THREADS - turn - 1].server);
					if (TRNS_SUCCEEDED == SendRes)
						success++;
					if (2 == success)
					{
						Done = TRUE;
						break;
					}
					else
					{
						exit(1); //an error occured
					}
				}
				else
				{
					if (NOT_YET_WON == state)
					{
						;//show the board to both clients
					}
					else
					{
						strcpy(SendStr, "Error: Illegal move");//An unwanted message appeared and we should handle it appropriately
						SendRes = SendString(SendStr, client->server);
						continue;
					}
				}
			}
		}

		else
		{
			strcpy(SendStr, "Error: Illegal command");//An unwanted message appeared and we should handle it appropriately
			SendRes = SendString(SendStr, client->server);
			continue;
		}

		turn = (turn + 1) % 2;
		if (TOT_NO_TURNS == accumulated_turns)
			Done = 1;
		else
			accumulated_turns++;
		sprintf(SendStr, "TURN_SWITCH:%s", clients_names[turn]); //Store the index to be changed within SendStr in te format "row;column;color". At the beginning there is no change in the board
		SendRes = SendString(SendStr, clients_connected[turn].server);
		SendRes = SendString(SendStr, clients_connected[NUM_OF_WORKER_THREADS - turn - 1].server);

	}
	if (close == 1)
	{
		close--;
		free(send_msg);
		free(outmsg);
		free(message);
		free(args_rcvd);
	}
	return 0;
}

