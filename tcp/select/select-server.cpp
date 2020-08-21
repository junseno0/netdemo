#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MYPORT 1234    // the port users will be connecting to

#define BACKLOG 5     // how many pending connections queue will hold

#define BUF_SIZE 200

int fd_clients[BACKLOG];    // accepted connection fd
int conn_amount;    // current connection amount


/*
 * https://www.cnblogs.com/faraway/archive/2009/03/06/1404449.html
 *使用select函数可以以非阻塞的方式和多个socket通信。
程序只是演示select函数的使用，功能非常简单，即使某个连接关闭
以后也不会修改当前连接数，连接数达到最大值后会终止程序。

1. 程序使用了一个数组fd_clients，通信开始后把需要通信的多个socket描述符都放入此数组。
2. 首先生成一个叫sock_fd的socket描述符，用于监听端口。

3. 将sock_fd和数组fd_clients中不为0的描述符放入select将检查的集合fdsr。
4. 处理fdsr中可以接收数据的连接。如果是sock_fd，表明
有新连接加入，将新加入连接的socket描述符放置到fd_clients。
 * */
void showclient()
{
    int i;
    printf("client amount: %d\n", conn_amount);
    for (i = 0; i < BACKLOG; i++) {
        printf("[%d]:%d  ", i, fd_clients[i]);
    }
    printf("\n\n");
}

int main(void)
{
    int sock_fd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct sockaddr_in server_addr;    // server address information
    struct sockaddr_in client_addr; // connector's address information
    socklen_t sin_size;
    int yes = 1;
    char buf[BUF_SIZE];
    int ret;
    int i;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    
    server_addr.sin_family = AF_INET;         // host byte order
    server_addr.sin_port = htons(MYPORT);     // short, network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sock_fd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("listen port %d\n", MYPORT);

    fd_set fdsr;
    int maxsock;
    struct timeval tv;

    conn_amount = 0;
    sin_size = sizeof(client_addr);
    maxsock = sock_fd;

	//kay: NIO select
    while (1) {
        // initialize file descriptor set
        FD_ZERO(&fdsr);
        FD_SET(sock_fd, &fdsr);

        // timeout setting
        tv.tv_sec = 30;
        tv.tv_usec = 0;

        // add active connection to fd set
        for (i = 0; i < BACKLOG; i++) {
            if (fd_clients[i] != 0) {
                FD_SET(fd_clients[i], &fdsr);
            }
        }

		//kay: select if any fd ready
        ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
        if (ret < 0) {
            perror("select");
            break;
        } else if (ret == 0) {
            printf("timeout\n");
            continue;
        }

        // check every fd in the set
        for (i = 0; i < conn_amount; i++) {
            if (FD_ISSET(fd_clients[i], &fdsr)) {
                ret = recv(fd_clients[i], buf, sizeof(buf), 0);
                if (ret <= 0) {        // client close
                    printf("client[%d] close\n", i);
                    close(fd_clients[i]);
                    FD_CLR(fd_clients[i], &fdsr);
                    fd_clients[i] = 0;
                } else {        // receive data
                    if (ret < BUF_SIZE)
                        memset(&buf[ret], '\0', 1);
                    printf("client[%d] send:%s\n", i, buf);
                }
            }
        }

        // check whether a new connection comes
        if (FD_ISSET(sock_fd, &fdsr)) {
            new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);
            if (new_fd <= 0) {
                perror("accept");
                continue;
            }

            // add to fd queue
            if (conn_amount < BACKLOG) {
                fd_clients[conn_amount++] = new_fd;
                printf("new connection client[%d] %s:%d\n", conn_amount,
                        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                if (new_fd > maxsock)
                    maxsock = new_fd;
            }
            else {
                printf("max connections arrive, exit\n");
                send(new_fd, "bye", 4, 0);
                close(new_fd);
                break;
            }
        }
        showclient();
    }

    // close other connections
    for (i = 0; i < BACKLOG; i++) {
        if (fd_clients[i] != 0) {
            close(fd_clients[i]);
        }
    }

    exit(0);
}
