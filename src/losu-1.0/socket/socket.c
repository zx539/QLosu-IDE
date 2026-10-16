#include "socket.h"

#ifdef _WIN32
    #include "win_sock.h"
    #include <winsock2.h>
    #include <wsrm.h>
#endif

#ifdef __linux__
    #include "linux_sock.h"
#endif

/*
    各结构应有原型
        typedef struct sockaddr_in sock_addr_t;

        typedef struct
        {
            int sockfd;            #网络套接字的描述符
            int family;            #该套接字的地址族
            int type;              #该套接字的类型
            int protocol;          #该套接字的协议
            sock_addr_t addr;      #该套接字的地址或是对面套接字的地址
        } socket_t;

        typedef struct{
            char  *h_name;         #标准主机名          
            char **h_aliases;      #主机的别名
            int    h_addrtype;     #主机ip地址的类型，只会是AF_INET
            int    h_length;       #主机地址的长度
            char **h_addr_list;    #主机的ip地址,是网络字节序,需要通过inet_ntop函数转换 
        } gethostbyname_t;

    各函数应有原型
        //辅助函数
        gethostbyname_t *LosuSock_gethostbyname(const char *name);
        bool LosuSock_sendtimeout(socket_t *sock, double sec);
        bool LosuSock_recvtimeout(socket_t *sock, double sec);

        //socket操作
        socket_t *LosuSock_socket(int family, int type, int protocol);
        int LosuSock_bind(socket_t *sock, const char *ip, int port);
        int LosuSock_listen(socket_t *sock, int amount);
        int LosuSock_connect(socket_t *sock, const char *ip, int port);
        socket_t *LosuSock_accept(socket_t *sock);
        int LosuSock_send(socket_t *sock, const char *msg, int length, int x);
        int LosuSock_recv(socket_t *sock, char *buffer, int length, int x);
        int LosuSock_close(socket_t *sock);
*/

int ELSAPI_socket_c_AF_INET(els_VmObj* vm)
{
    arg_returnnum(vm, AF_INET);
    return 1;
}

int ELSAPI_socket_c_SOCK_STREAM(els_VmObj* vm)
{
    arg_returnnum(vm, SOCK_STREAM);
    return 1;
}

#define PUTN(s, n) vm_setval(vm, s, obj_newnum(vm, n))

int ELSAPI_socket_c_constinit(els_VmObj* vm) // c_constinit()
{
    /*
        用于将一些常量，如AF_INET放入全局作用域中
    */
    //地址系列常量
    PUTN("AF_UNSPEC", AF_UNSPEC);
    PUTN("AF_INET", AF_INET);
    PUTN("AF_IPX", AF_IPX);
    PUTN("AF_APPLETALK", AF_APPLETALK);
    PUTN("AF_NETBIOS", AF_NETBIOS);
    PUTN("AF_INET6", AF_INET6);
    PUTN("AF_IRDA", AF_IRDA);
    PUTN("AF_BTH", AF_BTH);

    //套接字类型系列常量
    PUTN("SOCK_STREAM", SOCK_STREAM);
    PUTN("SOCK_DGRAM", SOCK_DGRAM);
    PUTN("SOCK_RAW", SOCK_RAW);
    PUTN("SOCK_RDM", SOCK_RDM);
    PUTN("SOCK_SEQPACKET", SOCK_SEQPACKET);

    //套接字协议系列常量
    PUTN("IPPROTO_ICMP", IPPROTO_ICMP);
    PUTN("IPPROTO_IGMP", IPPROTO_IGMP);
    PUTN("IPPROTO_TCP", IPPROTO_TCP);
    PUTN("IPPROTO_UDP", IPPROTO_UDP);
    PUTN("IPPROTO_ICMPV6", IPPROTO_ICMPV6);
    PUTN("IPPROTO_RM", IPPROTO_RM);

    return 0;
}

int ELSAPI_socket_c_gethostbyname(els_VmObj *vm)
{
    /*
        # 参数
            1. name 要获取的主机名称

        #返回值
            1. hosts 获取的结果
    */

    const char *name;
    gethostbyname_t *result = NULL;
    int index = 0;
    char buffer[128];

    //将结果以单元的形式返回
    LosuObj unit_obj = obj_newunit(vm);
    LosuObj aliases = obj_newunit(vm); //存放别称的单元
    LosuObj addr_list = obj_newunit(vm);

    /*
        void obj_setunit(els_VmObj*vm,LosuObj unit,LosuObj key,LosuObj value);
        设定一个对象指定下标的值
    */



    if (arg_gettype(vm, 1)!=ELS_API_TYPE_STRING || strlen((name=arg_getstr(vm, 1)))==0 || (result = LosuSock_gethostbyname(name))==NULL)
    {
        //如果获取失败，则返回
        //此时
        arg_returnnull(vm);
        return 1;
    }

    obj_setunit(
        //将存放别称的单元放入aliases键中
        vm, unit_obj,
        obj_newstr(vm, "aliases"), aliases
    );

    obj_setunit(
        //先将addr_list放入单元
        vm, unit_obj,
        obj_newstr(vm, "addr_list"), addr_list
    );

    obj_setunit(
        //放入标准主机名
        vm, unit_obj,
        obj_newstr(vm, "name"), obj_newstr(vm, result->h_name)
    );

    for (char **name=result->h_aliases; ; name++)
    {
        if (*name==NULL)
            break;

        //将别称放入unit.aliases中
        obj_setunit(
            vm, aliases,
            obj_newnum(vm, index), obj_newstr(vm, *name)
        );

        index++;
    }

    obj_setunit(
        //将主机地址的类型放入addrtype中
        vm, unit_obj,
        obj_newstr(vm, "addrtype"), obj_newnum(vm, result->h_addrtype)
    );

    obj_setunit(
        //将主机地址的长度放入length中
        vm, unit_obj,
        obj_newstr(vm, "length"), obj_newnum(vm, result->h_length)
    );

    index = 0;
    for (char **ip=result->h_addr_list; ;ip++)
    {
        //将ip转化后放入unit.addr_list
        if (*ip==NULL)
            break;

        inet_ntop(result->h_addrtype, *ip, buffer, sizeof(buffer));

        obj_setunit(
            vm, addr_list,
            obj_newnum(vm, index), obj_newstr(vm, buffer)
        );

        index++;
    }

    arg_return(vm, unit_obj);
    return 1;
}

int ELSAPI_socket_c_sendtimeout(els_VmObj *vm)
{
    /*
        # 参数
            1.sock
            2.sec    要设置的时间，单位为秒
    */
    socket_t *sock = (void*)arg_getptr(vm, 1);
    double sec = arg_getnum(vm, 2);

    if (LosuSock_sendtimeout(sock, sec))
        arg_returnnum(vm, 1);
    else
        arg_returnnull(vm);
    return 1;
}
int ELSAPI_socket_c_recvtimeout(els_VmObj *vm)
{
    /*
        # 参数
            1.sock
            2.sec    要设置的时间，单位为秒
    */
    socket_t *sock = (void*)arg_getptr(vm, 1);
    double sec = arg_getnum(vm, 2);

    if (LosuSock_recvtimeout(sock, sec))
        arg_returnnum(vm, 1);
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_socket_c_socket(els_VmObj* vm)
{
    /*

        # 参数
            1.family
            2.type
            3.protocol
        
        # 返回值
            1.sockfd
    */
    int family = arg_getnum(vm, 1);
    int type = arg_getnum(vm, 2);
    int protocol = arg_getnum(vm, 3);

    socket_t *sockfd = LosuSock_socket(family, type, protocol);
    arg_returnptr(vm, (void*)sockfd);

    return 1;
}

int ELSAPI_socket_c_bind(els_VmObj* vm)
{
    /*
        # 参数
            1.sock
            2.ip
            3.port
    
        # 返回值
            成功时返回1
            失败时返回null
    */
    socket_t *sock = (void*)arg_getptr(vm, 1);
    const char *ip = arg_getstr(vm, 2);
    int port = arg_getnum(vm, 3);

    if (LosuSock_bind(sock, ip, port))
        arg_returnnum(vm, 1);
    else
        arg_returnnull(vm);

    return 1;
}

int ELSAPI_socket_c_listen(els_VmObj* vm)
{
    /*
        # 参数
            1.sock
            2.amount
        
        # 返回值
            成功时返回1
            失败时返回null
    */
    socket_t * sock = (void*)arg_getptr(vm, 1);
    int amount = arg_getnum(vm, 2);

    if (LosuSock_listen(sock, amount))
        arg_returnnum(vm, 1);
    else
        arg_returnnull(vm);

    return 1;
}

int ELSAPI_socket_c_accept(els_VmObj* vm)
{
    /*
        # 参数
            1.sock
        
        # 返回值
            1.client
    */
    socket_t *sock = (void*)arg_getptr(vm, 1);

    arg_returnptr(vm, (void*)LosuSock_accept(sock));
    return 1;   
}

int ELSAPI_socket_c_connect(els_VmObj* vm)
{
    /*
        参数:
            1.sock
            2.ip
            3.port
        
        返回值:
            成功时返回1
            失败时返回null
    */
    socket_t *sock = (void*)arg_getptr(vm, 1);
    const char *ip = arg_getstr(vm, 2);
    int port = arg_getnum(vm, 3);

    if (LosuSock_connect(sock, ip, port))
        arg_returnnum(vm, 1);
    else
        arg_returnnull(vm);

    return 1;
}

int ELSAPI_socket_c_send(els_VmObj* vm)
{
    /*
        参数:
            1.sock
            2.data
    */
    socket_t *sock = (void*)arg_getptr(vm, 1);
    const char *data = "";

    if (arg_gettype(vm, 2) == ELS_API_TYPE_STRING)
        data = arg_getstr(vm, 2);

    LosuSock_send(sock, data, strlen(data), 0);
    return 1;
}

int ELSAPI_socket_c_recv(els_VmObj* vm)
{
    /*
        参数:
            1.sockfd
            2.amount
    */
    socket_t *sock = (void*)arg_getptr(vm, 1);
    int amount = arg_getnum(vm, 2);
    char *tmp = calloc(sizeof(char), amount + 1);

    LosuSock_recv(sock, tmp, amount, 0);
    arg_returnstr(vm, tmp);
    free(tmp);
    return 1;
}

int ELSAPI_socket_c_close(els_VmObj* vm)
{
    /*
        参数:
            1.sockfd
    */
    socket_t *sock = (void*)arg_getptr(vm, 1);
    LosuSock_close(sock);
    return 1;
}
