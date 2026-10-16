#include "els.h"
int ELSAPI_stdstring_length(els_VmObj* vm); // length(s: str ): int                        '返回 s 的长度'
int ELSAPI_stdstring_strstr(els_VmObj* vm); // strstr(fs: str,ss: str): str                '返回在 fs 中 ss 出现为首的子字符串'
int ELSAPI_stdstring_strtok(els_VmObj* vm); // strtok(fs: str,t: str): str                 '以 s 为标志,在 fs 中分词'
int ELSAPI_stdstring_replace(els_VmObj* vm); // replace(fs: str,ss: str, obj: str): str     '将 fs 中 的 ss 替换成 obj'
int ELSAPI_stdstring_mid(els_VmObj* vm); // mid(s: str,sp: int ,len: int): str          '在 s 中 sp 位置 截取 len 长度的字符串'
int ELSAPI_stdstring_reverse(els_VmObj* vm); // reverse(s: str): str                        '倒置 s'
int ELSAPI_stdstring_toupper(els_VmObj* vm); // toupper(s: str): str                        '大写 s'
int ELSAPI_stdstring_tolower(els_VmObj* vm); // tolower(s: str): str                        '小写 s'
int ELSAPI_stdstring_string(els_VmObj* vm); // string(s: str,n: int): str                  '将 n 个 fs 拼接为一个长字符串'
int ELSAPI_stdstring_coutnum(els_VmObj* vm); // coutnum(fs: str,ss: str): int               '统计 fs 中 ss 出现次数'
#ifdef ELS_CONF_TOKEN_CN
static const char LibScript_cn[]={
-23,-107,-65,-27,-70,-90,32,61,32,108,101,110,103,116,104,32,10,
-27,-81,-69,-24,-121,-77,32,61,32,115,116,114,115,116,114,10,
-27,-120,-122,-24,-81,-115,32,61,32,115,116,114,116,111,107,10,
-26,-101,-65,-26,-115,-94,32,61,32,114,101,112,108,97,99,101,32,10,
-26,-120,-86,-27,-113,-106,32,61,32,109,105,100,32,10,
-27,-128,-110,-25,-67,-82,32,61,32,114,101,118,101,114,115,101,32,10,
-27,-92,-89,-27,-122,-103,32,61,32,116,111,117,112,112,101,114,32,10,
-27,-80,-113,-27,-122,-103,32,61,32,116,111,108,111,119,101,114,32,10,
-26,-118,-124,-27,-122,-103,32,61,32,115,116,114,105,110,103,10,
-25,-69,-97,-24,-82,-95,32,61,32,99,111,117,116,110,117,109,10,
0};
#endif
void ElsLib_stdstring_libinit(els_VmObj *vm){
	vm_register(vm,"reverse",ELSAPI_stdstring_reverse);
	vm_register(vm,"tolower",ELSAPI_stdstring_tolower);
	vm_register(vm,"length",ELSAPI_stdstring_length);
	vm_register(vm,"strtok",ELSAPI_stdstring_strtok);
	vm_register(vm,"toupper",ELSAPI_stdstring_toupper);
	vm_register(vm,"mid",ELSAPI_stdstring_mid);
	vm_register(vm,"replace",ELSAPI_stdstring_replace);
	vm_register(vm,"coutnum",ELSAPI_stdstring_coutnum);
	vm_register(vm,"string",ELSAPI_stdstring_string);
	vm_register(vm,"strstr",ELSAPI_stdstring_strstr);
	#ifdef ELS_CONF_TOKEN_CN
		vm_dostring(vm,LibScript_cn);
	#endif
};
