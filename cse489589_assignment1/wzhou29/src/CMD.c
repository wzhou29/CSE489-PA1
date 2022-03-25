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


