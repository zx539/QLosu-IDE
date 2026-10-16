
#include "std.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>


int ELSAPI_std_getversion(els_VmObj* vm) // getversion(): int                       '获取版本信息'
{
    arg_returnstr(vm,ELS_VERSION);
    return 1;
}
int ELSAPI_std_print(els_VmObj* vm)
{   // print( s:str , ... ) : null             '打印并换行'
    
    char* tmp;
    for(int i=0;i<arg_num(vm);i++){
        tmp = vm_win_togbk(arg_getstr(vm,i+1));
        printf("%s",tmp);
        free(tmp);
    }
        
    printf("\n");

    return 0;
}

int ELSAPI_std_printl(els_VmObj* vm)
{   // printl( s:str , ... ) : null            '打印但不换行'

    char* tmp;
    for(int i=0;i<arg_num(vm);i++){
        tmp = vm_win_togbk(arg_getstr(vm,i+1));
        printf("%s",tmp);
        free(tmp);
    }

    return 0;

}
int ELSAPI_std_input(els_VmObj* vm)
{   // input( i:[int/null] ) : str             '输入 i 组数据'
    
    char tmp[ELS_BUFF_TMP_SIZE];
    char* _t;
    scanf("%s",tmp);
    _t = vm_win_toutf8(tmp);
    arg_returnstr(vm,_t);
    free(_t);
    return 1;
}

int ELSAPI_std_lineinput(els_VmObj* vm)
{   // lineinput( i:[int/null] ) : str         '输入 i 行'
    
    char tmp[ELS_BUFF_TMP_SIZE];
    char* _t;
    gets(tmp);
    _t = vm_win_toutf8(tmp);
    arg_returnstr(vm,_t);
    free(_t);
    return 1;
}

int ELSAPI_std_sleep(els_VmObj* vm)
{   // sleep( ms:int ) : null                  '等待毫秒'

    Sleep((int)arg_getnum(vm,1));
    return 0;
}

int ELSAPI_std_eval(els_VmObj* vm)
{    // eval( v:str , mod:[int/null] ) : int    '解释字符串,mod不为空时解释文件' 

    char * tmp = vm_win_toutf8(arg_getstr(vm,1));
    if(arg_gettype(vm,2)!=ELS_API_TYPE_NULL){
        arg_returnnum(vm,(Number)(vm_dofile(vm,tmp)));
        free(tmp);
        return 1;
    }
    arg_returnnum(vm,(Number)(vm_dostring(vm,tmp)));
    free(tmp);
    return 1;
}

int ELSAPI_std_val(els_VmObj* vm)
{   // val( s:[num/str] ) : num                '转换为数字'

    arg_returnnum(vm,arg_getnum(vm,1));
    return 1;
}
int ELSAPI_std_int(els_VmObj* vm) 
{   // int( s:[num/str] ) : int                '转换为整数'

    arg_returnnum(vm,(int)arg_getnum(vm,1));
    return 1;
} 
int ELSAPI_std_str(els_VmObj* vm) // str( n:[num/str] ) : str                '转换为字符串'
{
    arg_returnstr(vm,arg_getstr(vm,1));
    return 1;
}
int ELSAPI_std_type(els_VmObj* vm) // type( o:any ) : str                     '返回参数的数据类型'
{
    arg_returnstr(vm,ELS_TYPESYSTEM[(int)arg_gettype(vm,1)]);
    return 1;
}

int ELSAPI_std_exit(els_VmObj* vm) // exit( x:int ) : null                    '退出并返回x'
{
    exit((int)arg_getnum(vm,1));
    return 1;
}
int ELSAPI_std_char(els_VmObj* l) // char(i:int): str                            '返回 i 对应的字符'
{
    char tmp[2];
    int num=arg_getnum(l,1);
    tmp[0]=(char)num;
    tmp[1]='\0';
    arg_returnstr(l,tmp);
    return 1;
}
int ELSAPI_std_asc(els_VmObj* l) // ascii(c:str): int  
{
    const char* s1=arg_getstr(l,1);
    arg_returnnum(l,(Number)((char)(s1[0])));
    return 1;
} 