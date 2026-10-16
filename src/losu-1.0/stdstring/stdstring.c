#include "stdstring.h"
#include<string.h>
#define els_string_rt tmp
int ELSAPI_stdstring_length(els_VmObj* vm) // length(s: str ): int                        '返回 s 的长度'
{
    arg_returnnum(vm,strlen(arg_getstr(vm,1)));
    return 1;
}

int ELSAPI_stdstring_strstr(els_VmObj* vm) // strstr(fs: str,ss: str): str                '返回在 fs 中 ss 出现为首的子字符串'
{
    const char* tmp = strstr(arg_getstr(vm,1),arg_getstr(vm,2));
    if(tmp==NULL) arg_returnstr(vm,"");
    else arg_returnstr(vm,tmp);
    return 1;
}

int ELSAPI_stdstring_strtok(els_VmObj* vm) // strtok(fs: str,t: str): str                 '以 s 为标志,在 fs 中分词'
{
    char *tmp;
    if(arg_gettype(vm,1)==ELS_API_TYPE_NULL){
        tmp = strtok(NULL,arg_getstr(vm,2));
        if(tmp==NULL) arg_returnstr(vm,"");
        else arg_returnstr(vm,tmp);
    }else{ 
        tmp = strtok((char*)arg_getstr(vm,1),arg_getstr(vm,2));
        if(tmp==NULL) arg_returnstr(vm,"");
        else arg_returnstr(vm,tmp);
    }
    return 1;
}


int ELSAPI_stdstring_replace(els_VmObj* l) // replace(fs: str,ss: str, obj: str): str     '将 fs 中 的 ss 替换成 obj'
{
    char tmp[ELS_BUFF_TMP_SIZE];
    memset(tmp,'\0',ELS_BUFF_TMP_SIZE);
    const char* s1 = arg_getstr(l,1);
    const char* s2 = arg_getstr(l,2);
    const char* s3 = arg_getstr(l,3);
    int p =0;
    for(int i=0; i<strlen(s1); i++){
        if(s1[i]==s2[0]){
            // check equal
            for(int j = 0;j<strlen(s2);j++){
                if(s1[i+j]!=s2[j]) goto ueq;
            }

            for(int j=0;j<strlen(s3);j++){
                tmp[p++]=s3[j];
            }

        }else
            tmp[p++]=s1[i];
        
        ueq:;
    }
    arg_returnstr(l,tmp);
    return 1;
}

int ELSAPI_stdstring_mid(els_VmObj* l) // mid(s: str,sp: int ,len: int): str          '在 s 中 sp 位置 截取 len 长度的字符串'
{
    char tmp[ELS_BUFF_TMP_SIZE];
    memset(els_string_rt,'\0',ELS_BUFF_TMP_SIZE);
    const char* s1=arg_getstr(l,1);
    int sp=arg_getnum(l,2);
    int len=arg_getnum(l,3);
    for(int i=sp-1;i<sp+len-1;i++){
        els_string_rt[i-sp+1]=s1[i];
    }
    if(sp>strlen(s1)||sp<=0)   arg_returnstr(l,"");
    else    arg_returnstr(l,els_string_rt);
    return 1;
}
int ELSAPI_stdstring_reverse(els_VmObj* l) // reverse(s: str): str                        '倒置 s'
{
    char tmp[ELS_BUFF_TMP_SIZE];
    memset(els_string_rt,'\0',ELS_BUFF_TMP_SIZE);
    const char* s1=arg_getstr(l,1);
    int count=strlen(s1)-1;
    int i;
    for(i=0;i<strlen(s1);i++){
        if(s1[i]>=(char)0xe2&&s1[i]<=(char)0xef){
            els_string_rt[count-2]=s1[i];
            els_string_rt[count-1]=s1[i+1];
            els_string_rt[count]=s1[i+2];
            i=i+2;
            count-=3;
        }
        else if(s1[i]==(char)0xf0){
            els_string_rt[count-3] = s1[i];
            els_string_rt[count-2]=s1[i+1];
            els_string_rt[count-1]=s1[i+2];
            els_string_rt[count]=s1[i+3];
            i=i+3;
            count-=4;
        }
        else{
            els_string_rt[count]=s1[i];
            count--;
        }
    }
    els_string_rt[i]='\0';
    arg_returnstr(l,els_string_rt);
    return 1;
}

int ELSAPI_stdstring_toupper(els_VmObj* l) // toupper(s: str): str                        '大写 s'
{
    char tmp[ELS_BUFF_TMP_SIZE];
    memset(els_string_rt,'\0',ELS_BUFF_TMP_SIZE);
    const char* s1=arg_getstr(l,1);
    for(int i=0;i<strlen(s1);i++){
        if(s1[i]>='a'&&s1[i]<='z')  els_string_rt[i]=s1[i]-32;
        else    els_string_rt[i]=s1[i];
    }
    arg_returnstr(l,els_string_rt);
    return 1;
}
int ELSAPI_stdstring_tolower(els_VmObj* l) // tolower(s: str): str                        '小写 s'
{
    char tmp[ELS_BUFF_TMP_SIZE];
    memset(els_string_rt,'\0',ELS_BUFF_TMP_SIZE);
    const char* s1=arg_getstr(l,1);
    for(int i=0;i<strlen(s1);i++){
        if(s1[i]>='A'&&s1[i]<='Z')  els_string_rt[i]=s1[i]+32;
        else    els_string_rt[i]=s1[i];
    }
    arg_returnstr(l,els_string_rt);
    return 1;
}
int ELSAPI_stdstring_string(els_VmObj* l) // string(s: str,n: int): str                  '将 n 个 fs 拼接为一个长字符串'
{
    char tmp[ELS_BUFF_TMP_SIZE];
    memset(els_string_rt,'\0',ELS_BUFF_TMP_SIZE);
    const char* s1=arg_getstr(l,1);
    int len=arg_getnum(l,2);
    for(int i=0;i<len;i++){
        strcat(els_string_rt,s1);
    }
    arg_returnstr(l,els_string_rt);
    return 1;
}
int ELSAPI_stdstring_coutnum(els_VmObj* l) // coutnum(fs: str,ss: str): int               '统计 fs 中 ss 出现次数'
{
    const char* s1=arg_getstr(l,1);
    const char* s2=arg_getstr(l,2);
    int count=0,ans=0;
    for(int i=0;i<strlen(s1);i++){
        if(s1[i]==s2[0]){
            count=0;
            for(int j=i;j<i+strlen(s2);j++){
                if(s1[j]!=s2[j-i])  break;
                else    count++;
                if(count==strlen(s2))   ans++;
            }
        }
    }
    arg_returnnum(l,ans);
    return 1;
}
