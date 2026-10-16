#include "lsmath.h"
#include <math.h>

#define PI        3.141592653589793
#define E         2.718281828459045
#define GAMMA     0.577215664901532

// # 常量定义
int ELSAPI_lsmath_PI(els_VmObj* vm){
    /*
        返回π（圆周率）的值

        # 参数
        无参数

        # 返回值
            1.π
    */
    arg_returnnum(vm, PI);
    return 1;
}
int ELSAPI_lsmath_E(els_VmObj* vm){
    /*
        返回e（自然常数）的值

        # 参数
        无参数

        # 返回值
            1.e
    */
    arg_returnnum(vm, E);
    return 1;
} 
int ELSAPI_lsmath_GAMMA(els_VmObj* vm){
    /*
        返回gamma（欧拉常数）的值

        # 参数
        无参数

        # 返回值
            1.gamma
    */
    arg_returnnum(vm, GAMMA);
    return 1;
}

// #标准数学函数
//     #杂项函数
int ELSAPI_lsmath_abs(els_VmObj *vm){
    arg_returnnum(vm,fabs(arg_getnum(vm,1)));
    return 1;
}

int ELSAPI_lsmath_mod(els_VmObj *vm){
    arg_returnnum(vm,((long int)arg_getnum(vm,1)%(long int)arg_getnum(vm,2)));
    return 1;
}

int ELSAPI_lsmath_sqrt(els_VmObj *vm){
    arg_returnnum(vm,sqrt(arg_getnum(vm,1)));
    return 1;
}

int ELSAPI_lsmath_anyroot(els_VmObj* vm){
    /*
        若要开num的i次方，即求num的(1 / i)次方
        根据换底公式，num = exp(log(num))
        则num ^ (1 /i) = exp(log(num)) ^ (1 / i) = exp((1 / i) * log(num))

        # 参数
            1. num: number     要被开方的数
            2. i: int          要开的次方
        
        # 返回值
            1. output: float   一个近似根
    */
    if (arg_gettype(vm, 1)==ELS_API_TYPE_NUMBER && arg_gettype(vm, 2)==ELS_API_TYPE_NUMBER)
    {
        //类型检查合格的情况
        double num = arg_getnum(vm, 1);
        double i = (int)arg_getnum(vm, 2);
        double output = exp((1 / i) * log(num));
        
        arg_returnnum(vm, output);
        
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lsmath_gcd(els_VmObj* vm)
{
    /*
        gcd( a:int, b:int ): int                  返回整数a和b的最大公约数

        根据辗转相除法,若f(a, b)表示整数a和b的最大公约数，则有
            f(a, b) = f(b, a mod b)
    */
    if (arg_gettype(vm, 1) == ELS_API_TYPE_NUMBER && arg_gettype(vm, 2) == ELS_API_TYPE_NUMBER)
    {
        long long a = arg_getnum(vm, 1);
        long long b = arg_getnum(vm, 2);
        long long mid;

        while (b!=0)
        {
            mid = a;
            a = b;
            b = mid % b;
        }

        arg_returnnum(vm, a);
    }
    else
        arg_returnnull(vm);
    return 1;
}
//     #三角函数
int ELSAPI_lsmath_acos(els_VmObj *vm){
    arg_returnnum(vm,acos(arg_getnum(vm,1)));
    return 1;
}

int ELSAPI_lsmath_asin(els_VmObj *vm){
    arg_returnnum(vm,asin(arg_getnum(vm,1)));
    return 1;
}

int ELSAPI_lsmath_atan(els_VmObj *vm){
    arg_returnnum(vm,atan(arg_getnum(vm,1)));
    return 1;
}

int ELSAPI_lsmath_cos(els_VmObj *vm){
    arg_returnnum(vm,cos(arg_getnum(vm,1)));
    return 1;
}

int ELSAPI_lsmath_sin(els_VmObj *vm){
    arg_returnnum(vm,sin(arg_getnum(vm,1)));
    return 1;
}

int ELSAPI_lsmath_tan(els_VmObj *vm){
    arg_returnnum(vm,tan(arg_getnum(vm,1)));
    return 1;
}

int ELSAPI_lsmath_exp(els_VmObj *vm){
    arg_returnnum(vm,exp(arg_getnum(vm,1)));
    return 1;
}

int ELSAPI_lsmath_log(els_VmObj *vm){
    /*
        返回以a为底N的对数
        根据换底公式，若f(x)=ln(x),则logaN可表示为
            f(a) / f(N)

        # 参数
            1.a       底数
            2.N       真数
        
        # 返回值
            1.x       对数
    */
    double a = arg_getnum(vm, 1);
    double N = arg_getnum(vm, 2);
    arg_returnnum(vm, log(a) / log(N));
    return 1;
}

int ELSAPI_lsmath_ln(els_VmObj *vm){
    arg_returnnum(vm,log(arg_getnum(vm,1)));
    return 1;
}

int ELSAPI_lsmath_lg(els_VmObj *vm){
    arg_returnnum(vm,log10(arg_getnum(vm,1)));
    return 1;
}

//     #取整函数
int ELSAPI_lsmath_ceil(els_VmObj *vm){
    arg_returnnum(vm,ceil(arg_getnum(vm,1)));
    return 1;
}

int ELSAPI_lsmath_floor(els_VmObj *vm){
    arg_returnnum(vm,floor(arg_getnum(vm,1)));
    return 1;
}

int ELSAPI_lsmath_round(els_VmObj *vm){
    /*
        调用math.h中的double round(double)函数，用于四舍五入
        从洛书处接收要四舍五入的数
    */
    arg_returnnum(vm, (double)round(arg_getnum(vm, 1)));
    return 1;
}

int ELSAPI_lsmath_cut(els_VmObj *vm){
    /*
        截取小数点后若干位的函数

        # 参数
        从洛书处接受两个参数
            1.num         被截取的数
            2.x           要保留的位数
    */
    double num = arg_getnum(vm, 1);
    int x = arg_getnum(vm, 2);
    x = pow(10, x);
    arg_returnnum(vm, (double)(long long)(num * x) / x);
    return 1;
}

//     #位运算
int ELSAPI_lsmath_move_left(els_VmObj* vm)
{
    /*
        # 参数
            1. num: int
            2. n: int
    */
    if (arg_gettype(vm, 1)==ELS_API_TYPE_NUMBER && arg_gettype(vm, 2)==ELS_API_TYPE_NUMBER)
    {
        long long num = arg_getnum(vm, 1);
        long long n = arg_getnum(vm, 2);
        long long result = num << n;
        arg_returnnum(vm, result);
    }
    else
        arg_returnnull(vm);
    return 1;
}
int ELSAPI_lsmath_move_right(els_VmObj* vm)
{
    /*
        # 参数
            1. num: int
            2. n: int
    */
    if (arg_gettype(vm, 1)==ELS_API_TYPE_NUMBER && arg_gettype(vm, 2)==ELS_API_TYPE_NUMBER)
    {
        long long num = arg_getnum(vm, 1);
        long long n = arg_getnum(vm, 2);
        long long result = num >> n;
        arg_returnnum(vm, result);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lsmath_bitwise_and(els_VmObj* vm)
{
    /*
        # 参数
            1. num: int
            2. n: int
    */
    if (arg_gettype(vm, 1)==ELS_API_TYPE_NUMBER && arg_gettype(vm, 2)==ELS_API_TYPE_NUMBER)
    {
        long long num = arg_getnum(vm, 1);
        long long n = arg_getnum(vm, 2);
        long long result = num & n;
        arg_returnnum(vm, result);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lsmath_bitwise_or(els_VmObj* vm)
{
    /*
        # 参数
            1. num: int
            2. n: int
    */
    if (arg_gettype(vm, 1)==ELS_API_TYPE_NUMBER && arg_gettype(vm, 2)==ELS_API_TYPE_NUMBER)
    {
        long long num = arg_getnum(vm, 1);
        long long n = arg_getnum(vm, 2);
        long long result = num | n;
        arg_returnnum(vm, result);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lsmath_bitwise_not(els_VmObj* vm)
{
    /*
        # 参数
            1. num: int
    */
    if (arg_gettype(vm, 1)==ELS_API_TYPE_NUMBER && arg_gettype(vm, 2)==ELS_API_TYPE_NUMBER)
    {
        long long num = arg_getnum(vm, 1);
        long long result = ~num;
        arg_returnnum(vm, result);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lsmath_bitwise_xor(els_VmObj* vm)
{
    /*
        # 参数
            1. num: int
            2. n: int
    */
    if (arg_gettype(vm, 1)==ELS_API_TYPE_NUMBER && arg_gettype(vm, 2)==ELS_API_TYPE_NUMBER)
    {
        long long num = arg_getnum(vm, 1);
        long long n = arg_getnum(vm, 2);
        long long result = num ^ n;
        arg_returnnum(vm, result);
    }
    else
        arg_returnnull(vm);
    return 1;
}

//     #数组运算
int ELSAPI_lsmath_sum(els_VmObj* vm)
{
    /*
        目前的实现是，从1开始遍历单元，直至索引到的值为空
        
        # 参数
            1. u unit
        
        #返回值
            1. u中从1开始到索引为0时的所有数字之和
    */
    if (arg_gettype(vm, 1)==ELS_API_TYPE_UNIT)
    {
        LosuObj *unit = arg_get(vm, 1);
        LosuObj key;
        LosuObj *val;
        double output = 0;
        unsigned int index = 1;

        for (;;)
        {
            key = obj_newnum(vm, index);
            val = obj_indexunit(vm, *unit, key);

            if (obj_type(vm, val)==ELS_API_TYPE_NULL)
                break;
            if (obj_type(vm, val)==ELS_API_TYPE_NUMBER)
                output += obj_tonum(vm, val);
            index++;
        }
        arg_returnnum(vm, output);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lsmath_even(els_VmObj* vm)
{
    if (arg_gettype(vm, 1)==ELS_API_TYPE_UNIT)
    {
        LosuObj *unit = arg_get(vm, 1);
        LosuObj key;
        LosuObj *val;
        double output = 0;
        unsigned int index = 0;

        for (;;)
        {
            key = obj_newnum(vm, index+1);
            val = obj_indexunit(vm, *unit, key);

            if (obj_type(vm, val)==ELS_API_TYPE_NULL)
                break;
            if (obj_type(vm, val)==ELS_API_TYPE_NUMBER)
                output += obj_tonum(vm, val);
            index++;
        }

        if (index==0)
            arg_returnnum(vm, 0);
        else
            arg_returnnum(vm, output / (double)(index));
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lsmath_degree(els_VmObj* vm)
{
    /*
        将弧度制转化为角度制

        # 参数
            1. x 角度制的数
        
        # 返回值
            1. 转化为弧度制后
    */
    if (arg_gettype(vm, 1) == ELS_API_TYPE_NUMBER)
    {
        double x = arg_getnum(vm, 1);
        arg_returnnum(vm, x * 180 / PI);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lsmath_radian(els_VmObj* vm)
{
    /*
        将角度制转化为弧度制

        # 参数
            1. x 弧度制的数
        
        # 返回值
            1. 转化为角度制后
    */
    if (arg_gettype(vm, 1) == ELS_API_TYPE_NUMBER)
    {
        double x = arg_getnum(vm, 1);
        arg_returnnum(vm, x * PI / 180);
    }
    else
        arg_returnnull(vm);
    return 1;
}