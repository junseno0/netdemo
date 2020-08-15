#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

void err_handling(const char *str) {
	fputs(str, stderr);
	fputc('\n', stderr);
	exit(1);
}

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("Usage: %s. <IP> <port> \n", argv[0]);
		exit(1);
	}
	int client_sock, msglen;
	struct sockaddr_in serv_addr, from_addr;
	socklen_t from_addr_size;
	char msg[BUF_SIZE] = {0};
	char store[BUF_SIZE] = {0};
	client_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(client_sock == -1)
		err_handling("fail to create client socket.");
	
	memset(&serv_addr, 0x00, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	from_addr_size = sizeof(from_addr);
	while(1) {
		fputs("Input mesage(Q to quit):",stdout);
		fgets(msg, sizeof(msg), stdin);
		if(!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) break;
		msglen = sendto(client_sock, msg, strlen(msg), 0,
				(struct sockaddr*)&serv_addr, sizeof(serv_addr));
		recvfrom(client_sock, store, msglen, 0, (struct sockaddr*)&from_addr,
				&from_addr_size);
		store[msglen] = '\0';
		printf("Message from server: %s \n", store);
	}
	close(client_sock);
	return 0;
}
