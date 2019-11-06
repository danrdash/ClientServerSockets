# ClientServerSockets
pseudo-online version for 4 in a row game, including chat between 2 players. server/client implementation with TCP Sockets communication.

## How to compile and run
first initalize a server, by running the program with 3 arguments, path to logfile and path to output file and entering the mode.
then, initalize two instances of players by entering the following arguments: path to input, socket_port, player name, mode, and output file path.
an example is listed below:

### On Windows OS: 
1. download and install MinGW: https://osdn.net/projects/mingw/downloads/68260/mingw-get-setup.exe/
2. download all project files into the same folder, insert the following command: cd /...your_file_path.../
3. insert the following command: gcc *.c -o Client
4. insert the following command: gcc *.c -o Server
5. insert the following command to initialize server: Server --logfile="/path/to/logfile" --output="/path/to/output "" "server"
6. insert the following command to initialize player 1: Client1 --input="/path/to/input" socket_port "player_name" "client" --output="/path/to/output"
7. insert the following command to initialize player 2: Client2 --input="/path/to/input" socket_port "player_name" "client" --output="/path/to/output"



 * for socket_port enter 1234 or any combination of 4 numbers


## IDE
Written and compiled in Visual Studio 2017

## License
[MIT](https://choosealicense.com/licenses/mit/)
