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

#define BUF_SIZE 1024
int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("Usage: %s. <ip> <port> \n", argv[0]);
		exit(1);
	}
	char *ip = argv[1];
	int port = atoi(argv[2]);
	printf("ip %s. port %d.\n", ip, port);
	char filename[100] = {0};
	printf("Input filename to save:");
	scanf("%s", filename);
	FILE* fp = fopen(filename, "wb");
	if(!fp) {
		printf("Cannot open file, press any key to exit!\n");
		return 0;
	}
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;//use IPv4
	//serv_addr.sin_addr.s_addr = inet_addr("193.112.203.56");//IP addr
	//serv_addr.sin_port = htons(1238);
	serv_addr.sin_addr.s_addr = inet_addr(ip);//IP addr
	serv_addr.sin_port = htons(port);
	connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	char buffer[BUF_SIZE];
	size_t nCount = 0;
	while(nCount = read(sock, buffer, BUF_SIZE) > 0) {
		printf("write %lu bytes.", nCount);
		fwrite(buffer, nCount, 1, fp);
	}
	printf("File transfer success!\n");

	fclose(fp);
	close(sock);

	return 0;
}
