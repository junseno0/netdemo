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
	if(argc != 2) {
		printf("Usage: %s. <port> \n", argv[0]);
		exit(1);
	}
	int serv_sock;
	int msglen = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;
	socklen_t client_addr_size;
	char buffer[BUF_SIZE] = {0};
	serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock == -1) err_handling("fail to create udp socket");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		err_handling("fail to bind socket.");
	client_addr_size = sizeof(client_addr);

	while(1) {
		msglen = recvfrom(serv_sock, buffer, BUF_SIZE, 0,(struct sockaddr*)&client_addr,\
				&client_addr_size);
		buffer[msglen] = '\0';
		printf("Message from client: %s", buffer);
		sendto(serv_sock, buffer, msglen, 0, (struct sockaddr*)&client_addr,\
			   	client_addr_size);
	}

	close(serv_sock);
	return 0;
}
