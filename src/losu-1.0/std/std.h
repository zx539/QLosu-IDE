#include "els.h"
int ELSAPI_std_print(els_VmObj* vm); // print( s:str , ... ) : null             '打印并换行'
int ELSAPI_std_printl(els_VmObj* vm); // printl( s:str , ... ) : null            '打印但不换行'
int ELSAPI_std_input(els_VmObj* vm); // input( i:[int/null] ) : str             '输入 i 组数据'
int ELSAPI_std_lineinput(els_VmObj* vm); // lineinput( i:[int/null] ) : str         '输入 i 行'
int ELSAPI_std_sleep(els_VmObj* vm); // sleep( ms:int ) : null                  '等待毫秒'
int ELSAPI_std_eval(els_VmObj* vm); // eval( v:str , mod:[int/null] ) : int    '解释字符串,mod不为空时解释文件' 
int ELSAPI_std_val(els_VmObj* vm); // val( s:[num/str] ) : num                '转换为数字'
int ELSAPI_std_int(els_VmObj* vm); // int( s:[num/str] ) : int                '转换为整数'
int ELSAPI_std_str(els_VmObj* vm); // str( n:[num/str] ) : str                '转换为字符串'
int ELSAPI_std_type(els_VmObj* vm); // type( o:any ) : str                     '返回参数的数据类型'
int ELSAPI_std_exit(els_VmObj* vm); // exit( x:int ) : null                    '退出并返回x'
int ELSAPI_std_char(els_VmObj* vm); // char(i:int): str                        '返回 i 对应的字符'
int ELSAPI_std_asc(els_VmObj* vm); // asc(c:str): int                         '返回 c 对应的 ASCII 码'
int ELSAPI_std_getversion(els_VmObj* vm); // getversion(): str                       '获取版本信息'
#ifdef ELS_CONF_TOKEN_CN
static const char LibScript_cn[]={
-26,-119,-109,-27,-115,-80,32,61,32,112,114,105,110,116,10,
-26,-119,-109,-27,-115,-80,-28,-72,-128,-24,-95,-116,32,61,32,112,114,105,110,116,108,10,
-24,-66,-109,-27,-123,-91,32,61,32,32,105,110,112,117,116,10,
-24,-66,-109,-27,-123,-91,-28,-72,-128,-24,-95,-116,32,61,32,32,108,105,110,101,105,110,112,117,116,10,
-25,-83,-119,-27,-66,-123,32,61,32,115,108,101,101,112,10,
-23,-121,-118,32,61,32,32,101,118,97,108,32,10,
-26,-107,-80,-27,-128,-68,-27,-116,-106,32,61,32,118,97,108,32,10,
-26,-107,-76,-26,-107,-80,-27,-116,-106,32,61,32,32,105,110,116,32,10,
-27,-83,-105,-25,-84,-90,-27,-116,-106,32,61,32,32,115,116,114,32,10,
-25,-79,-69,-27,-98,-117,32,61,32,32,116,121,112,101,32,10,
-23,-128,-128,32,61,32,32,101,120,105,116,10,
-27,-83,-105,-24,-118,-126,32,61,32,99,104,97,114,32,10,
-27,-83,-105,-25,-96,-127,32,61,32,97,115,99,10,
-24,-114,-73,-27,-113,-106,-25,-119,-120,-26,-100,-84,32,61,32,103,101,116,118,101,114,115,105,111,110,10,
0};
#endif
void ElsLib_std_libinit(els_VmObj *vm){
	vm_register(vm,"type",ELSAPI_std_type);
	vm_register(vm,"getversion",ELSAPI_std_getversion);
	vm_register(vm,"int",ELSAPI_std_int);
	vm_register(vm,"asc",ELSAPI_std_asc);
	vm_register(vm,"char",ELSAPI_std_char);
	vm_register(vm,"val",ELSAPI_std_val);
	vm_register(vm,"str",ELSAPI_std_str);
	vm_register(vm,"lineinput",ELSAPI_std_lineinput);
	vm_register(vm,"printl",ELSAPI_std_printl);
	vm_register(vm,"print",ELSAPI_std_print);
	vm_register(vm,"exit",ELSAPI_std_exit);
	vm_register(vm,"input",ELSAPI_std_input);
	vm_register(vm,"eval",ELSAPI_std_eval);
	vm_register(vm,"sleep",ELSAPI_std_sleep);
	#ifdef ELS_CONF_TOKEN_CN
		vm_dostring(vm,LibScript_cn);
	#endif
};
