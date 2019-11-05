//This module consists of the functions dedicated for client side

#include "Thread.h"
#include "Client.h"
#include "GenFuncs.h"





//declare global socket variable
SOCKET m_socket; 
char Input[256] = { "\n" };
int valid = 0;
int terminate = FALSE;
int buff_signal = EMPTY;
int the_end = 0; //This parameters indicates that 'GAME_ENDED' was reached and thus the clients should exit
///////////////


//Description - takes the arguments of the file and inserts them into a structure
//Parameters - logfile server poert username mode and file all strings
//Return - c_args structure 
c_args *InitClientArgs(char logfile[], char server_port[], char username[], char mode[], char file[])
{
	c_args  *item;
	item = (c_args*)malloc(sizeof(c_args));
	strcpy(item->logfile, logfile);
	item->s_port = atoi(server_port);
	strcpy(item->username, username);
	strcpy(item->mode, mode);
	if (NULL !=file)
	strcpy(item->file, file);
	return item;
}

//Description - listens in for the input, either human or file.
//Parameters - LPVOID Item, which is c_args type structure which we cast later
//Return - 0
static DWORD ListenThread(LPVOID itemA)
{
	
	
	FILE *fp = NULL;
	errno_t t;
	int i = 0;
	extern int terminate;
	c_args *item = (c_args*)itemA;
	char c = 'c', *p=NULL;
	extern char Input[256];
	char buffer[256];
	extern int valid; //here valid is 0
	extern int the_end;
	if (strcmp(item->mode, "file") == 0) // file mode
	{
		t = fopen_s(&fp, item->file, "r");
		if (fp == NULL || t != 0)
			printf("help");
		while (1) //read from file
		{
			if (terminate == TRUE)
			{
				fclose(fp);
				return 0x555;
			}
			if (i == 0) {
				fgets(Input, 256, fp);
				p = strchr(Input, '\n');
				*p = '\0';
				i++;
			}//If this is the first run, we want to skip reading the username
			if (valid == 1)
			{
				//if (feof(fp)) break;

				continue;
			}
			p = fgets(Input, 256, fp);
			if (NULL == p)
				break;
			p = strchr(Input, '\n');
			*p = '\0';
			valid = 1;
			if (feof(fp))break;
			if (1 == the_end)
				break;
		}
		fclose(fp);
		
	}
	else {
		while (1)
		{
			if (terminate == TRUE)
			{
				return 0;
			}
			if (valid == 1) //if valid is enabled, don't listen sendthread is working
				continue;
			if (terminate == TRUE)
			{
				return 0;
			}
			if (terminate == TRUE)
			{
				return 0;
			}
			if (terminate == TRUE)
			{
				return 0;
			}
			if (terminate == TRUE)
			{
				return 0;
			}
			if (terminate == TRUE)
			{
				return 0;
			}
			if (terminate == TRUE)
			{
				return 0;
			}
			if (terminate == TRUE)
			{
				return 0;
			}
			if (terminate == TRUE)
			{
				return 0;
			}
			if (terminate == TRUE)
			{
				return 0;
			}
			if (terminate == TRUE)
			{
				return 0;
			}

			gets_s(Input, sizeof(Input));
			
			valid = 1;
			if (1 == the_end)
				return 0;
			if (terminate == TRUE)
			{
				return 0;
			}
		}
	}
	return 0;
}
//Description - recieved data from server and acts accoridngly
//Parameters - LPVOID Item, which is c_args type structure which we cast later
//Return - 0
static DWORD RecvDataThread(LPVOID itemA)
{
	HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	extern int board[BOARD_HEIGHT][BOARD_WIDTH];
	extern int the_end, buff_signal;
	extern int terminate;
	TransferResult_t RecvRes;
	c_args *item = (c_args*)itemA;
	char SendStr[256], *buffer = NULL, *tok = NULL, *output=NULL, pre_board[256], *msg =NULL;
	
	while (1)
	{
		if (terminate == TRUE)
		{
			free(output);
			free(buffer);
			return 0;
		}
		char *AcceptedStr = NULL;
		RecvRes = ReceiveString(&AcceptedStr, m_socket);
		buffer = (char*)malloc(256 * sizeof(buffer));
		output = (char*)malloc(256 * sizeof(char));
		output[0] = '\0';
		strcpy(buffer, AcceptedStr);
		//printf("Got string:%s\n", AcceptedStr);
		////////////////THERE COULD BE NO ; //////////////////
		tok = strtok(buffer, ":");
	
		if (strcmp("PLAY_ACCEPTED", buffer) == 0)
		{
			printf("Well played\n");
			WriterFunc(item, AcceptedStr, RECIEVE);
			
		}
		else if (strcmp(tok, "PLAY_DECLINED")==0)
		{
			tok = strtok(NULL, ":");
			printf("Error: %s\n", tok);
			WriterFunc(item, AcceptedStr, RECIEVE);
			

		}
		else if (strcmp(tok, "GAME_STARTED")==0)
		{
			printf("Game is on!\n");
			WriterFunc(item, AcceptedStr, RECIEVE);
		
			
		}
		else if (strcmp(tok, "BOARD_VIEW")==0)
		{
			strcpy(pre_board, AcceptedStr);
			UpdateBoard(board, pre_board);
			PrintBoard(board, hConsole);
			
			WriterFunc(item, AcceptedStr, RECIEVE);
		}
		else if (strcmp(tok, "RECEIVE_MESSAGE") == 0)
		{
			WriterFunc(item, AcceptedStr, RECIEVE); ///////////
			msg = OpenMsg(AcceptedStr);
			printf("%s\n", msg);
			
			
		}
		else if (strcmp(tok, "TURN_SWITCH")==0)
		{
			tok = strtok(NULL, ":");
			strcat(output, tok); //output now contains username
			printf("%s's turn\n", output);
			WriterFunc(item, AcceptedStr, RECIEVE);
			
		}
		else if (strcmp(tok, "GAME_ENDED") == 0)
		{
			tok = strtok(NULL, ":");
			if (strcmp(tok, "Tie") == 0)
				printf("Game Ended. Everybody wins!\n");
			else
				printf("Game Ended. The winner is %s!\n", tok);
			WriterFunc(item, AcceptedStr, RECIEVE);
			free(AcceptedStr);
			the_end = 1;
			break;
		}
	
	


		if (RecvRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
		else if (RecvRes == TRNS_DISCONNECTED)
		{
			printf("Server closed connection. Bye!\n");
			return 0x555;
		}
		else
		{
			free(buffer);
			free(output);
			//printf("%s\n", AcceptedStr);
			//WriterFunc(item, AcceptedStr, RECIEVE);
			continue;
		}
		output[0] = '\0';
		free(AcceptedStr);
		//free(msg);
	}
	CloseHandle(hConsole);
	return 0;
}


//Description - sends data to server which is recieved from listne thread
//Parameters - LPVOID Item, which is c_args type structure which we cast later
//Return - 0
static DWORD SendDataThread(LPVOID itemA) //need to implement file reading capabilities
{
	extern char Input[256];
	extern int valid;
	extern int the_end, buff_signal, terminate;
	char SendStr[256], *buffer=(char*)malloc(256*sizeof(char)), *tok=NULL, *output = (char*)malloc(256 * sizeof(char)), *sender = NULL;
	char buff[10][256];
	c_args *item = (c_args*)itemA;
	TransferResult_t SendRes=1;
	
	strcpy(SendStr, Input);
	
	while (1)
	{
		//gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard
		
		
		//if (strcmp(SendStr, "") == 0) continue;
		if (valid == 0)
			continue;
		
		strcpy(SendStr, Input);
		strcpy(buffer, SendStr);
		tok = strtok(buffer, " ");
	
		if (strcmp(tok, "play")==0)
		{
			strcpy(output, "PLAY_REQUEST:");
			SendRes = SendString(strcat(output,tok = strtok(NULL, " " )), m_socket); // send column number
			WriterFunc(item, output, SEND);
			valid = 0;
		}
		else if (strcmp(tok, "message")==0)
		{
			strcpy(buffer, SendStr);
			sender = BreakMsg(buffer, SEND);
			SendRes = SendString(sender, m_socket); //send msg
			WriterFunc(item, sender, SEND);
			free(sender);
			valid = 0;
		}
		else if (strcmp(tok, "exit")!=0)
		{
			printf("Error: Illegal command\n");
		
			//SendRes = SendString(SendStr, m_socket);
			WriterFunc(item, "Error: Illegal command\n" , 5);
			valid = 0;
		}

		if (STRINGS_ARE_EQUAL(SendStr, "exit"))
		{
			shutdown(m_socket, SD_RECEIVE);
			
			free(output);
			free(buffer);
			valid = 0;
			terminate = TRUE;
			return 0; //"quit" signals an exit from the client side
		}
		

		if (SendRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
		if (1 == the_end)
			break;
	}
	free(buffer);
	return 0;
}
//Description - client start up, takes care of conneting with the server and opening the other threads
//Parameters - c_args structure pointer
//Return - NULL
void MainClient(c_args *c_item)
{
	
	SOCKADDR_IN clientService;
	HANDLE hThread[2];
	int res = 0;
	char SendStr[256];
	char buffy[256];
	char **MsgArray = NULL;
	char *RcvStr = NULL, *token=NULL;
	WSADATA wsaData; //create a WSADATA object
	char username[MAX_USERNAME];
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error in WSAStartup");
	FILE *fp = NULL;
	HANDLE MutexRcv;
	MutexRcv = CreateMutex(NULL, FALSE, "1");
	c_item->MutexRcv = &MutexRcv;
	//WriterFunc(c_item, "dude fuck this shit");
	if ((NULL == MutexRcv))
	{
		printf("Error creating mutexes\n");
		ErrorPlan(c_item->logfile);
		exit(1);
	}
	c_item->f_pointer = fp;
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	
	//Create a sockaddr_in object clientService and set  values.
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(SERVER_ADDRESS_STR); //Setting the IP address to connect to
	clientService.sin_port = htons(c_item->s_port); //Setting the port to connect to.
	//AF_INET is the Internet address family.



	// Call the connect function, passing the created socket and the sockaddr_in structure as parameters. 
	// Check for general errors.
	res = connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService));
	//printf("This error happened %d\n", WSAGetLastError());
	if (res == SOCKET_ERROR) {
		printf("Failed connecting to server on 127.0.0.1:%d. Exiting\n", c_item->s_port);
		sprintf(buffy, "Failed connecting to server on 127.0.0.1:%d. Exiting\n", c_item->s_port);
		WriterFunc(c_item, buffy, 5);
		WSACleanup();
		ErrorPlan(c_item->logfile);
		exit(1);
		return;
	}
	sprintf(buffy, "Connected to server on 127.0.0.1:%d", c_item->s_port);
	printf("Connected to server on 127.0.0.1:%d\n", c_item->s_port);
	WriterFunc(c_item, buffy, 5);
	AquireUsername(username, c_item->file, c_item->mode);

	strcpy(SendStr, "NEW_USER_REQUEST:");
	strcat(SendStr, username);
	WriterFunc(c_item, SendStr, 5);
	SendString(SendStr, m_socket);
	ReceiveString(&RcvStr, m_socket);
	token = strtok(RcvStr, ":");
	if (strcmp(token, "NEW_USER_DENIED")==0) {
		printf("Request to join was refused");
		ErrorPlan(c_item->logfile);
	}
	else if (strcmp(token, "NEW_USER_ACCEPTED")==0)
	{
		token = strtok(RcvStr, ":");
		if (2 == atoi(token))
			printf("You are Yellow!\n");
		else printf("You are Red!\n");
	}

	
	

	hThread[1] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)RecvDataThread,
		c_item,
		0,
		NULL
	);
	hThread[2] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ListenThread,
		c_item,
		0,
		NULL
	);
	/*hThread[3] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)WriterThread,
		c_item,
		0,
		NULL
	);*/
	hThread[0] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)SendDataThread,
		c_item,
		0,
		NULL
	);
	
	WaitForSingleObject(hThread[0], INFINITE);
	WaitForSingleObject(hThread[2], INFINITE);
	//WaitForMultipleObjects(3, hThread, TRUE, INFINITE);
	printf("getlasterror %d", GetLastError());

	TerminateThread(hThread[0], 0x555);
	TerminateThread(hThread[1], 0x555);
	TerminateThread(hThread[2], 0x555);

	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);
	shutdown(m_socket, SD_SEND);
	closesocket(m_socket);

	WSACleanup();

	return;
}
