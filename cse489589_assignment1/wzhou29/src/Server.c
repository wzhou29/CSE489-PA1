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
		if ((selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL)) < 0){
			perror("select() Error");
			exit(-1);
		}
		else{
			for (SocketIdx = 0; SocketIdx <= head_socket; ++SocketIdx){
				if (FD_ISSET(SocketIdx, &watch_list)){
					if (SocketIdx == STDIN){
						char *input = (char *)malloc(sizeof(char) * MsgSize);
						bzero(input,MsgSize);
						if (fgets(input, MsgSize - 1, stdin) == NULL){
							exit(-1);
						}
						trim(input);
						char *command = (char *)malloc(sizeof(char) * CMDSize);
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
							trim(command);
							if (num_args == 1){
								args[0][strlen(args[0])] = '\0';
							}
						}
						if (strcmp(command, "AUTHOR") == 0){AUTHOR();}
						else if (strcmp(command, "IP") == 0){IP();}
						else if (strcmp(command, "PORT") == 0){PORT(PortNumber);}
						else if (strcmp(command, "LIST") == 0){
							for (int i = 0; i < num_users; ++i){
								int min = i;
								for (int j = i + 1; j < num_users; ++j){
									if (users[j].PortNumber < users[min].PortNumber){ min = j;}
								}
								struct user temp = users[i];
								users[i] = users[min];
								users[min] = temp;
							}
							int x = 1;
							cse4589_print_and_log("[%s:SUCCESS]\n", "LIST");
							for (int i = 0; i < num_users; ++i){
								if (users[i].login == 1){
									cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", x, users[i].hostname, users[i].ip_addr, users[i].PortNumber);
									++x;
								}
							}
							cse4589_print_and_log("[%s:END]\n", "LIST");
						}
						else if (strcmp(command, "STATISTICS") == 0){STATISTICS(users,num_users);}
						else if (strcmp(command, "BLOCKED") == 0){BLOCKED(users,num_users,args[1]);}
					}
					else if (SocketIdx == server_socket){
						caddr_len = sizeof(client_addr);
						if ((fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len)) < 0){
							perror("accept() on incoming client failed");
						}
						else{
							char ip_addr[16];
							inet_ntop(AF_INET, &client_addr.sin_addr, ip_addr, sizeof(ip_addr));
							int loc = FindByIP(ip_addr, users, num_users);
							int logged_in_before = 0;
							if (loc > -1){
								logged_in_before = 1;
							}
							if (logged_in_before){
								users[loc].status = 1;
								users[loc].socket = fdaccept;
							}
							else{
								struct user new_user;
								bzero(new_user.hostname,HOSTNAME_LEN);
								if (strcmp(ip_addr, "128.205.36.33") == 0){strcpy(new_user.hostname, "highgate.cse.buffalo.edu");}
								else if (strcmp(ip_addr, "128.205.36.34") == 0){strcpy(new_user.hostname, "euston.cse.buffalo.edu");}
								else if (strcmp(ip_addr, "128.205.36.35") == 0){strcpy(new_user.hostname, "embankment.cse.buffalo.edu");}
                                else if (strcmp(ip_addr, "128.205.36.36") == 0){strcpy(new_user.hostname, "underground.cse.buffalo.edu");}
								else if (strcmp(ip_addr, "128.205.36.46") == 0){strcpy(new_user.hostname, "stones.cse.buffalo.edu");}
							
								strcpy(new_user.ip_addr, ip_addr);
								new_user.PortNumber = ntohs(client_addr.sin_port);
								new_user.status = 1;
								new_user.socket = fdaccept;
								new_user.msg_sent = 0;
								new_user.msg_recv = 0;
								new_user.buff_size = 0;
								new_user.num_blocked = 0;
								users[num_users] = new_user;
								num_users++;
							}
							FD_SET(fdaccept, &master_list);
							if (fdaccept > head_socket){
								head_socket = fdaccept;
							}
							loc = FindByIP(ip_addr, users, num_users);
							for (int i = 0; i < num_users; ++i){
								users[loc].list_storage[i] = &users[i];
							}
							users[loc].num_users = num_users;
							char *temp[num_users];
							ListFunc(users, num_users, temp);
							char *sendable = (char *)malloc(sizeof(char) * MsgSize);
							bzero(sendable,MsgSize);
							for (int i = 0; i < num_users; ++i){
								strcat(sendable, temp[i]);
							}
							send(fdaccept, sendable, strlen(sendable), 0);
							char success[50] = "[LOGIN:SUCCESS]\n";
							send(fdaccept, success, strlen(success), 0);
							if (logged_in_before){
								for (int i = 0; i < users[loc].buff_size; ++i){
									send(fdaccept, users[loc].buffered[i], strlen(users[loc].buffered[i]), 0);
								}
								users[loc].buff_size = 0;
							}
							char end[50] = "[LOGIN:END]\n";
							send(fdaccept, end, strlen(end), 0);
						}
					}
					else{
						char *buffer = (char *)malloc(sizeof(char) * MsgSize);
						bzero(buffer,MsgSize);
						if (recv(SocketIdx, buffer, MsgSize, 0) <= 0){
							int remove = FindBySocket(SocketIdx, users, num_users);
							if (remove != -1){
								for (int i = remove; i < num_users; ++i){
									users[i] = users[i + 1];
								}
								num_users--;
								close(SocketIdx);
								FD_CLR(SocketIdx, &master_list);
							}
						}
						else{
							trim(buffer);
							char *temp = (char *)malloc(sizeof(char) * strlen(buffer));
							strcpy(temp, buffer);
							char *args[100];
							int num_args = 0;
							args[num_args] = strtok(temp, " ");
							while (args[num_args] != NULL){
								args[++num_args] = strtok(NULL, " ");
							}
							char *command = (char *)malloc(sizeof(char) * CMDSize);
							bzero(command,CMDSize);
							strcpy(command, args[0]);
							trim(command);
							if (strcmp("LOGOUT", command) == 0){
								int loc = FindBySocket(SocketIdx, users, num_users);
								users[loc].status = 0;
								close(SocketIdx);
								FD_CLR(SocketIdx, &master_list);
							}
							else if (strcmp("EXIT", command) == 0){
								int loc = FindBySocket(SocketIdx, users, num_users);
								for (int i = loc; i < num_users; ++i){
									users[i] = users[i + 1];
								}
								num_users--;
								close(SocketIdx);
								FD_CLR(SocketIdx, &master_list);
							}
							else if (strcmp("REFRESH", command) == 0){
								char *temp[num_users];
								ListFunc(users, num_users, temp);
								char *list = (char *)malloc(sizeof(char) * MsgSize);
								bzero(list,strlen(list));
								for (int i = 0; i < num_users; ++i){
									strcat(list, temp[i]);
								}
								send(SocketIdx, list, strlen(list), 0);
								int loc = FindBySocket(SocketIdx, users, num_users);
								for (int i = 0; i < num_users; ++i)
								{
									users[loc].list_storage[i] = &users[i];
								}
								users[loc].num_users = num_users;
								char success[50] = "[REFRESH:SUCCESS]\n[REFRESH:END]\n";
								send(SocketIdx, success, strlen(success), 0);
							}
							else if (strcmp("SEND", command) == 0){SEND(SocketIdx, args[1], buffer, users, num_users);}
							else if (strcmp("BROADCAST", command) == 0){BROADCAST(SocketIdx, buffer, users, num_users);}
							else if (strcmp("BLOCK", command) == 0){BLOCK(SocketIdx, args[1], users, num_users);}
							else if (strcmp("UNBLOCK", command) == 0){UNBLOCK(SocketIdx, args[1], users, num_users);}
						}
					}
				}
			}
		}
	}
}
