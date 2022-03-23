/**
 * @wzhou29_assignment1
 * @author  Weicheng Zhou <wzhou29@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "../include/global.h"
#include "../include/logger.h"

#define BACKLOG 5 // Amount of request line up waiting for Accept() function

void ClientH(int PortNumber);
void ServerH(int PortNumber);

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */

int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));

	/*Start Here*/
	if(argc == 3)
	{
		if(strcmp("s", argv[1]) == 0)
		{
			ServerH(atoi(argv[2]));
		}
		else if(strcmp("c", argv[1]) == 0)
		{
			ClientH(atoi(argv[2]));
		}
		else { 
			perror("./[name of file] [c/s] [port]"); 
			exit(-1); 
		}
		
	}
	else{
		cse4589_print_and_log("Please enter two arguments: \n");
		cse4589_print_and_log("First argument: 's' for server, 'c' for client\n");
		cse4589_print_and_log("Second argument: Port Number\n");
		exit(-1);
	}
	return 0;
}
// server
void ServerH(int PortNumber){
	// creating sock
	int ServerSocket = socket(AF_INET,SOCK_STREAM,0); // Creating TCP Socket
	if (ServerSocket == -1) { perror("Error On Creating Socket"); exit(-1); }

	//socket information
	struct sockaddr_in ServerHost; 	// create a new struct base on sockaddr_in  
	bzero(&ServerHost,sizeof(ServerHost)); // initial the struct         
	ServerHost.sin_family = AF_INET; // Address Family: IPv4
	ServerHost.sin_addr.s_addr = htonl(INADDR_ANY); // Internet Address       
	ServerHost.sin_port = htons(PortNumber); // Port Number
	if (bind(ServerSocket,(struct sockaddr_in *)&ServerHost,sizeof(ServerHost)) != 0){ // Bind socket and check if the socket bind or not if not print error message and exit
		perror("Error on binding Socket");
		close(ServerSocket);
		exit(-1);
	}
		cse4589_print_and_log("it work\n");
	// //set server socket to lsiten
	// if(listen(ServerSocket,BACKLOG) != 0){ // Listen to port, if can't listen to port number, it will exit
	// 	perror("Error on listening to Port");
	// 	close(ServerSocket);
	// 	exit(-1);
	// }

	// //accepting client socket
	// int ClientSocket;
	// int socklen = sizeof(struct sockaddr_in);
	// struct sockaddr_in ClientHost;    //client information
	// ClientSocket = accept(ServerSocket, (struct sockaddr_in*)&ClientHost, (socklen_t*)&socklen); // accepting client request

	// fd_set MasterList, WatchList;
	// FD_ZERO(&MasterList);
	// FD_ZERO(&WatchList);
	// FD_SET(ServerSocket,&MasterList);
	// FD_SET(0,&MasterList);

	// int HSocket = ServerSocket;
	// while(1){
	// 	memcpy(&WatchList,&MasterList,sizeof(MasterList));
	// 	if(select(HSocket+1,&WatchList,NULL,NULL,NULL) < 0){
	// 		perror("Select Error");
	// 		close(ServerSocket);
	// 		exit(-1);
	// 	}
	// 	for(int idx = 0; idx <= HSocket; ++idx){
	// 		if(FD_ISSET(idx,&WatchList)){
	// 			if(idx == 0){
	// 				char *input = (char*) malloc(sizeof(char)*1024);
	// 				bzero(input,1024);
	// 				if(fgets(input,1023,0) == NULL){
	// 					exit(-1);
	// 				}
	// 				char *cmd = (char*) malloc(sizeof(char)*32);
	// 				bzero(cmd,32);
	// 				int NumberArgument = 0;
	// 				char *Argument[256];
	// 				if(strcmp("",input) == 0){
	// 					Argument[NumberArgument] = strtok(input,"");
	// 					while(Argument[NumberArgument] != NULL){
	// 						Argument[++NumberArgument] = strtok(NULL,"");
	// 					}
	// 					strcpy(cmd,Argument[0]);
	// 					if(NumberArgument == 1){
	// 						Argument[0][strlen(Argument[0])] = "\0";
	// 					}
	// 				}
	// 				if(strcmp(cmd,"AUTHOR") == 0){AUTHOR();}
	// 				else if(strcmp(cmd,"IP") == 0){IP();}
	// 				else if(strcmp(cmd,"PORT") == 0){PORT();}
	// 				else if(strcmp(cmd,"LIST") == 0){LIST();}
	// 				else if(strcmp(cmd,"STATISTICS") == 0){STATISTICS();}
	// 				else if(strcmp(cmd,"BLOCKED") == 0){BLOCKED();}
	// 				else if(strcmp(cmd,"LOGIN") == 0){LOGIN();}
	// 				else if(strcmp(cmd,"REFRESH") == 0){REFRESH();}
	// 				else if(strcmp(cmd,"BROADCAST") == 0){BROADCAST();}
	// 				else if(strcmp(cmd,"BLOCK") == 0){BLOCK();}
	// 				else if(strcmp(cmd,"UNBLOCK") == 0){UNBLOCK();}
	// 				else if(strcmp(cmd,"LOGOUT") == 0){LOGOUT();}
	// 				else if(strcmp(cmd,"EXIT") == 0){EXIT();}
	// 				else if(strcmp(cmd,"SENDFILE") == 0){SENDFILE();}
	// 			}
	// 		}
	// 	}
	// }
	// close(ClientSocket);
	close(ServerSocket);
}
// client
void ClientH(int PortNumber){
	//creating socket
	int ClientSocket = socket(AF_INET,SOCK_STREAM,0); // Creating TCP Socket
	if(ClientSocket == -1) { perror("Error On Creating Socket"); exit(-1);}

	struct host_to_server* hoster;
	struct sockaddr_in ClientHost;
	ClientHost.sin_addr.s_addr = htonl(INADDR_ANY);  // point to ip address
	bzero(&ClientHost,sizeof(ClientHost));
	ClientHost.sin_family = AF_INET;
	ClientHost.sin_port = htons(PortNumber);
	cse4589_print_and_log("it work c\n");
	// if(connect(ClientSocket, (struct sockaddr *)&ClientHost, sizeof(ClientHost)) != 0) {
	// 	perror("Connecting Error");
	// 	close(ClientSocket); exit(-1);
	// }
	close(ClientSocket);
}
