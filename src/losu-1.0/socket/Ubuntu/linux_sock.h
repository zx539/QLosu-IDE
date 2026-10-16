#ifndef LOSU_LINUX_SOCK_H__
#define LOSU_LINUX_SOCK_H__

#ifndef __linux__
#warning "该文件用于Linux系统"
#endif

#ifdef __linux__

#include <sys/socket.h>
#include <sys/time.h>  //struct timeval
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>  //gethostbyname

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct sockaddr_in sock_addr_t;

typedef struct
{
    int sockfd;
    int family;
    int type;
    int protocol;
    sock_addr_t addr;
} socket_t;

typedef struct hostent gethostbyname_t;

//辅助函数
gethostbyname_t *LosuSock_gethostbyname(const char *name);
bool LosuSock_sendtimeout(socket_t *sock, double sec);
bool LosuSock_recvtimeout(socket_t *sock, double sec);

//socket操作函数
socket_t *LosuSock_socket(int family, int type, int protocol);
bool LosuSock_bind(socket_t *sock, const char *ip, int port);
bool LosuSock_listen(socket_t *sock, int amount);
socket_t *LosuSock_accept(socket_t *sock);
bool LosuSock_connect(socket_t *sock, const char *ip, int port);
bool LosuSock_send(socket_t *sock, const char *data, int length, int x);
bool LosuSock_recv(socket_t *sock, char *buffer, int length, int x);
bool LosuSock_close(socket_t *sock);

#endif

#endif