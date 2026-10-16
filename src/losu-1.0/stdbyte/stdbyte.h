#include "els.h"
int ELSAPI_stdbyte_stdbyte_newbyte_str(els_VmObj* vm); // stdbyte_newbyte_str(o: object,b: str): byte         '生成一个新的二进制变量-从字符串'
int ELSAPI_stdbyte_stdbyte_newbyte_num(els_VmObj* vm); // stdbyte_newbyte_num(o: object,b: num): byte         '生成一个新的二进制变量-从数字'
int ELSAPI_stdbyte_stdbyte_newbyte_int(els_VmObj* vm); // stdbyte_newbyte_int(o: object,b: num): byte         '生成一个新的二进制变量-从整形'
int ELSAPI_stdbyte_stdbyte_tostring(els_VmObj* vm); // stdbyte_tostring(o: object,b:byte): str             '转换为字符串'
int ELSAPI_stdbyte_stdbyte_tonumber(els_VmObj* vm); // stdbyte_tonumber(o: object,b:byte): number          '转换为数字'
int ELSAPI_stdbyte_stdbyte_toint(els_VmObj* vm); // stdbyte_toint(o:object,b:byte): int                 '转换为整形'
#ifdef ELS_CONF_TOKEN_EN
static const char LibScript[]={
98,121,116,101,32,61,32,123,10,
32,32,32,32,98,121,115,116,114,32,32,32,61,32,115,116,100,98,121,116,101,95,110,101,119,98,121,116,101,95,115,116,114,44,10,
32,32,32,32,98,121,110,117,109,32,32,32,61,32,115,116,100,98,121,116,101,95,110,101,119,98,121,116,101,95,110,117,109,44,10,
32,32,32,32,98,121,105,110,116,32,32,32,61,32,115,116,100,98,121,116,101,95,110,101,119,98,121,116,101,95,105,110,116,44,10,
32,32,32,32,116,111,115,116,114,32,32,32,61,32,115,116,100,98,121,116,101,95,116,111,115,116,114,105,110,103,44,10,
32,32,32,32,116,111,110,117,109,32,32,32,61,32,115,116,100,98,121,116,101,95,116,111,110,117,109,98,101,114,44,10,
32,32,32,32,116,111,105,110,116,32,32,32,61,32,115,116,100,98,121,116,101,95,116,111,105,110,116,44,10,
125,10,
0};
#endif
#ifdef ELS_CONF_TOKEN_CN
static const char LibScript_cn[]={
-28,-70,-116,-24,-65,-101,-27,-120,-74,32,61,32,123,10,
32,32,32,32,-27,-120,-101,-27,-69,-70,-24,-121,-86,32,61,32,123,10,
32,32,32,32,32,32,32,32,-27,-83,-105,-25,-84,-90,-28,-72,-78,32,61,32,115,116,100,98,121,116,101,95,110,101,119,98,121,116,101,95,115,116,114,44,10,
32,32,32,32,32,32,32,32,-26,-107,-80,-27,-83,-105,32,32,32,61,32,115,116,100,98,121,116,101,95,110,101,119,98,121,116,101,95,110,117,109,44,10,
32,32,32,32,32,32,32,32,-26,-107,-76,-27,-98,-117,32,32,32,61,32,115,116,100,98,121,116,101,95,110,101,119,98,121,116,101,95,105,110,116,44,10,
32,32,32,32,125,44,10,
32,32,32,32,-24,-67,-84,-26,-115,-94,-27,-120,-80,32,61,32,123,10,
32,32,32,32,32,32,32,32,-27,-83,-105,-25,-84,-90,-28,-72,-78,32,32,32,61,32,115,116,100,98,121,116,101,95,116,111,115,116,114,105,110,103,44,10,
32,32,32,32,32,32,32,32,-26,-107,-80,-27,-83,-105,32,32,32,32,32,61,32,115,116,100,98,121,116,101,95,116,111,110,117,109,98,101,114,44,10,
32,32,32,32,32,32,32,32,-26,-107,-76,-27,-98,-117,32,32,32,61,32,115,116,100,98,121,116,101,95,116,111,105,110,116,44,10,
32,32,32,32,125,10,
32,32,32,32,10,
125,10,
0};
#endif
void ElsLib_stdbyte_libinit(els_VmObj *vm){
	vm_register(vm,"stdbyte_toint",ELSAPI_stdbyte_stdbyte_toint);
	vm_register(vm,"stdbyte_newbyte_str",ELSAPI_stdbyte_stdbyte_newbyte_str);
	vm_register(vm,"stdbyte_newbyte_int",ELSAPI_stdbyte_stdbyte_newbyte_int);
	vm_register(vm,"stdbyte_tostring",ELSAPI_stdbyte_stdbyte_tostring);
	vm_register(vm,"stdbyte_newbyte_num",ELSAPI_stdbyte_stdbyte_newbyte_num);
	vm_register(vm,"stdbyte_tonumber",ELSAPI_stdbyte_stdbyte_tonumber);
	#ifdef ELS_CONF_TOKEN_EN
		vm_dostring(vm,LibScript);
	#endif
	#ifdef ELS_CONF_TOKEN_CN
		vm_dostring(vm,LibScript_cn);
	#endif
};
