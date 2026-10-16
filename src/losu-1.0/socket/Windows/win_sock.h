#ifndef LOSU_WIN_SOCK_H__
#define LOSU_WIN_SOCK_H__

#ifndef _WIN32
    #warning "该文件用于Windows系统"
#endif


#ifdef _WIN32

#include "els.h"

#include <winsock2.h>
#include <ws2tcpip.h> //包含inet_ntop

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

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
bool LosuSock_init();
gethostbyname_t *LosuSock_gethostbyname(const char *name);
bool LosuSock_sendtimeout(socket_t *sock, double sec);
bool LosuSock_recvtimeout(socket_t *sock, double sec);

//socket操作
socket_t *LosuSock_socket(int family, int type, int prototol);
bool LosuSock_bind(socket_t *sock, const char *ip, int port);
bool LosuSock_listen(socket_t *sock, int amount);
socket_t *LosuSock_accept(socket_t *sock);
bool LosuSock_connect(socket_t *sock, const char *ip, int port);
bool LosuSock_send(socket_t *sock, const char *msg, int length, int x);
bool LosuSock_recv(socket_t *sock, char *buffer, int length, int x);
bool LosuSock_close(socket_t *sock);
#endif

#endif