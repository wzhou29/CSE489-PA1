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

//Setting for all function
#include "../include/setting.h"

#include "../include/global.h"
#include "../include/logger.h"

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

	int check_port = 1;
	for (int i = 0; i < strlen(argv[2]); ++i){
		if ((!isdigit(argv[2][i])) && isdigit(argv[2][i]) > -1){ check_port=0; }
	}

	if (argc == 3 && check_port == 1){
		if (strcmp("s", argv[1]) == 0){
			ServerHost(atoi(argv[2]));
		}
		else if (strcmp("c", argv[1]) == 0){
			ClientHost(atoi(argv[2]));
		}
		else{
			cse4589_print_and_log("Please select first argument as s or c\n");
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

void trim(char *str){
	int index = -1;
	for(int i = 0; str[i] != '\0';++i){
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n'){
			index = i;
		}
	}
	str[index + 1] = '\0';
}

int ValidIP(char *ip){
	char temp[20];
	bzero(temp,20);
	strcpy(temp, ip);
	
	int num_args = 0;
	char *args[20];

	args[num_args] = strtok(temp, ".");
	while (args[num_args] != NULL){
		args[++num_args] = strtok(NULL, ".");
	}

	if (num_args != 4){ return 0; }
	else{
		for (int i = 0; i < num_args; ++i){
			for (int j = 0; j < strlen(args[i]); ++j){
				if (isdigit(args[i][j])){ return 0; }
			}
			int check = atoi(args[i]);
			if (check > 256 || check < 0){ return 0; }
		}
	}
	return 1;
}

void ListFunc(struct user *users, int numOf_users, char **list_format){
	for (int i = 0; i < numOf_users; ++i){
		int smallest = i;
		for (int j = i + 1; j < numOf_users; ++j){
			if (users[j].PortNumber < users[smallest].PortNumber){ smallest = j; }
		}
		struct user temp = users[i];
		users[i] = users[smallest];
		users[smallest] = temp;
	}
	int count = 1;
	for (int i = 0; i < numOf_users; ++i){
		if (users[i].login == 1){
			char *buffer = (char *)malloc(sizeof(char) * MsgSize);
			bzero(buffer,MsgSize);
			cse4589_print_and_log(buffer, "%-5d%-35s%-20s%-8d\n", count, users[i].hostname, users[i].ip_addr, users[i].PortNumber);
			list_format[count - 1] = buffer;
			count++;
		}
	}
}

int FindByIP(char *ip, struct user users[], int num_users){
	for (int i = 0; i < num_users; ++i){
		if (strcmp(users[i].ip_addr, ip) == 0){
			return i;
		}
	}
	return -1;
}

int FindBySocket(int socket, struct user users[], int num_users){
	for (int i = 0; i < num_users; ++i){
		if (socket == users[i].socket){
			return i;
		}
	}
	return -1;
}

int ConnectToServer(char *server_ip, char *server_port_char, int host_port){
	int check_port = 1;
	for (int i = 0; i < strlen(server_port_char); ++i){
		if ((!isdigit(server_port_char[i])) && isdigit(server_port_char[i]) > -1){ check_port=0; }
	}
	if (!ValidIP(server_ip) || check_port == 0){
		cse4589_print_and_log("[%s:ERROR]\n", "LOGIN");
		cse4589_print_and_log("[%s:END]\n", "LOGIN");
		return -1;
	}
	else{
		int server_port = atoi(server_port_char);
		int socketfd;
		struct sockaddr_in server_addr, client_addr;

		socketfd = socket(AF_INET, SOCK_STREAM, 0);
		if (socketfd < 0){
			perror("socket() failed\n");
		}
		bzero(&client_addr, sizeof(client_addr));
		client_addr.sin_family = AF_INET;
		client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		client_addr.sin_port = htons(host_port);
		if (bind(socketfd, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_in)) != 0){
			perror("failed to bind PortNumber to client");
		}
		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
		server_addr.sin_port = htons(server_port);
		if (connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
			socketfd = -1;
			cse4589_print_and_log("[%s:ERROR]\n", "LOGIN");
			cse4589_print_and_log("[%s:END]\n", "LOGIN");
			return -1;
		}
		return socketfd;
	}
}

int LocalIP(char *ip_addr, struct user *list_storage[], int num_users){
	for (int i = 0; i < num_users; ++i){
		if (strcmp((list_storage[i])->ip_addr, ip_addr) == 0){
			return i;
		}
	}
	return -1;
}

int BlockedBy(char *blocker, char *block, struct user users[], int num_users){
	int BlockerIdx = FindByIP(blocker, users, num_users);
	for (int i = 0; i < users[BlockerIdx].num_blocked; ++i){
		if (strcmp(users[BlockerIdx].blocked[i]->ip_addr, block) == 0){
			return 1;
		}
	}
	return 0;
}