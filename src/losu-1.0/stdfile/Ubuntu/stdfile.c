#include "stdfile.h"
#include<stdio.h>
#include<string.h>

int ELSAPI_stdfile_fopen(els_VmObj* vm) // fopen(fname: str,mod: str): ptr     '以指定模式打开一个文件,mod对应C语言,失败返回null'
{
    FILE* f = fopen(arg_getstr(vm,1),arg_getstr(vm,2));

    if(f) arg_returnptr(vm,(char*)(f));
    else arg_returnnull(vm);

    return 1;
}
int ELSAPI_stdfile_fclose(els_VmObj* vm) // fclose(fp: ptr): int                '关闭一个文件,成功返回0,失败返回null'
{
    int i = fclose((FILE*)(arg_getptr(vm,1)));

    if(i!=EOF) arg_returnnum(vm,i);
    else  arg_returnnull(vm);

    return 1;
}
int ELSAPI_stdfile_fputc(els_VmObj* vm) // fputc(fp: ptr,c: [int]): int        'c 的字符值写入到 fp 所指向的输出流中。如果写入成功，它会返回写入的字符，如果发生错误，则会返回 null'
{
    int i = fputc(arg_getnum(vm,2),(FILE*)(arg_getptr(vm,1)));

    if(i!=EOF) arg_returnnum(vm,i);
    else       arg_returnnull(vm);

    return 1;
}
int ELSAPI_stdfile_fputs(els_VmObj* vm) // fputs(fp: ptr,s: str): int          's 写入到 fp 所指向的输出流中。如果写入成功，它会返回一个非负值，如果发生错误，则会返回 null'
{
    int i = fputs(arg_getstr(vm,2),(FILE*)(arg_getptr(vm,1)));
    if(i!=EOF) arg_returnnum(vm,i);
    else       arg_returnnull(vm);

    return 1;
}

int ELSAPI_stdfile_fgetc(els_VmObj* vm) // fgetc(fp: ptr): int                 '函数从 fp 所指向的输入文件中读取一个字符。返回值是读取的字符，如果发生错误则返回 null' 
{
    arg_returnnum(vm,(Number)(fgetc((FILE*)(arg_getptr(vm,1)))));
    return 1;
}

int ELSAPI_stdfile_finput(els_VmObj* vm) // finput(fp:ptr): str                 '从 fp 所指向文件中读取一个字段，遇到空格等结束'
{
    char tmp[ELS_BUFF_TMP_SIZE];
    memset(tmp,'\0',ELS_BUFF_TMP_SIZE);
    if(feof((FILE*)(arg_getptr(vm,1)))) arg_returnnull(vm);
    else{
        fscanf((FILE*)(arg_getptr(vm,1)),"%s",tmp);
        arg_returnstr(vm,tmp);
    }
    return 1;
}

int ELSAPI_stdfile_fgets(els_VmObj* vm) // fgets(fp: ptr, int n): str          '从 fp 所指向的输入流中读取 n-1 个字符"如果n为null,则读取为内核最大字符缓冲区"如果这个函数在读取最后一个字符之前就遇到一个换行符或文件的末尾 null,则只会返回读取到的字符，包括换行符'
{
    char tmp[ELS_BUFF_TMP_SIZE];
    memset(tmp,'\0',ELS_BUFF_TMP_SIZE);
    
    if(feof((FILE*)(arg_getptr(vm,1)))) {
        arg_returnnull(vm);
        return 1;
    }
    if(arg_gettype(vm,2)==ELS_API_TYPE_NULL){
        fgets(tmp,ELS_BUFF_TMP_SIZE,(FILE*)(arg_getptr(vm,1)));
    }else{
        fgets(tmp,(int)arg_getnum(vm,2),(FILE*)(arg_getptr(vm,1)));
    }
    char a = tmp[strlen(tmp)-1];
    tmp[strlen(tmp)-1]=(a=='\n')?'\0':a;
    a = tmp[strlen(tmp)-1];
    tmp[strlen(tmp)-1]=(a=='\r')?'\0':a;
    arg_returnstr(vm,tmp);
    return 1;
}

int ELSAPI_stdfile_feof(els_VmObj* vm) // feof(fp: ptr): int                  'fp 是否到达结尾，否返回 null'
{
    int i = feof((FILE*)(arg_getptr(vm,1)));
    
    if(i) arg_returnnum(vm,i);
    else   arg_returnnull(vm);

    return 1;
}

int ELSAPI_stdfile_fseek(els_VmObj* vm) // fseek(fp: ptr,i: int,sp: int):int   '设置流 fp 的文件位置为给定的偏移 i,参数i 意味着从给定的 sp 位置查找的字节数'
{
    int i = fseek((FILE*)(arg_getptr(vm,1)),(int)(arg_getnum(vm,2)),(int)(arg_getnum(vm,3)));
    if(i) arg_returnnum(vm,i);
    else  arg_returnnull(vm);

    return 1;
}
int ELSAPI_stdfile_ftell(els_VmObj* vm) // ftell(fp: ptr): int                 '返回文件当前位置，以文件头为相对位置' 
{
    arg_returnnum(vm,(Number)(ftell((FILE*)(arg_getptr(vm,1)))));
    return 1;
}
int ELSAPI_stdfile_rewind(els_VmObj* vm) // rewind(fp: prt): null               '返回文件头' 
{
    rewind((FILE*)(arg_getptr(vm,1)));
    arg_returnnull(vm);
    return 1;
}


