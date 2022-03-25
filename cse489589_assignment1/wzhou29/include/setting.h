#ifndef SETTING_H_
#define SETTING_H_

#include "global.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <ctype.h>

#define MsgSize 256
#define CMDSize 20
#define Backlog 5
#define STDIN 0
#define TRUE 1

struct user{
	char hostname[HOSTNAME_LEN];
	char ip_addr[16];
	int PortNumber;
	int status;
	int login;
	int logout;
	int socket;

	int msg_sent;
	int msg_recv;

	char buffered[100][MsgSize];
	int buff_size;

	int num_blocked;
	struct user *blocked[4];

	struct user *list_storage[4];
	int num_users;
};

void trim(char *str);
int ValidIP(char *ip);
void ListFunc(struct user *users, int size, char **list_format);
int FindByIP(char *ip, struct user users[], int num_users);
int FindBySocket(int socket, struct user users[], int num_users);
int LocalIP(char *ip_addr, struct user *list_storage[], int num_users);
int BlockedBy(char *blocker, char *block, struct user users[], int num_users);
int ConnectToServer(char *server_ip, char *server_port_char, int host_port);

void ServerHost(int PortNumber);
void ClientHost(int PortNumber);

void AUTHOR();
void IP();
void PORT(int PortNumber);
void STATISTICS(struct user users[], int num_users);
void BLOCKED(struct user users[], int num_users, char *ip);
int SEND(int from_socket, char *to_ip, char *buffer, struct user users[], int num_users);
int BROADCAST(int from_socket, char *buffer, struct user users[], int num_users);
int BLOCK(int socket, char *ip, struct user users[], int num_users);
int UNBLOCK(int socket, char *ip, struct user users[], int num_users);

#endif