#include "../include/setting.h"

void AUTHOR(){
    cse4589_print_and_log("[%s:SUCCESS]\n", "AUTHOR");
    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "wzhou29");
    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "haifengx");
    cse4589_print_and_log("[%s:END]\n", "AUTHOR");
}

void IP(){
    char ip[18];
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;    // Google's ip address and port number
	inet_pton(AF_INET, "8.8.8.8", &addr.sin_addr);
	addr.sin_port = htons(53);

    // connect the socket to the server
	if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0){
		cse4589_print_and_log("[%s:ERROR]\n", "IP");
		cse4589_print_and_log("[%s:END]\n", "IP");
        close(sockfd);
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

void PORT(int PortNumber){
    cse4589_print_and_log("[%s:SUCCESS]\n", "PORT");
	cse4589_print_and_log("PORT:%d\n", PortNumber);
	cse4589_print_and_log("[%s:END]\n", "PORT");
	return;
}


void STATISTICS(struct user users[], int num_users){
	for (int i = 0; i < num_users; ++i){
		int smallest = i;
		for (int j = i + 1; j < num_users; ++j){
			if (users[j].PortNumber < users[smallest].PortNumber){ smallest = j; }
		}
		struct user temp = users[i];
		users[i] = users[smallest];
		users[smallest] = temp;
	}
	cse4589_print_and_log("[%s:SUCCESS]\n", "STATISTICS");
	for (int i = 0; i < num_users; ++i){
		if (users[i].login == 1 || users[i].logout == 1){
			char *status;
			if (users[i].login == 1){
				status = "logged-in";
			}
			else if (users[i].logout == 1){
				status = "logged-out";
			}
			cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", i + 1, users[i].hostname, users[i].msg_sent, users[i].msg_recv, status);
		}
	}
	cse4589_print_and_log("[%s:END]\n", "STATISTICS");
}

void BLOCKED(struct user users[], int num_users, char *ip){
	if (!ValidIP(ip) || (FindByIP(ip, users, num_users) == -1)){
		cse4589_print_and_log("[BLOCKED:ERROR]\n");
		cse4589_print_and_log("[BLOCKED:END]\n");
	}
	else{
		int loc = FindByIP(ip, users, num_users);
		cse4589_print_and_log("[BLOCKED:SUCCESS]\n");
		for (int i = 0; i < users[loc].num_blocked; ++i){
			cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i, users[loc].blocked[i]->hostname, users[loc].blocked[i]->ip_addr, users[loc].blocked[i]->PortNumber);
		}
	cse4589_print_and_log("[BLOCKED:END]\n");
	}
}

int SEND(int from_socket, char *to_ip, char *buffer, struct user users[], int num_users){
	int from_loc = FindBySocket(from_socket, users, num_users);
	int to_loc = FindByIP(to_ip, users, num_users);
	char error[50] = "[SEND:ERROR]\n[SEND:END]\n";
	int loc = LocalIP(to_ip, users[from_loc].list_storage, users[from_loc].num_users);
	if (!ValidIP(to_ip) || loc == -1 || users[from_loc].list_storage[loc]->login != 1){
		send(from_socket, error, strlen(error), 0);
		return 0;
	}
	else{
		int blocked = BlockedBy(to_ip, users[from_loc].ip_addr, users, num_users);
		char msg[MsgSize], to_client[MsgSize];
		bzero(msg,MsgSize);
		bzero(to_client,MsgSize);
		int msg_len = (strlen(buffer) - (strlen("SEND") + strlen(to_ip) + 2)); // message prep
		int start = strlen("SEND") + 1 + strlen(to_ip) + 1;
		memcpy(msg, buffer + start, msg_len);
		msg[msg_len] = '\0';
		cse4589_print_and_log(to_client, "[RECEIVED:SUCCESS]\nmsg from:%s\n[msg]:%s\n[RECEIVED:END]\n", users[from_loc].ip_addr, msg);
		if (!blocked){
			if (users[to_loc].login == 1){
				send(users[to_loc].socket, to_client, strlen(to_client), 0);
			}
			else{
				if (users[to_loc].logout == 1){
					strcpy(users[to_loc].buffered[users[to_loc].buff_size], to_client);
					users[to_loc].buff_size++;
					users[to_loc].msg_recv++;
				}
			}
			users[to_loc].msg_recv++;
		}
		users[from_loc].msg_sent++;
		char success[50] = "[SEND:SUCCESS]\n[SEND:END]\n";
		send(from_socket, success, strlen(success), 0);
		cse4589_print_and_log("[RELAYED:SUCCESS]\nmsg from:%s, to:%s\n[msg]:%s\n[RELAYED:END]\n", users[from_loc].ip_addr, to_ip, msg);
	}
}

int BROADCAST(int from_socket, char *buffer, struct user users[], int num_users){
	int from_loc = FindBySocket(from_socket, users, num_users);

	int msg_len = (strlen(buffer) - (strlen("BROADCAST") + 1)); // message prep
	int start = strlen("BROADCAST") + 1;
	char *msg = (char *)malloc(sizeof(char) * msg_len);
	memcpy(msg, buffer + start, msg_len);
	msg[msg_len] = '\0';

	char to_clients[MsgSize];
	cse4589_print_and_log(to_clients, "[RECEIVED:SUCCESS]\nmsg from:%s\n[msg]:%s\n[RECEIVED:END]\n", users[from_loc].ip_addr, msg);
	for (int i = 0; i < num_users; ++i){
		int blocked = BlockedBy(users[i].ip_addr, users[from_loc].ip_addr, users, num_users);
		if (users[i].socket != from_socket && !blocked){
			if (users[i].login == 1){
				send(users[i].socket, to_clients, strlen(to_clients), 0);
			}
			else{
				if (users[i].logout == 1){
					strcpy(users[i].buffered[users[i].buff_size], to_clients);
					users[i].buff_size++;
				}
			}
			users[i].msg_recv++;
			users[from_loc].msg_sent++;
		}
	}
	char success[75] = "[BROADCAST:SUCCESS]\n[BROADCAST:END]\n";
	send(from_socket, success, strlen(success), 0);
	cse4589_print_and_log("[RELAYED:SUCCESS]\nmsg from:%s, to:%s\n[msg]:%s\n[RELAYED:END]\n", users[from_loc].ip_addr, "255.255.255.255", msg);
}

int BLOCK(int socket, char *ip, struct user users[], int num_users){
	int blocker = FindBySocket(socket, users, num_users);
	int blockee = FindByIP(ip, users, num_users);
	char error[50] = "[BLOCK:ERROR]\n[BLOCK:END]\n";
	if (!ValidIP(ip)){ send(socket, error, strlen(error), 0); return 0;}
	else if (LocalIP(ip, users[blocker].list_storage, users[blocker].num_users) == -1 || BlockedBy(users[blocker].ip_addr, ip, users, num_users)){
		send(socket, error, strlen(error), 0); return 0;
	}
	users[blocker].blocked[users[blocker].num_blocked] = &users[blockee];
	users[blocker].num_blocked++;
	char success[50] = "[BLOCK:SUCCESS]\n[BLOCK:END]\n";
	send(socket, success, strlen(success), 0);
	return 1;
}

int UNBLOCK(int socket, char *ip, struct user users[], int num_users){
	int unblocker = FindBySocket(socket, users, num_users);
	char error[50] = "[UNBLOCK:ERROR]\n[UNBLOCK:END]\n";
	int blocked = BlockedBy(users[unblocker].ip_addr, ip, users, num_users) - 1;
	if (!ValidIP(ip) || LocalIP(ip, users[unblocker].list_storage, users[unblocker].num_users || blocked == -1) == -1){
		send(socket, error, strlen(error), 0);
		return 0;
	}
	for (int i = blocked; i < users[unblocker].num_blocked; ++i){
		users[unblocker].blocked[i] = users[unblocker].blocked[i + 1];
	}
	users[unblocker].num_blocked--;
	char success[50] = "[UNBLOCK:SUCCESS]\n[UNBLOCK:END]\n";
	return 1;
}