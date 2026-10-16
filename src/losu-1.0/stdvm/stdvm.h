#include "els.h"
int ELSAPI_stdvm_stdvm_get_version(els_VmObj* vm); // stdvm_get_version(): str    '获取版本信息'
int ELSAPI_stdvm_stdvm_get_build(els_VmObj* vm); // stdvm_get_build():   str    '获取构建信息'
int ELSAPI_stdvm_stdvm_get_copy(els_VmObj* vm); // stdvm_get_copy():    str    '获取版权信息'
int ELSAPI_stdvm_stdvm_get_branch(els_VmObj* vm); // stdvm_get_branch():  str    '获取分支信息'
int ELSAPI_stdvm_stdvm_set_gc(els_VmObj* vm); // stdvm_set_gc(o:object,size: int,p:str): null   '设置GC上限,p可以为 B,K(默认),M'
int ELSAPI_stdvm_stdvm_get_mem(els_VmObj* vm); // stdvm_get_mem(o: obj,p:str): int        '获取内存占用大小,单位 B(默认),K,M'
int ELSAPI_stdvm_stdvm_get_gc(els_VmObj* vm); // stdvm_get_gc(o: obj,p:str): int         '获取GC上限，单位 B(默认),K,M' 
int ELSAPI_stdvm_stdvm_gc(els_VmObj* vm); // stdvm_gc()                  '立即进行GC'
#ifdef ELS_CONF_TOKEN_EN
static const char LibScript[]={
118,109,32,61,32,123,10,
32,32,32,32,118,101,114,115,105,111,110,32,61,32,115,116,100,118,109,95,103,101,116,95,118,101,114,115,105,111,110,44,10,
32,32,32,32,98,117,105,108,100,32,32,32,61,32,115,116,100,118,109,95,103,101,116,95,98,117,105,108,100,44,10,
32,32,32,32,99,111,112,121,32,32,32,32,61,32,115,116,100,118,109,95,103,101,116,95,99,111,112,121,44,10,
32,32,32,32,98,114,97,110,99,104,32,32,61,32,115,116,100,118,109,95,103,101,116,95,98,114,97,110,99,104,44,10,
32,32,32,32,115,101,116,103,99,32,32,32,61,32,115,116,100,118,109,95,115,101,116,95,103,99,44,10,
32,32,32,32,103,101,116,109,101,109,32,32,61,32,115,116,100,118,109,95,103,101,116,95,109,101,109,44,10,
32,32,32,32,103,101,116,103,99,32,32,32,61,32,115,116,100,118,109,95,103,101,116,95,103,99,44,10,
32,32,32,32,103,99,32,32,32,32,32,32,61,32,115,116,100,118,109,95,103,99,44,10,
125,10,
0};
#endif
#ifdef ELS_CONF_TOKEN_CN
static const char LibScript_cn[]={
-24,-89,-93,-23,-121,-118,-27,-103,-88,32,61,32,123,10,
32,32,32,32,-25,-119,-120,-26,-100,-84,32,61,32,115,116,100,118,109,95,103,101,116,95,118,101,114,115,105,111,110,44,10,
32,32,32,32,-26,-98,-124,-27,-69,-70,32,32,32,61,32,115,116,100,118,109,95,103,101,116,95,98,117,105,108,100,44,10,
32,32,32,32,-25,-119,-120,-26,-99,-125,32,32,32,32,61,32,115,116,100,118,109,95,103,101,116,95,99,111,112,121,44,10,
32,32,32,32,-27,-120,-122,-26,-108,-81,32,32,61,32,115,116,100,118,109,95,103,101,116,95,98,114,97,110,99,104,44,10,
32,32,32,32,-24,-82,-66,-27,-82,-102,71,67,32,32,32,61,32,115,116,100,118,109,95,115,101,116,95,103,99,44,10,
32,32,32,32,-24,-114,-73,-27,-113,-106,-27,-122,-123,-27,-83,-104,32,32,61,32,115,116,100,118,109,95,103,101,116,95,109,101,109,44,10,
32,32,32,32,-24,-114,-73,-27,-113,-106,71,67,32,32,32,61,32,115,116,100,118,109,95,103,101,116,95,103,99,44,10,
32,32,32,32,-27,-101,-98,-26,-108,-74,-27,-98,-125,-27,-100,-66,32,61,32,115,116,100,118,109,95,103,99,44,10,
125,10,
0};
#endif
void ElsLib_stdvm_libinit(els_VmObj *vm){
	vm_register(vm,"stdvm_get_version",ELSAPI_stdvm_stdvm_get_version);
	vm_register(vm,"stdvm_get_build",ELSAPI_stdvm_stdvm_get_build);
	vm_register(vm,"stdvm_gc",ELSAPI_stdvm_stdvm_gc);
	vm_register(vm,"stdvm_get_branch",ELSAPI_stdvm_stdvm_get_branch);
	vm_register(vm,"stdvm_get_gc",ELSAPI_stdvm_stdvm_get_gc);
	vm_register(vm,"stdvm_get_mem",ELSAPI_stdvm_stdvm_get_mem);
	vm_register(vm,"stdvm_get_copy",ELSAPI_stdvm_stdvm_get_copy);
	vm_register(vm,"stdvm_set_gc",ELSAPI_stdvm_stdvm_set_gc);
	#ifdef ELS_CONF_TOKEN_EN
		vm_dostring(vm,LibScript);
	#endif
	#ifdef ELS_CONF_TOKEN_CN
		vm_dostring(vm,LibScript_cn);
	#endif
};
