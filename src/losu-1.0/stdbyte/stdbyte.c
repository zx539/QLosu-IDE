#include "stdbyte.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int ELSAPI_stdbyte_stdbyte_newbyte_str(els_VmObj* vm) // stdbyte_newbyte_str(o: object,b: str): byte         '生成一个新的二进制变量-从字符串'
{
    arg_returnbyte(vm,(char*)arg_getstr(vm,2),arg_get(vm,2)->value.ts->len);
    return 1;
}
int ELSAPI_stdbyte_stdbyte_newbyte_num(els_VmObj* vm) // stdbyte_newbyte_num(o: object,b: num): byte         '生成一个新的二进制变量-从数字'
{
    char tmp[sizeof(Number)]={0};
    Number i = arg_getnum(vm,2);
    char * _i = (char*)(&i);
    for(int j=0;j<sizeof(Number);j++)
        tmp[j]= (char)(*_i++);
    arg_returnbyte(vm,tmp,sizeof(Number));
    return 1;
}
int ELSAPI_stdbyte_stdbyte_newbyte_int(els_VmObj* vm) // stdbyte_newbyte_int(o: object,b: num): byte         '生成一个新的二进制变量-从整形'
{
    char tmp[sizeof(int)]={0};
    int i = (int)arg_getnum(vm,2);
    char * _i = (char*)(&i);
    for(int j=0;j<sizeof(int);j++)
        tmp[j]= (char)(*_i++);
    arg_returnbyte(vm,tmp,sizeof(int));
    return 1;
}
int ELSAPI_stdbyte_stdbyte_tostring(els_VmObj* vm) // stdbyte_tostring(o: object,b:byte): str             '转换为字符串'
{
    arg_returnstr(vm,arg_getbyte(vm,2));
    return 1;
}
int ELSAPI_stdbyte_stdbyte_tonumber(els_VmObj* vm) // stdbyte_tonumber(o: object,b:byte): number          '转换为数字'
{
    Number * i = (Number*)arg_getbyte(vm,2);
    arg_returnnum(vm,*i);
    return 1;
}
int ELSAPI_stdbyte_stdbyte_toint(els_VmObj* vm) // stdbyte_toint(o:object,b:byte): int                 '转换为整形'
{
    int * i = ((int*)(arg_getbyte(vm,2)));
    arg_returnnum(vm,*i);
    return 1;
}