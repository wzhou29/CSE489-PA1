#include "../include/setting.h"

void ServerHost(int PortNumber){
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0){ perror("socket() Error"); exit(-1); }


	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PortNumber);
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){ perror("bind() Error"); exit(-1); }
	if (listen(server_socket, Backlog) < 0){ perror("listen() Error"); exit(-1); }

	int head_socket, selret, SocketIdx, fdaccept = 0, caddr_len;
	struct sockaddr_in client_addr;
	fd_set master_list, watch_list;
	FD_ZERO(&master_list);
	FD_ZERO(&watch_list);
	FD_SET(server_socket, &master_list);
	FD_SET(STDIN, &master_list);
	head_socket = server_socket;
	struct user users[4];
	int num_users = 0;
	while(1){
		memcpy(&watch_list, &master_list, sizeof(master_list));
		if ((selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL)) < 0){ exit(-1);}
		else{
			for (SocketIdx = 0; SocketIdx <= head_socket; ++SocketIdx){
				if (FD_ISSET(SocketIdx, &watch_list)){
					if (SocketIdx == STDIN){
						char *input = (char *)calloc(MsgSize, sizeof(char));
						bzero(input,MsgSize);
						if (fgets(input, MsgSize - 1, stdin) == NULL){exit(-1);}
						fix(input);
						char *command = (char *)calloc(CMDSize, sizeof(char));
						bzero(command,CMDSize);
						int num_args = 0;
						char *args[100];
						if (strcmp("", input) != 0){
							char *temp = (char *)malloc(sizeof(char) * strlen(input));
							strcpy(temp, input);
							args[num_args] = strtok(temp, " ");
							while (args[num_args] != NULL){
								args[++num_args] = strtok(NULL, " ");
							}
							strcpy(command, args[0]);
							fix(command);
							if (num_args == 1){args[0][strlen(args[0])] = '\0';}
						}
						if (strcmp(command, "AUTHOR") == 0){AUTHOR();}
						else if (strcmp(command, "IP") == 0){IP();}
						else if (strcmp(command, "PORT") == 0){PORT(PortNumber);}
						else if (strcmp(command, "LIST") == 0){ 
                            int counter = 1;
                            for(int i = 0; i < num_users; i++) {
                                int small = i;
                                for(int j = i+1; j < num_users; j++) {
                                    if(users[j].PortNumber < users[small].PortNumber) { small = j; }
                                }
                                struct user user_inList = users[i];
                                users[i] = users[small];
                                users[small] = user_inList;
                            }
                            cse4589_print_and_log("[%s:SUCCESS]\n", "LIST");
                            for(int i = 0; i < 4; i++) {
                                if(users[i].login == 1) cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", counter, users[i].hostname, users[i].ip_addr, users[i].PortNumber); counter++; }
                        }
						else if (strcmp(command, "STATISTICS") == 0){STATISTICS(users,num_users);}
					
	
	}}}}}
}
