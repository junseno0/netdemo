#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	/*
	 *struct sockaddr_in{
        sa_family_t     sin_family;   //地址族（Address Family），也就是地址类型
        uint16_t        sin_port;     //16位的端口号
   	    struct in_addr  sin_addr;     //32位IP地址
    	char            sin_zero[8];  //不使用，一般用0填充
	  };
	 * */
	/*
	 *struct in_addr {
	 	in_addr_t s_addr;//32bit IP addr, in_addr_t = unsigned long	
	 }
	 * */
	/*
	 *struct sockaddr {
	 	sa_family_t sin_family;
		char sa_data[14];
	 }
	 * */
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;//IPV4 addr
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//IP addr
	serv_addr.sin_port = htons(1234);//port

	//int bind(int sock, struct sockaddr *addr, socklen_t addrlen);  //struct sockaddr
	bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	//wait client request
	//int listen(int sock, int backlog);
	//backlog -> SOMAXCONN, by system, about 100+kb in Request Queue
	listen(serv_sock, 20);

	//receive client request
	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);
	//int accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
	//Block IO
	int client_sock = accept(serv_sock, (struct sockaddr*) &client_addr,
			&client_addr_size);

	//send data to client
	char str[] = "Hello kayson!";
	//ssize_t write(int fd, const void *buf, size_t nbytes);
	write(client_sock, str, sizeof(str));

	//close socket
	close(client_sock);
	close(serv_sock);

	return 0;
}
