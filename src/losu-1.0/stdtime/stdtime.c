#include "stdtime.h"
#include<time.h>
//#include "../buildenv/els.h"
int ELSAPI_stdtime_stdtime_getyear(els_VmObj* vm) // stdtime_getyear():int   '获取年'
{
    time_t t;
    time(&t);
    arg_returnnum(vm,(localtime(&t) -> tm_year) +1900);
    return 1;
}

int ELSAPI_stdtime_stdtime_getmon(els_VmObj* vm) // stdtime_getmon():int    '获取月'
{
    time_t t;
    time(&t);
    arg_returnnum(vm,localtime(&t) -> tm_mon+1);
    return 1;
}

int ELSAPI_stdtime_stdtime_getday(els_VmObj* vm) // stdtime_getday():int    '获取日'
{
    time_t t;
    time(&t);
    arg_returnnum(vm,localtime(&t) -> tm_mday);
    return 1;
}

int ELSAPI_stdtime_stdtime_gethour(els_VmObj* vm) // stdtime_gethour():int   '获取时'
{
    time_t t;
    time(&t);
    arg_returnnum(vm,localtime(&t) -> tm_hour);
    return 1;
}

int ELSAPI_stdtime_stdtime_getmin(els_VmObj* vm) // stdtime_getmin():int    '获取分'
{
    time_t t;
    time(&t);
    arg_returnnum(vm,localtime(&t) -> tm_min);
    return 1;
}

int ELSAPI_stdtime_stdtime_getsec(els_VmObj* vm) // stdtime_getsec():int    '获取秒'
{
    time_t t;
    time(&t);
    arg_returnnum(vm,localtime(&t) -> tm_sec);
    return 1;
}

int ELSAPI_stdtime_stdtime_getdate(els_VmObj* vm) // stdtime_getdate():int   '获取周几'
{
    time_t t;
    time(&t);
    arg_returnnum(vm,localtime(&t) -> tm_wday);
    return 1;
}

int ELSAPI_stdtime_stdtime_clock(els_VmObj* vm) // stdtime_clock():int     '获取计时器程序从运行开始的毫秒计时'
{
    arg_returnnum(vm,(Number)(clock())/CLOCKS_PER_SEC);
    return 1;
}
