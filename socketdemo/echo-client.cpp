#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/*
 * create a socket(fd)
 * connect to a localhost specific port.
 * read respond from sever
 * close client socket(fd)
 * */
#define BUF_SIZE 100
int main() {
	while(1) {
	//create socket
	//int socket(int af, int type, int protocol);
	//AF = Address Family, PF = Protocol Family
	//type : data transmit type, SOCK_STREAM, SOCK_DGRAM
	//protocol : transmit protocol, IPPROTO_TCP, IPPTOTO_UDP
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	//send request to server with IP and port
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;//use IPv4
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//IP addr
	serv_addr.sin_port = htons(1235);
	//int connect(int sock, struct sockaddr *serv_addr, socklen_t addrlen);
	int res = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	printf("connect res: %d\n", res);
	//read data from server
	char bufSend[BUF_SIZE] = {0};
	printf("Input a string: ");
	scanf("%s", bufSend);
	printf("message to send: %s\n", bufSend);
	usleep(1000);
	res = write(sock, bufSend, strlen(bufSend));
	printf("write res %d.\n", res);

	//receiver data from server
	char bufRecv[BUF_SIZE] = {0};
	//read(sock, bufRecv, BUF_SIZE);
	
	printf("Message from server: %s\n", bufRecv);
	close(sock);
	}
	return 0;
}
