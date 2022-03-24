/**
 * @wzhou29_assignment1
 * @author  Weicheng Zhou <wzhou29@buffalo.edu>
 * @author  Haifeng Xiao <haifengx@bufalo.edu>
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
#include <sys/select.h>

#include "../include/global.h"
#include "../include/logger.h"

// construct boolean
#ifndef __cplusplus
typedef unsigned char bool;
static const bool False = 0;
static const bool True = 1;
#endif

#define BACKLOG 5 // Amount of request line up waiting for Accept() function

void ClientH(int PortNumber);
void ServerH(int PortNumber);
bool check_port(char *port_num);
void AUTHOR();
void IP();
void PORT();
void LIST();
void STATISTICS();
void BLOCKED();
void LOGIN();
void REFRESH();
void SEND();
void BROADCAST();
void BLOCK();
void UNBLOCK();
void LOGOUT();
void EXIT(int Socket);
void SENDFILE();

struct user 
{
	char* hostname;
	char* ip_addr;
	int port_num;
	int num_msg_sent;
	int num_msg_rcv;
	// char status;
	// int fd;
	bool is_logged_in;
	bool is_server;
	// struct message * queued_messages;
};

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
		if(strcmp("s", argv[1]) == 0 && check_port(argv[2]) == 1)
		{
			ServerH(atoi(argv[2]));
		}
		else if(strcmp("c", argv[1]) == 0 && check_port(argv[2]) == 1)
		{
			ClientH(atoi(argv[2]));
		}
		else { 
			cse4589_print_and_log("./[name of file] [c/s] [port]\n"); 
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
	if (bind(ServerSocket,(struct sockaddr *)&ServerHost,sizeof(ServerHost)) < 0){ // Bind socket and check if the socket bind or not if not print error message and exit
		perror("Error on binding Socket");
		close(ServerSocket);
		exit(-1);
	}
	//set server socket to lsiten
	if(listen(ServerSocket,BACKLOG) < 0){ // Listen to port, if can't listen to port number, it will exit
		perror("Error on listening to Port");
		close(ServerSocket);
		exit(-1);
	}
	// //accepting client socket
	// int ClientSocket;
	// int socklen = sizeof(struct sockaddr_in);
	// struct sockaddr_in ClientHost;    //client information
	// ClientSocket = accept(ServerSocket, (struct sockaddr*)&ClientHost, (socklen_t*)&socklen); // accepting client request

	// fd_set MasterList, WatchList;
	// FD_ZERO(&MasterList);
	// FD_ZERO(&WatchList);
	// FD_SET(ServerSocket,&MasterList);
	// // FD_SET(ClientSocket,&MasterList);
	// int HSocket = ServerSocket;
	// while(1){
	// 	memcpy(&WatchList,&MasterList,sizeof(MasterList));
	// 	if(select(HSocket+1,&WatchList,NULL,NULL,NULL) < 0){
	// 		perror("Select Error");
	// 		close(ServerSocket);
	// 		exit(-1);
	// 	}
	// 	char *input;
	// 	scanf("%s",input);
	// 	cse4589_print_and_log("%s",input);
	// }
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
}

// //check if the ip address in correct form
// bool check_ip(char *ip) {
//     char tmp[16];
//     int sections = 0;
//     char *ip_length[16];

// 	memset(tmp, '\0', 16);
// 	strcpy(tmp, ip);
	
// 	ip_length[sections] = strtok(tmp, ".");
// 	while (ip_length[sections] != NULL) { 
// 		ip_length[++sections] = strtok(NULL, "."); 
// 	}
    
//     for (int i = 0; i < sections; ++i) { 
// 			for (int j = 0; j < strlen(ip_length[i]); ++j) { 
// 				if (ip_length[i][j] < '0' || ip_length[i][j] > '9')	return 0;
// 			}
// 			int check = atoi(ip_length[i]); 
//             if(check < 0 || check > 256) return 0;
// 		}
// 	if (sections != 4)return 0; 
// 	return 1;
// }

// //Check if the port in the correct form
bool check_port(char *port_num){
    if(strlen(port_num) != 4) return 0;
    else{
        for(int i = 0; i < strlen(port_num); i++){
            if(!isdigit(port_num[i])){
                perror("Invalid Port Number!");
                return 0;
            }
        }
    }
    return 1;
}

// print AUTHOR information
void AUTHOR(){
	// Print the author information
	cse4589_print_and_log("[AUTHOR:SUCCESS]\n");
	cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "haifengx");
	cse4589_print_and_log(("I, %s, have read and understood the course academic integrity policy.\n", "wzhou29"));
	cse4589_print_and_log("[AUTHOR:END\n]");
}
void IP(){
	char* ip;
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, "8.8.8.8", &addr.sin_addr);  // set ip address to google's ip address
	addr.sin_port = htons(53);

	// connecting socket
	if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		cse4589_print_and_log("[%s:ERROR]\n", "IP");
		cse4589_print_and_log("[%s:END]\n", "IP");
		return;
	}
	bzero(&addr, sizeof(addr));
	int addr_len = sizeof(addr);
	getsockname(sockfd, (struct sockaddr *)&addr, &addr_len);
	inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));

	cse4589_print_and_log("[%s:SUCCESS]\n", "IP");
	cse4589_print_and_log("IP:%s\n", ip);
	cse4589_print_and_log("[%s:END]\n", "IP");
	close(sockfd);
	return; 
}

// Print Port information
void PORT(){
	struct user curr_user;
	cse4589_print_and_log("[PORT:SUCCESS]\n");
	cse4589_print_and_log("PORT:%d\n", curr_user.port_num);
	cse4589_print_and_log("[PORT:END]\n");
}

void LIST(){
	struct user curr_user;
	int numOf_user;
	struct user *list;
	char **ret;

	for(int i = 0; i < numOf_user; i++) {
		int j = i;
		for(int k = i+1; k < numOf_user; ++j) {
			if(list[k].port_num < list[j].port_num) {
				j = k;
			}
		}
		struct user tmp_user = list[i];
		list[i] = list[j];
		list[j] = tmp_user;

	}
	cse4589_print_and_log("[LIST:SUCCESS]\n]");
	int count = 1;
	for(int i = 0; i < numOf_user; ++i) {
		if(curr_user.is_logged_in) {
			char *buf = (char*) malloc(sizeof(char)*500);
			bzero(buf, 500);
			sprintf(buf, "%-5d%-35s%-20s%-8d\n", count, list[i].hostname, list[i].ip_addr, list[i].port_num);
			ret[count-1] = buf;
			count++;
		}
	}
	cse4589_print_and_log("[LIST:END]\n]");

}

void STATISTICS(){
	struct user curr_user;
	int numOf_user;
	struct user *list;
	char **ret;

	for(int i = 0; i < numOf_user; i++) {
		int j = i;
		for(int k = i+1; k < numOf_user; ++j) {
			if(list[k].port_num < list[j].port_num) {
				j = k;
			}
		}
		struct user tmp_user = list[i];
		list[i] = list[j];
		list[j] = tmp_user;

	}
	cse4589_print_and_log("[STATISTICS:SUCCESS]\n]");
	int count = 1;
	for(int i = 0; i < numOf_user; ++i) {
		if(curr_user.is_logged_in) {
			char *buf = (char*) malloc(sizeof(char)*500);
			bzero(buf, 500);
			char *statis = "Logged-out";
			if(list[i].is_logged_in){
				statis = "Logged-in";
			}
			sprintf(buf,"%-5d%-35s%-8d%-8d%-8s\n", count,list[i].hostname, list[i].num_msg_sent, list[i].num_msg_rcv, statis);
			ret[count-1] = buf;
			count++;
		}
	}
	cse4589_print_and_log("[STATISTICS:END]\n]");
}

void BLOCKED(){
}

void LOGIN(){
	// if(connect(ClientSocket, (struct sockaddr *)&ClientHost, sizeof(ClientHost)) != 0) {
	// 	cse4589_print_and_log("[LOGIN:ERROR]")
	// 	close(ClientSocket); exit(-1);
	// }
}

void REFRESH(){
	cse4589_print_and_log("[REFRESH:SUCCESS]\n"); 
	cse4589_print_and_log("[REFRESH:END]\n"); 
}

void SEND(){
}

void BROADCAST(){
}

void BLOCK(){
}

void UNBLOCK(){
}

void LOGOUT(){
}

void EXIT(int Socket){
	close(Socket);
	cse4589_print_and_log("[EXIT:SUCCESS]");
	cse4589_print_and_log("[EXIT:END]");
}

void SENDFILE(){
	cse4589_print_and_log("[%s:ERROR]\n", "IP");
	cse4589_print_and_log("[%s:END]\n", "IP");
}