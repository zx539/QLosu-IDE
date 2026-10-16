#include "els.h"
int ELSAPI_bytemaster_bytemaster_alloc(els_VmObj* vm); // bytemaster_alloc(length:int)
int ELSAPI_bytemaster_bytemaster_write(els_VmObj* vm); // bytemaster_write(m:byte, index:int, data:[int/byte/str])
int ELSAPI_bytemaster_bytemaster_read(els_VmObj* vm); // bytemaster_read(m:byte, index:int, size:int)
#ifdef ELS_CONF_TOKEN_EN
static const char LibScript[]={
10,
118,97,114,32,98,121,116,101,109,97,115,116,101,114,32,61,32,123,10,
32,32,32,32,97,108,108,111,99,32,61,32,98,121,116,101,109,97,115,116,101,114,95,97,108,108,111,99,44,10,
32,32,32,32,119,114,105,116,101,32,61,32,98,121,116,101,109,97,115,116,101,114,95,119,114,105,116,101,44,10,
32,32,32,32,114,101,97,100,32,61,32,98,121,116,101,109,97,115,116,101,114,95,114,101,97,100,10,
125,10,
10,
0};
#endif
void ElsLib_bytemaster_libinit(els_VmObj *vm){
	vm_register(vm,"bytemaster_read",ELSAPI_bytemaster_bytemaster_read);
	vm_register(vm,"bytemaster_write",ELSAPI_bytemaster_bytemaster_write);
	vm_register(vm,"bytemaster_alloc",ELSAPI_bytemaster_bytemaster_alloc);
	#ifdef ELS_CONF_TOKEN_EN
		vm_dostring(vm,LibScript);
	#endif
};
