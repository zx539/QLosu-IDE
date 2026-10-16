#include "els.h"
int ELSAPI_re_re_match(els_VmObj* vm); // re_match()
int ELSAPI_re_re_search(els_VmObj* vm); // re_search()
int ELSAPI_re_re_replace(els_VmObj* vm); // re_replace()
#ifdef ELS_CONF_TOKEN_EN
static const char LibScript[]={
10,
118,97,114,32,114,101,32,61,32,123,10,
32,32,32,32,109,97,116,99,104,32,61,32,114,101,95,109,97,116,99,104,44,10,
32,32,32,32,115,101,97,114,99,104,32,61,32,114,101,95,115,101,97,114,99,104,44,10,
32,32,32,32,114,101,112,108,97,99,101,32,61,32,114,101,95,114,101,112,108,97,99,101,10,
125,10,
10,
0};
#endif
void ElsLib_re_libinit(els_VmObj *vm){
	vm_register(vm,"re_match",ELSAPI_re_re_match);
	vm_register(vm,"re_search",ELSAPI_re_re_search);
	vm_register(vm,"re_replace",ELSAPI_re_re_replace);
	#ifdef ELS_CONF_TOKEN_EN
		vm_dostring(vm,LibScript);
	#endif
};
