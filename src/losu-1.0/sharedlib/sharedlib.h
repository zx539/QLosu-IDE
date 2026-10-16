#include "els.h"
int ELSAPI_sharedlib_sharedlib_load(els_VmObj* vm); // sharedlib_load()
int ELSAPI_sharedlib_sharedlib_unload(els_VmObj* vm); // sharedlib_unload()
int ELSAPI_sharedlib_sharedlib_sys(els_VmObj* vm); // sharedlib_sys()
#ifdef ELS_CONF_TOKEN_EN
static const char LibScript[]={
10,
118,97,114,32,115,104,97,114,101,100,108,105,98,32,61,32,123,10,
32,32,32,32,108,111,97,100,32,61,32,115,104,97,114,101,100,108,105,98,95,108,111,97,100,44,10,
32,32,32,32,117,110,108,111,97,100,32,61,32,115,104,97,114,101,100,108,105,98,95,117,110,108,111,97,100,44,10,
32,32,32,32,115,121,115,32,61,32,115,104,97,114,101,100,108,105,98,95,115,121,115,10,
125,10,
10,
0};
#endif
void ElsLib_sharedlib_libinit(els_VmObj *vm){
	vm_register(vm,"sharedlib_unload",ELSAPI_sharedlib_sharedlib_unload);
	vm_register(vm,"sharedlib_load",ELSAPI_sharedlib_sharedlib_load);
	vm_register(vm,"sharedlib_sys",ELSAPI_sharedlib_sharedlib_sys);
	#ifdef ELS_CONF_TOKEN_EN
		vm_dostring(vm,LibScript);
	#endif
};
