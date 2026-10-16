#include "linux_sock.h"

#ifdef __linux__

//辅助函数
gethostbyname_t *LosuSock_gethostbyname(const char *name)
{
    return gethostbyname(name);
}

bool LosuSock_sendtimeout(socket_t *sock, double sec)
{
    int _sec;
    int usec;
    usec = modf(sec, (void*)&_sec) * 1000000;

    struct timeval timeout; //<sys/time.h>
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


//socket操作函数
socket_t *LosuSock_socket(int family, int type, int protocol)
{
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
    sock->addr.sin_port = htons(port);

    if (ip==NULL)
        sock->addr.sin_addr.s_addr = INADDR_ANY;
    else
        sock->addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(sock->sockfd, (void*)&sock->addr, sizeof(sock->addr)) == 0)
        return true;
    return false;
};

bool LosuSock_listen(socket_t *sock, int amount)
{
    if (listen(sock->sockfd, amount) == 0)
        return true;
    return false;
}

socket_t *LosuSock_accept(socket_t *sock)
{
    socket_t *client = malloc(sizeof(socket_t));
    int length = sizeof(client->addr);

    client->family = sock->family;
    client->type = sock->type;
    client->protocol = sock->protocol;
    client->addr.sin_family = sock->family;
    
    client->sockfd = accept(sock->sockfd, (void*)&client->addr, (void*)&length);
    return client;
}

bool LosuSock_connect(socket_t *sock, const char *ip, int port)
{
    sock->addr.sin_port = htons(port);
    sock->addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock->sockfd, (struct sockaddr*)&sock->addr, sizeof(sock->addr)) == 0)
        return true;
    return false;
}

bool LosuSock_send(socket_t *sock, const char *data, int length, int x)
{
    if (send(sock->sockfd, data, length, x) != -1);
        return true;
    return false;
}

bool LosuSock_recv(socket_t *sock, char *buffer, int length, int x)
{
    if (recv(sock->sockfd, buffer, length, x) >= 0)
        return true;
    return false;
}

bool LosuSock_close(socket_t *sock)
{
    if (close(sock->sockfd) != -1)
    {
        free(sock);
        return true;
    }
    free(sock);
    return false;
}



#endif