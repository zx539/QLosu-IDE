#include "win_sock.h"

#ifdef _WIN32

int WinSockInited = 0; //保存初始化信息，已初始化则为1，否则为0

//辅助函数
bool LosuSock_init()
{
    /*
        winsock库需要特殊的初始化
    */
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsdata;

    if (WSAStartup(sockVersion, &wsdata) == 0)
        return true;
    return false;
}

gethostbyname_t *LosuSock_gethostbyname(const char *name)
{
    if (!WinSockInited)
        WinSockInited = LosuSock_init();
    gethostbyname_t *output;
    output = gethostbyname(name);
    return output;
}

bool LosuSock_sendtimeout(socket_t *sock, double sec)
{
    int _sec;
    int usec;
    usec = modf(sec, (void*)&_sec) * 1000000;

    struct timeval timeout;
    timeout.tv_sec = _sec;
    timeout.tv_usec = usec;

    if (setsockopt(sock->sockfd, SOL_SOCKET, SO_SNDTIMEO, (void *)&timeout, sizeof(timeout)) == 0)
        return true;
    return false;
}

bool LosuSock_recvtimeout(socket_t *sock, double sec)
{
    int _sec;
    int usec;
    usec = modf(sec, (void*)&_sec) * 1000000;

    struct timeval timeout;
    timeout.tv_sec = _sec;
    timeout.tv_usec = usec;

    if (setsockopt(sock->sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, sizeof(timeout)) == 0)
        return true;
    return false;
}

//socket操作

socket_t *LosuSock_socket(int family, int type, int protocol)
{
    /*
        创建一个套接字
    */
    if (!WinSockInited)
        LosuSock_init();

    socket_t *sock = malloc(sizeof(socket_t));
    sock->sockfd = socket(family, type, protocol);
    sock->family = family;
    sock->type = type;
    sock->protocol = protocol;
    sock->addr.sin_family = family;

    return sock;
}

bool LosuSock_bind(socket_t *sock, const char *ip, int port)
{
    /*
        将套接字和对应的ip和端口相绑定
    */
    sock->addr.sin_port = htons(port);

    if (ip == NULL)
        //ip缺省时的情况
        sock->addr.sin_addr.S_un.S_addr = INADDR_ANY;
    else
        sock->addr.sin_addr.S_un.S_addr = inet_addr(ip);

    if (bind(sock->sockfd, (void*)&sock->addr, sizeof(sock->addr)) != SOCKET_ERROR)
        return true;
    return false;
}

bool LosuSock_listen(socket_t *sock, int amount)
{
    /*
        监听
    */
    if (listen(sock->sockfd, amount) != SOCKET_ERROR)
        return true;
    return false;
}

bool LosuSock_connect(socket_t *sock, const char *ip, int port)
{
    /*
        连接
    */
    sock->addr.sin_port = htons(port);
    sock->addr.sin_addr.S_un.S_addr = inet_addr(ip);
    int length = sizeof(sock->addr);

    if (connect(sock->sockfd, (void*)&sock->addr, length) == SOCKET_ERROR)
        return false;
    return true;
}

socket_t *LosuSock_accept(socket_t * sock)
{
    socket_t *client = malloc(sizeof(socket_t));
    int length = sizeof(client->addr);
    client->sockfd = accept(sock->sockfd, (void*)&client->addr, &length);
    client->family = sock->family;
    client->type = sock->type;
    client->protocol = sock->protocol;
    return client;
}

bool LosuSock_send(socket_t *sock, const char *msg, int length, int x)
{
    if (send(sock->sockfd, msg, length, x) != SOCKET_ERROR)
    {
        return true;
    }
    return false;
}

bool LosuSock_recv(socket_t *sock, char *buffer, int length, int x)
{
    recv(sock->sockfd, buffer, length, x);
    return true;
}

bool LosuSock_close(socket_t *sock)
{
    /*
        关闭
    */
    if (closesocket(sock->sockfd) == 0)
    {
        free(sock);
        return true;
    }
    free(sock);
    return false;
}

#endif
