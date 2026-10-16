#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    "els.h"

int     els_getargv (els_VmObj *l);
int     els_getargc (els_VmObj *l);
void    losu_repl(els_VmObj*l);
void losu_run(els_VmObj*l,const char* libfile,int argc,char** argv);

#ifdef LOSU_WINDOWS
#include<windows.h>
extern char* vm_win_togbk(const char * str);
extern char* vm_win_toutf8(const char* str);
#endif

#ifdef LOSU_LINUX
#include<unistd.h>
#include<dlfcn.h>
#endif


#ifdef LOSU_LINUX

void losu_run(els_VmObj*l,const char* libfile,int argc,char** argv){
    char tmp[512];
    typedef int (*api)(int c,char** v);
    sprintf(tmp,"./ElsLib_%s.lsd",libfile);
    
    void* dlibptr = dlopen(tmp,RTLD_LAZY);
    if(dlibptr==NULL){ 
        sprintf(tmp,"/els/lib/ElsLib_%s.lsd",libfile);
        dlibptr = dlopen(tmp,RTLD_LAZY);
    }
    if(dlibptr==NULL) {
        sprintf(tmp,"LibError: Couldn't find Library\t%s\n",libfile);
        printf("%s",tmp);
        exit(1);
    }
    
    api func = (api)dlsym(dlibptr,"main");
    if(func==NULL){ 
        sprintf(tmp,"LibError: Couldn't find Main in Library\t%s\n",libfile);
        printf("%s",tmp);
        exit(1);
    }
    func(argc,argv);
}
void    losu_repl(els_VmObj*l){
        char tmp[1024];
        while(1){
            printf(">>>>");
            fgets(tmp,1024,stdin);
            vm_dostring(l,tmp);
        }
}

#endif

#ifdef LOSU_WINDOWS

void    losu_repl(els_VmObj*l){
        char tmp[1024];
        char *tmp2;
        while(1){
            printf(">>>>");
            fgets(tmp,1024,stdin);
            tmp2 = vm_win_toutf8(tmp);
            vm_dostring(l,tmp2);
            free(tmp2);
        }
}
void losu_run(els_VmObj*l,const char* libfile,int argc,char** argv){
    char tmp[512];
    typedef int (*api)(int c,char** v);
    sprintf(tmp,"./lib/ElsLib_%s.lsd",libfile);
    HMODULE dlibptr =  LoadLibrary(tmp);
    if(dlibptr == NULL){
        sprintf(tmp,"./ElsLib_%s.lsd",libfile);
        dlibptr =  LoadLibrary(tmp);    
    }
    api func = (api)GetProcAddress(dlibptr,"main");
    if(func==NULL){ 
        sprintf(tmp,"LibError: Couldn't find Main in Library\t%s\n",libfile);
        printf("%s",tmp);
        exit(1);
    }
    func(argc,argv);
}

#endif



int main(int argc,char** argv)
{
    int s=0;
    els_VmObj *l=vm_create(1024);
    vm_setGC(l,1024*1024*2);      // 2 M 内存限制，超出时会将 GC提升至满功率，引起一定的性能下降
    els_argc = argc;
    els_argv = argv;
    #ifdef ELS_CONF_TOKEN_EN
        vm_register(l,"argv", els_getargv);
        vm_register(l,"argc", els_getargc);
    #endif
    #ifdef ELS_CONF_TOKEN_CN
        vm_register(l,"命令行参数", els_getargv);
        vm_register(l,"参数个数", els_getargc);
    #endif


    els_lib_init(l);
    vm_dostring(l,"import('stdlib')\n");
    if (argc < 2){
        printf("EasyLosu Script  %s\t%s\n%s\n%s\n",ELS_VERSION,ELS_COPYRIGHT,ELS_BUILD,ELS_BRANCH);
        printf("----------------------------------------------------------------\n");
        
        losu_repl(l);
        return 0;
    }else{
        if(!strcmp(argv[1],"-r")){
            char tmp[256];
            // if(argc>=3) 
            //     losu_run(l,argv[2],argc-2,&argv[2]);
            if(argc>=3) {
                els_argc = argc-1;
                els_argv = (char**)(argv)+1;
                sprintf(tmp,"import(\"%s\")",argv[2]);
                s = vm_dostring(l,tmp);
            }
        }else if(!strcmp(argv[1],"-v")){
            printf("EasyLosu Script  %s\t%s\n%s\n%s\n",ELS_VERSION,ELS_COPYRIGHT,ELS_BUILD,ELS_BRANCH);
            s = 0;
        }else if(!strcmp(argv[1],"-V")){
            printf("%s\n",ELS_VERSION);
            s = 0;   
        }else{
            s =  vm_dofile (l,argv[1]);
        }
    }
    vm_close(l);
    return s;
}
