#include "els.h"
int ELSAPI_bridge_bridge_c_cint8(els_VmObj* vm); // bridge_c_cint8()
int ELSAPI_bridge_bridge_c_cint16(els_VmObj* vm); // bridge_c_cint16()
int ELSAPI_bridge_bridge_c_cint32(els_VmObj* vm); // bridge_c_cint32()
int ELSAPI_bridge_bridge_c_cint64(els_VmObj* vm); // bridge_c_cint64()
int ELSAPI_bridge_bridge_c_cfloat32(els_VmObj* vm); // bridge_c_cfloat32()
int ELSAPI_bridge_bridge_c_cfloat64(els_VmObj* vm); // bridge_c_cfloat64()
int ELSAPI_bridge_bridge_c_cstr(els_VmObj* vm); // bridge_c_cstr()
int ELSAPI_bridge_bridge_c_iif(els_VmObj* vm); // bridge_c_iif(...) #int output int arg function
#ifdef ELS_CONF_TOKEN_EN
static const char LibScript[]={
10,
118,97,114,32,98,114,105,100,103,101,32,61,32,123,10,
10,
125,10,
10,
98,114,105,100,103,101,46,99,32,61,32,123,10,
32,32,32,32,105,110,116,32,61,32,98,114,105,100,103,101,95,99,95,99,105,110,116,51,50,44,10,
32,32,32,32,105,110,116,56,32,61,32,98,114,105,100,103,101,95,99,95,99,105,110,116,56,44,10,
32,32,32,32,105,110,116,49,54,32,61,32,98,114,105,100,103,101,95,99,95,99,105,110,116,49,54,44,10,
32,32,32,32,105,110,116,51,50,32,61,32,98,114,105,100,103,101,95,99,95,99,105,110,116,51,50,44,10,
32,32,32,32,105,110,116,54,52,32,61,32,98,114,105,100,103,101,95,99,95,99,105,110,116,54,52,44,10,
32,32,32,32,102,108,111,97,116,51,50,32,61,32,98,114,105,100,103,101,95,99,95,99,102,108,111,97,116,51,50,44,10,
32,32,32,32,102,108,111,97,116,54,52,32,61,32,98,114,105,100,103,101,95,99,95,99,102,108,111,97,116,54,52,44,10,
32,32,32,32,115,116,114,32,61,32,98,114,105,100,103,101,95,99,95,99,115,116,114,44,10,
32,32,32,32,105,105,102,32,61,32,98,114,105,100,103,101,95,99,95,105,105,102,44,10,
125,10,
10,
0};
#endif
void ElsLib_bridge_libinit(els_VmObj *vm){
	vm_register(vm,"bridge_c_cint16",ELSAPI_bridge_bridge_c_cint16);
	vm_register(vm,"bridge_c_cint8",ELSAPI_bridge_bridge_c_cint8);
	vm_register(vm,"bridge_c_cstr",ELSAPI_bridge_bridge_c_cstr);
	vm_register(vm,"bridge_c_cfloat32",ELSAPI_bridge_bridge_c_cfloat32);
	vm_register(vm,"bridge_c_cint32",ELSAPI_bridge_bridge_c_cint32);
	vm_register(vm,"bridge_c_cfloat64",ELSAPI_bridge_bridge_c_cfloat64);
	vm_register(vm,"bridge_c_iif",ELSAPI_bridge_bridge_c_iif);
	vm_register(vm,"bridge_c_cint64",ELSAPI_bridge_bridge_c_cint64);
	#ifdef ELS_CONF_TOKEN_EN
		vm_dostring(vm,LibScript);
	#endif
};
