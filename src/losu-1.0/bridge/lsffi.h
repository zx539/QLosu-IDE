#ifndef LSFFI_H_
#define LSFFI_H_

#include <stdint.h>

#define LSFFI_INTRET    0
#define LSFFI_FLOATRET  1

/*
    CDEL部分

    CDEL在Linux的整形传参顺序:
        参数顺序(按C语言参数列表顺序从左到右)    传入的寄存器
            1                                       rdi
            2                                       rsi
            3                                       rdx
            4                                       rcx
            5                                       r8
            6                                       r9
        
        6个以上参数通过压栈传递，而且是从右自左压栈
        如有如下几个要压栈的参数，...是前6个传入寄存器的参数
            (... , 1, 2, 3)
        汇编压栈方式如下
            pushq $3
            pushq $2
            pushq $1
        这样函数在用pop将参数从栈中弹出时，就会将参数顺序恢复到自左向右的状态

    CDEL在Windows的整形传参顺序:
        参数顺序(按C语言参数列表顺序从左到右)    传入的寄存器
            1                                       rcx
            2                                       rdx
            3                                       r8
            4                                       r9
        
        在Windows平台下，CDEL只有前面4个参数是传入寄存器的，其余参数均传入栈中
        Windows平台要求函数调用者调整栈帧位置并传参。如传递如下参数
            (1, 2, 3, 4, 5, 6)
        对应汇编代码如下
            subq $48, %rsp  //调整栈帧位置，即将栈帧减去参数的总字节数，包括传入寄存器的参数

            movq $40, 16(%rsp)  //栈帧自上往下递增，即内存地址在递减
            movq $32, 8(%rsp)
            movq $4, %r9
            movq $3, %r8
            movq $2, %rdx
            movq $1, %rcx

            call FUNCTION       //假设有一个叫FUNCTION的函数
            addq $48, %rsp      //在函数结尾，函数要恢复栈帧
            ret                 //函数返回
*/

uint64_t lsffi_cdel_intargs6(uint64_t args[6], void *fun);
/*
    此函数用于Linux cdel的整形传参,且最多传参数为6个

    @arg args: 
        要传入的参数,是一个长度为6的64位整形数组,不一定是无符号类型
    @arg fun: 
        要调用的函数指针

    @output:
        会将fun的值当做函数指针来调用，返回值即该函数的返回值
*/

uint64_t lsffi_cdel_intargsx(uint64_t args[6], uint64_t *eargs, void *fun, uint64_t len);
/*
    此函数用于Linux cdel的整形传参,无参数传递上限。

    @arg args: 
        要传入的参数,是一个长度为6的64位整形数组,不一定是无符号类型
    @arg eargs:
        额外的参数，当len不为空时才会被使用
    @arg fun: 
        要调用的函数指针
    @arg len:
        额外参数的个数，当无额外参数时需要设为0，否则会引发错误

    @output:
        会将fun的值当做函数指针来调用，返回值即该函数的返回值
*/

double lsffi_cdel_floatargs8(double args[8], void *fun);

#endif