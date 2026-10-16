/*
MIT License

Copyright (c) 2023-2024 Matriller

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
    .text
    .globl lsffi_cdel_intargs6
lsffi_cdel_intargs6:
    movq %rcx, %rbx     //rbx保存前6个参数指针
    movq %rdx, %r11     //r11保存函数指针
    
    movq 0(%rbx), %rcx
    movq 8(%rbx), %rdx
    movq 16(%rbx), %r8
    movq 24(%rbx), %r9

    movq 32(%rbx), %r12     //X86架构不允许内存直接直接传递数据
    movq %r12, -16(%rsp)    
    movq 40(%rbx), %r12
    movq %r12, -8(%rsp)

    subq $48, %rsp
    call *%r11
    addq $48, %rsp

    ret

uint64_numlist_reverse:
    /*
        void numlist_reverse(uint64_t *num, uint64_t len);

        Linux cdel调用约定:
            num参数储存在rdi中
            len参数储存在rsi中
        
        原理即交换内存实现数组反转.如有一个名为a,长度为n的int数组,用C实现如下:
            for (int i = 0; i < n / 2; i ++)
            {
                int mid = a[i];
                a[i] = a[n - i - 1];
                a[n - i - 1] = mid;
            }
        
        汇编中使用三个寄存器实现这个算法
            一是基址寄存器,用于保存数组的基址
            二是前偏移寄存器,用于保存较小的偏移
            三是后偏移寄存器,用于保存较大的偏移
        每次循环时,将基址和前偏移相加,读取前数。之后将基址与后偏移相加，读取后数。
        将前数存到后数的内存区，将后数存到前数的内存区，完成一轮交换。之后将前偏移值加8字节，
        将后偏移值加8字节。
        循环退出的条件是前偏移地址大于等于后偏移地址。大于即n为偶数的情况，此时最中间的数不用移动。
        等于即n为奇数的情况，此时所有数均已交换，并且没有最中间的数。
    */
    cmp $0, %rsi                //检查len参数是否为0,是则直接返回函数
    je .LUINT64_NUMLIST_REVERSE_L1_END

    /*
        这段代码中的基址寄存器是rdi,前偏移寄存器是rcx,后偏移寄存器是rsi
    */

    dec %rsi                    //%rsi中储存着len参数
    imulq $8, %rsi              //这一条指令和上一条是在计算最后一个数字的偏移量,减去1是避免差一错误
    movq $0, %rcx               //rcx用于计数
.LUINT64_NUMLIST_REVERSE_L1:
    movq (%rdi, %rcx), %r14     //读取前数
    movq (%rdi, %rsi), %r15     //读取后数
    movq %r15, (%rdi, %rcx)     //后数存至前数内存区域
    movq %r14, (%rdi, %rsi)     //前数存至后数内存区域
    addq $8, %rcx               //前偏移地址加8字节
    subq $8, %rsi               //后偏移地址减8字节
    cmp %rsi, %rcx              //比较前指针和后指针的值,如果前指针大于或等于后指针,则
    jl .LUINT64_NUMLIST_REVERSE_L1
.LUINT64_NUMLIST_REVERSE_L1_END:
    ret


    .globl lsffi_cdel_intargsx__
lsffi_cdel_intargsx__:
    movq %rcx, %rbx     //rbx保存前6个参数指针
    movq %rdx, %r10     //r10保存其余参数指针
    movq %r8, %r11      //r11保存函数指针
    movq %r9, %r12      //r12保存其余参数个数

    //调用uint64_numlist_reverse函数
    movq %r10, %rdi
    movq %r12, %rsi
    call uint64_numlist_reverse

    //调整rsp位置
    movq $6, %r15
    addq %r12, %r15
    imulq $8, %r15
    subq %r15, %rsp

    //检查是否有要通过栈传递的参数
    cmp $0, %r12
    je .L1_END

    //通过栈传递参数
    movq %r12, %rcx
    movq $0, %r13       //相对r10的参数寻址偏移
    movq %r15, %r14     //参数的目的偏移，即在栈中的偏移
.L1:
    movq (%r10, %r13), %rdx     //从内存读取参数
    addq $8, %r13               //增加参数的源内存偏移
    subq $8, %r14
    movq %rdx, (%rsp, %r14)     //将参数移入栈中
    loop .L1                    //循环
.L1_END:

    //按寄存器传递的参数
    movq 0(%rbx), %rcx
    movq 8(%rbx), %rdx
    movq 16(%rbx), %r8
    movq 24(%rbx), %r9

    movq 32(%rbx), %r12     //X86架构不允许内存直接直接传递数据
    movq %r12, 32(%rsp)    
    movq 40(%rbx), %r12
    movq %r12, 40(%rsp)

    call *%r11          //函数调用
    addq %r15, %rsp      //恢复栈桢

    ret

/*
    lsffi_cdel_floatargs8
*/

    .globl lsffi_cdel_floatargs8
lsffi_cdel_floatargs8:
    //保存数据
    movq %rcx, %rbx     //rbx存储参数指针
    movq %rdx, %r10     //r10存储函数指针

    //传参
    movq 56(%rbx), %xmm7
    movq 48(%rbx), %xmm6
    movq 40(%rbx), %xmm5
    movq 32(%rbx), %xmm4
    movq 24(%rbx), %xmm3
    movq 16(%rbx), %xmm2
    movq 8(%rbx), %xmm1
    movq 0(%rbx), %xmm0

    movq %r10, %rax

    //调整栈帧
    subq $64, %rsp

    //函数调用
    call *%r10

    //调整栈帧
    addq $64, %rsp
    
    ret
