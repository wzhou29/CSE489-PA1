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
						fix(input);
						char *command = (char*) malloc(sizeof(char)*CMDSize);
						bzero(command,CMDSize);
						int num_args = 0; 
						char *args[100];
						if (strcmp("", input) != 0) { 
							char *temp = (char*) malloc(sizeof(char)*strlen(input)); 
							strcpy(temp, input); 
							args[num_args] = strtok(temp, " "); 
							while (args[num_args] != NULL) { 
								args[++num_args] = strtok(NULL, " "); 
							}
							strcpy(command, args[0]); 
							fix(command); 
							if (num_args == 1) { 
								args[0][strlen(args[0])] = '\0';
							}
						}						 
						if (strcmp(command, "AUTHOR") == 0) { AUTHOR();}
						else if (strcmp(command, "IP") == 0) { IP();; }
						else if (strcmp(command, "PORT") == 0) { PORT(PortNumber); }
						else if (strcmp(command, "LIST") == 0) {
							cse4589_print_and_log("[%s:ERROR]\n", command);
							cse4589_print_and_log("Didin't login to Server\n");
							cse4589_print_and_log("[%s:END]\n", command);
						}
						else if (strcmp(command, "LOGIN") == 0) {
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
									cse4589_print_and_log("[%s:ERROR]\n", command);
									cse4589_print_and_log("[%s:END]\n", command);
								}
							}
						}
						else if (strcmp(command, "EXIT") == 0) {
							if (server == -8) {
								server = ConnectToServer(args[1], args[2], PortNumber); 
							}
							if (server > 0) { 
								send(server, command, strlen(command), 0); 
							}
							cse4589_print_and_log("[%s:SUCCESS]\n", command);
							cse4589_print_and_log("[%s:END]\n", command);
							exit(0); 
						}
						else {
							if (server > 0) { 
								if (strcmp("LIST", command) == 0) {
									cse4589_print_and_log("[%s:SUCCESS]\n", command);
									cse4589_print_and_log("%s", list_storage); 
									cse4589_print_and_log("[%s:END]\n", command);
								}		
							}
						}
					}
				}
			}
		}
	}
}
