#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#define BUF_SIZE 1024
//获取文件的大小
int get_file_size(int f)
{
  struct stat st;
  fstat(f, &st);
  return st.st_size;
}

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("Usage: %s. <ip> <port> \n", argv[0]);
		exit(1);
	}
	char *ip = argv[1];
	int port = atoi(argv[2]);
	printf("ip %s. port %d.\n", ip, port);
	char filename[100] = {0};
	printf("Input filename to transmit:\n");
	//scanf("%s", filename);
	FILE* fp = fopen("/home/ubuntu/client.cpp", "r");
	if(!fp) {
		printf("Cannot open file, press any key to exit!\n");
		return 0;
	}
	size_t nCount = 0;
	char buffer[BUF_SIZE] = {0};
	int res = -1;
	/*
	fseek(fp, 0, SEEK_SET);
	int fd = open("/home/ubuntu/client.cpp",O_RDWR);
	printf("file size %d.", get_file_size(fd));
	close(fd);
	nCount = fread(buffer, 1, BUF_SIZE, fp);
	printf("read count %lu.\n", nCount);
	nCount = fread(buffer, BUF_SIZE, 1, fp);
	printf("read count %lu.\n", nCount);
	*/
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;//IPV4 addr
	//serv_addr.sin_addr.s_addr = inet_addr("193.112.203.56");//IP addr
	//serv_addr.sin_port = htons(1238);//port
	serv_addr.sin_addr.s_addr = inet_addr(ip);//IP addr
	serv_addr.sin_port = htons(port);//port

	res = bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if(res) {
		printf("bind failed. errno %d.\n", errno); return 0;
	}

	res = listen(serv_sock, BUF_SIZE);
	if(res) {
		printf("listen failed. errno %d.\n", errno); return 0;
	}

	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);
	int client_sock = accept(serv_sock, (struct sockaddr*) &client_addr,
			&client_addr_size);
	printf("accepted. client sock: %d.\n", client_sock);
	while(nCount = fread(buffer, 1, BUF_SIZE, fp) > 0) {
		printf("read and write %lu bytes.\n", nCount);
		write(client_sock, buffer, nCount);
	}
	printf("write end. to shutdown and close.\n");
	//SHUT_RD, SHUT_WR, SHUT_RDWR
	shutdown(client_sock, SHUT_WR);//Until all buffer sent, then send FIN packet.
	read(client_sock, buffer, BUF_SIZE);//Until client receive all

	close(client_sock);
	close(serv_sock);
	
	fclose(fp);
	return 0;
}
