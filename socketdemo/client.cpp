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
int main() {
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
	serv_addr.sin_port = htons(1234);
	//int connect(int sock, struct sockaddr *serv_addr, socklen_t addrlen);
	connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	//read data from server
	char buffer[24];
	//ssize_t read(int fd, void *buf, size_t nbytes);
	read(sock, buffer, sizeof(buffer)-1);

	printf("Message from server: %s\n", buffer);

	close(sock);

	return 0;
}
