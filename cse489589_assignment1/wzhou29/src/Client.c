#include "../include/setting.h"

void ClientHost(int PortNumber) {  
	int server = -1;
	char *list_storage = (char*) malloc(sizeof(char)*1000);
	bzero(list_storage,1000);
	int refresh = 0, login = 0; 
	int selret, sock_idx, head_socket; 
	fd_set watch_list, master_list;  
	FD_ZERO(&master_list); 
	FD_ZERO(&watch_list); 
	FD_SET(STDIN, &master_list); 
	head_socket = STDIN; 
	
	while(1) { 		
		memcpy(&watch_list, &master_list, sizeof(master_list)); 
		if ((selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL)) < 0) { 
			perror("select() Error"); 
			exit(-1);
		} 
		else {
			for (sock_idx = 0; sock_idx <= head_socket; ++sock_idx) { 
				if (FD_ISSET(sock_idx, &watch_list)) { 
					if (sock_idx == STDIN) {
						char *input = (char*) malloc(sizeof(char)*MsgSize);
						bzero(input,CMDSize);
						if(fgets(input, MsgSize-1, stdin) == NULL) {
							exit(-1);
						}
						trim(input);
						char *cmd = (char*) malloc(sizeof(char)*CMDSize);
						bzero(cmd,CMDSize);
						int num_args = 0; 
						char *args[100];
						if (strcmp("", input) != 0) { 
							char *temp = (char*) malloc(sizeof(char)*strlen(input)); 
							strcpy(temp, input); 
							args[num_args] = strtok(temp, " "); 
							while (args[num_args] != NULL) { 
								args[++num_args] = strtok(NULL, " "); 
							}
							strcpy(cmd, args[0]); 
							trim(cmd); 
							if (num_args == 1) { 
								args[0][strlen(args[0])] = '\0';
							}
						}						 
						if (strcmp(cmd, "AUTHOR") == 0) { AUTHOR();}
						else if (strcmp(cmd, "IP") == 0) { IP();; }
						else if (strcmp(cmd, "PORT") == 0) { PORT(PortNumber); }
						else if (strcmp(cmd, "LIST") == 0) {
							cse4589_print_and_log("[%s:ERROR]\n", cmd);
							cse4589_print_and_log("Didin't login to Server\n");
							cse4589_print_and_log("[%s:END]\n", cmd);
						}
						else if (strcmp(cmd, "LOGIN") == 0) {
							if (num_args == 3) { 
								server = ConnectToServer(args[1], args[2], PortNumber); 
								if (server > -1) { 
									FD_SET(server, &master_list); 
									if (server > head_socket) { 
										head_socket = server; 
									} 
									login = 1; 
								} 
								else { 
									cse4589_print_and_log("[%s:ERROR]\n", cmd);
									cse4589_print_and_log("[%s:END]\n", cmd);
								}
							}
						}
						else if (strcmp(cmd, "EXIT") == 0) {
							if (server == -8) {
								server = ConnectToServer(args[1], args[2], PortNumber); 
							}
							if (server > 0) { 
								send(server, cmd, strlen(cmd), 0); 
							}
							cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
							cse4589_print_and_log("[%s:END]\n", cmd);
							exit(0); 
						}
						else {
							if (server > 0) { 
								if (strcmp("LIST", cmd) == 0) {
									cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
									cse4589_print_and_log("%s", list_storage); 
									cse4589_print_and_log("[%s:END]\n", cmd);
								}
								else { 
									send(server, input, strlen(input), 0);
									if (strcmp("LOGOUT", cmd) == 0) {															
										FD_CLR(server, &master_list); 
										close(server); 
										server = -8; 
										cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
										cse4589_print_and_log("[%s:END]\n", cmd);
									}
									else if (strcmp("REFRESH", cmd) == 0) { 
										refresh = 1; 
									}
								}		
							}
						}
					}
					else if (sock_idx == server) { 
						char *buffer = (char*) malloc(sizeof(char)*MsgSize);
						bzero(buffer,MsgSize);
						if (recv(server, buffer, MsgSize, 0) <= 0) { 
							FD_CLR(sock_idx, &master_list); 
							close(server); 
						}
						else {
							if (!refresh && !login) { 
								cse4589_print_and_log("%s", buffer); 
								fflush(stdout); 
							}
							if (refresh || login) {
								strcpy(list_storage, buffer); 
								refresh = 0;
								login = 0; 
							}
						}
					}
				}
			}
		}
	}
}