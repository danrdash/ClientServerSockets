
//Authors – Dan Ram 305015521 Ori Asulin 308459304
//Project – ex4
//Description - Enter server side or client side as you see fit
#include "Client.h"
#include "Server.h"
#include "GenFuncs.h"


// global variables
char error_file_path[MAX_PATH_FILE];
SOCKET m_socket;
int board[BOARD_HEIGHT][BOARD_WIDTH] = { 0 };


int main(int argc, char  *argv[])
{
	s_args *s_item = NULL; // initialize structures
	c_args *c_item = NULL;

	
	FILE *fp = NULL;

	
	
	
	strcpy(error_file_path, argv[2]);
	
	if (strcmp(argv[1], "server") == 0) //go into Server mode
	{
		while (1)
		{
			MainServer(argv[2], argv[3]);
			fopen_s(&fp, argv[2], "w");//clear the log file
			fclose(fp);
		}

	}
	else if (strcmp(argv[1], "client") == 0) //go into Client mode
	{
		c_item = InitClientArgs(argv[2], argv[3], argv[4], argv[5], argv[6]);
		fopen_s(&fp, c_item->logfile, "w"); // empty file
		fclose(fp);
		MainClient(c_item);
	}
	else
	{	
		printf("Wrong input, Server or client?");
		ErrorPlan(error_file_path);
	}
	return 0;
}