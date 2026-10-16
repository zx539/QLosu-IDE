#include "stdmath.h"
#include<math.h>

#define PI        3.141592653589793
#define E         2.718281828459045
#define GAMMA     0.577215664901532

int ELSAPI_stdmath_PI(els_VmObj* vm){
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
int ELSAPI_stdmath_E(els_VmObj* vm){
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
int ELSAPI_stdmath_GAMMA(els_VmObj* vm){
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
int ELSAPI_stdmath_abs(els_VmObj *vm){
    arg_returnnum(vm,fabs(arg_getnum(vm,1)));
    return 1;
}
int ELSAPI_stdmath_mod(els_VmObj *vm){
    arg_returnnum(vm,((long int)arg_getnum(vm,1)%(long int)arg_getnum(vm,2)));
    return 1;
}
int ELSAPI_stdmath_acos(els_VmObj *vm){
    arg_returnnum(vm,acos(arg_getnum(vm,1)));
    return 1;
}
int ELSAPI_stdmath_asin(els_VmObj *vm){
    arg_returnnum(vm,asin(arg_getnum(vm,1)));
    return 1;
}
int ELSAPI_stdmath_atan(els_VmObj *vm){
    arg_returnnum(vm,atan(arg_getnum(vm,1)));
    return 1;
}
int ELSAPI_stdmath_cos(els_VmObj *vm){
    arg_returnnum(vm,cos(arg_getnum(vm,1)));
    return 1;
}
int ELSAPI_stdmath_sin(els_VmObj *vm){
    arg_returnnum(vm,sin(arg_getnum(vm,1)));
    return 1;
}
int ELSAPI_stdmath_tan(els_VmObj *vm){
    arg_returnnum(vm,tan(arg_getnum(vm,1)));
    return 1;
}
int ELSAPI_stdmath_exp(els_VmObj *vm){
    arg_returnnum(vm,exp(arg_getnum(vm,1)));
    return 1;
}
int ELSAPI_stdmath_log(els_VmObj *vm){
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
int ELSAPI_stdmath_ln(els_VmObj *vm){
    arg_returnnum(vm,log(arg_getnum(vm,1)));
    return 1;
}
int ELSAPI_stdmath_lg(els_VmObj *vm){
    arg_returnnum(vm,log10(arg_getnum(vm,1)));
    return 1;
}
int ELSAPI_stdmath_sqrt(els_VmObj *vm){
    arg_returnnum(vm,sqrt(arg_getnum(vm,1)));
    return 1;
}
int ELSAPI_stdmath_ceil(els_VmObj *vm){
    arg_returnnum(vm,ceil(arg_getnum(vm,1)));
    return 1;
}
int ELSAPI_stdmath_floor(els_VmObj *vm){
    arg_returnnum(vm,floor(arg_getnum(vm,1)));
    return 1;
}
int ELSAPI_stdmath_round(els_VmObj *vm){
    /*
        调用math.h中的double round(double)函数，用于四舍五入
        从洛书处接收要四舍五入的数
    */
    arg_returnnum(vm, (double)round(arg_getnum(vm, 1)));
    return 1;
}
int ELSAPI_stdmath_cut(els_VmObj *vm){
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