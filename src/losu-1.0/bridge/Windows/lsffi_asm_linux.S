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

    /*
        出于性能考虑,此处采用汇编代码
    */

    .text

/*
    lsffi_cdel_intargs6
*/
    .globl lsffi_cdel_intargs6
    .type llsffi_cdel_intargs6, @function
lsffi_cdel_intargs6:
    /*
        uint64_t lsffi_cdel_intargs6(uint64_t args[6], void *fun);
    */
    endbr64
    movq %rdi, %rbx     //将参数指针保存在rbx中
    movq %rsi, %r15     //将函数指针保存在r15中

    //传参
    movq 0(%rbx), %rdi  //第一个参数,剩下的以此类推
    movq 8(%rbx), %rsi
    movq 16(%rbx), %rdx
    movq 24(%rbx), %rcx
    movq 36(%rbx), %r8
    movq 40(%rbx), %r9

    call *%r15          //函数调用
    ret                 //返回

    /*
        按寄存器传递的参数，传递顺序在Linux和Windows平台是不同的
    */

/*
    uint64_numlist_reverse
*/

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
    endbr64                     //Linux程序中得有这行代码

    cmp $0, %rsi                //检查len参数是否为0,是则直接返回函数
    je ..LUINT64_NUMLIST_REVERSE_L1_END

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
..LUINT64_NUMLIST_REVERSE_L1_END:
    ret

/*
        lsffi_cdel_intargsx__维护须知
            1. 在amd64计算机中，无论整形参数长度如何，都使用pushq,即压入8字节
            2. 在ret之前，要调用leave清理栈桢
            3. 在leave之前，要将rsp加上向栈中压入的参数的总字节数，即压入的参数的8倍
            4. leave命令之前不可删去pushq %rbp和movq %rsp, %rbp两条指令，会导致程序异常
*/

    .globl lsffi_cdel_intargsx__
    .type lsffi_cdel_intargsx__, @function
lsffi_cdel_intargsx__:
    /*
        uint64_t lsffi_cdel_intargsx(uint64_t args[6], uint64_t *eargs, void *fun, uint64_t len);
    */
    endbr64
    pushq %rbp
    movq %rsp, %rbp

    movq %rdi, %r10     //r10保存前6个参数指针
    movq %rsi, %r11     //r11保存其余参数指针
    movq %rdx, %r12     //r12保存函数指针
    movq %rcx, %r13     //r13保存其余参数个数

    //调用uint64_numlist_reverse函数
    movq %r11, %rdi     //要反转的数组基址
    movq %r13, %rsi     //元素个数
    call uint64_numlist_reverse //函数调用

    //压栈传入的参数
    movq %r13, %rcx
    movq $0, %r14       //相对r11的寻址偏移
.L1:
    movq (%r11, %r14), %r15
    pushq %r15
    addq $8, %r14       //每循环一次，r14加8字节，即相对r11多偏移8字节，即访问下一个64位数
    loop .L1

    //按寄存器传递的参数
    movq 0(%r10), %rdi  //第一个参数,剩下的以此类推
    movq 8(%r10), %rsi
    movq 16(%r10), %rdx
    movq 24(%r10), %rcx
    movq 32(%r10), %r8
    movq 40(%r10), %r9
    call *%r12          //函数调用

    //清理栈桢
    imulq $8, %r13      //计算压栈传参的字节数
    addq %r13, %rsp     //向rsp加上该字节数
    leave               //清理栈桢
    ret                 //函数返回


/*
    lsffi_cdel_floatargs8
*/

    .globl lsffi_cdel_floatargs8
    .type lsffi_cdel_floatargs8, @function
lsffi_cdel_floatargs8:
    endbr64
    //保存数据
    movq %rdi, %rbx     //rbx存储参数指针
    movq %rsi, %rax     //rax存储函数指针

    //传参
    movq 56(%rbx), %xmm7
    movq 48(%rbx), %xmm6
    movq 40(%rbx), %xmm5
    movq 32(%rbx), %xmm4
    movq 24(%rbx), %xmm3
    movq 16(%rbx), %xmm2
    movq 8(%rbx), %xmm1
    movq 0(%rbx), %xmm0

    //函数调用
    call *%rax
    ret
