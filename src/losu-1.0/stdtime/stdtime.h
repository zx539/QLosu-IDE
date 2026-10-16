#include "els.h"
int ELSAPI_stdtime_stdtime_getyear(els_VmObj* vm); // stdtime_getyear():int   '获取年'
int ELSAPI_stdtime_stdtime_getmon(els_VmObj* vm); // stdtime_getmon():int    '获取月'
int ELSAPI_stdtime_stdtime_getday(els_VmObj* vm); // stdtime_getday():int    '获取日'
int ELSAPI_stdtime_stdtime_gethour(els_VmObj* vm); // stdtime_gethour():int   '获取时'
int ELSAPI_stdtime_stdtime_getmin(els_VmObj* vm); // stdtime_getmin():int    '获取分'
int ELSAPI_stdtime_stdtime_getsec(els_VmObj* vm); // stdtime_getsec():int    '获取秒'
int ELSAPI_stdtime_stdtime_getdate(els_VmObj* vm); // stdtime_getdate():int   '获取周几'
int ELSAPI_stdtime_stdtime_clock(els_VmObj* vm); // stdtime_clock():int     '获取计时器程序从运行开始的毫秒计时'
#ifdef ELS_CONF_TOKEN_EN
static const char LibScript[]={
10,
35,32,116,105,109,101,32,-25,-69,-109,-26,-98,-124,-25,-79,-69,10,
116,105,109,101,32,61,32,123,10,
32,32,32,32,121,101,97,114,32,61,32,115,116,100,116,105,109,101,95,103,101,116,121,101,97,114,44,10,
32,32,32,32,109,111,110,32,32,61,32,115,116,100,116,105,109,101,95,103,101,116,109,111,110,44,10,
32,32,32,32,100,97,121,32,32,61,32,115,116,100,116,105,109,101,95,103,101,116,100,97,121,44,10,
32,32,32,32,104,111,117,114,32,61,32,115,116,100,116,105,109,101,95,103,101,116,104,111,117,114,44,10,
32,32,32,32,109,105,110,32,32,61,32,115,116,100,116,105,109,101,95,103,101,116,109,105,110,44,10,
32,32,32,32,115,101,99,32,32,61,32,115,116,100,116,105,109,101,95,103,101,116,115,101,99,44,10,
32,32,32,32,100,97,116,101,32,61,32,115,116,100,116,105,109,101,95,103,101,116,100,97,116,101,44,10,
32,32,32,32,99,108,111,99,107,32,32,32,61,32,115,116,100,116,105,109,101,95,99,108,111,99,107,44,10,
125,10,
0};
#endif
#ifdef ELS_CONF_TOKEN_CN
static const char LibScript_cn[]={
-26,-105,-74,-23,-105,-76,32,61,32,123,10,
32,32,32,32,-27,-71,-76,32,32,32,61,32,115,116,100,116,105,109,101,95,103,101,116,121,101,97,114,44,10,
32,32,32,32,-26,-100,-120,32,32,32,61,32,115,116,100,116,105,109,101,95,103,101,116,109,111,110,44,10,
32,32,32,32,-26,-105,-91,32,32,32,61,32,115,116,100,116,105,109,101,95,103,101,116,100,97,121,44,10,
32,32,32,32,-26,-105,-74,32,32,32,61,32,115,116,100,116,105,109,101,95,103,101,116,104,111,117,114,44,10,
32,32,32,32,-27,-120,-122,32,32,32,61,32,115,116,100,116,105,109,101,95,103,101,116,109,105,110,44,10,
32,32,32,32,-25,-89,-110,32,32,32,61,32,115,116,100,116,105,109,101,95,103,101,116,115,101,99,44,10,
32,32,32,32,-27,-111,-88,-27,-121,-96,32,61,32,115,116,100,116,105,109,101,95,103,101,116,100,97,116,101,44,10,
32,32,32,32,-24,-82,-95,-26,-105,-74,32,61,32,115,116,100,116,105,109,101,95,99,108,111,99,107,44,10,
125,10,
0};
#endif
void ElsLib_stdtime_libinit(els_VmObj *vm){
	vm_register(vm,"stdtime_getyear",ELSAPI_stdtime_stdtime_getyear);
	vm_register(vm,"stdtime_clock",ELSAPI_stdtime_stdtime_clock);
	vm_register(vm,"stdtime_getday",ELSAPI_stdtime_stdtime_getday);
	vm_register(vm,"stdtime_getdate",ELSAPI_stdtime_stdtime_getdate);
	vm_register(vm,"stdtime_gethour",ELSAPI_stdtime_stdtime_gethour);
	vm_register(vm,"stdtime_getmin",ELSAPI_stdtime_stdtime_getmin);
	vm_register(vm,"stdtime_getsec",ELSAPI_stdtime_stdtime_getsec);
	vm_register(vm,"stdtime_getmon",ELSAPI_stdtime_stdtime_getmon);
	#ifdef ELS_CONF_TOKEN_EN
		vm_dostring(vm,LibScript);
	#endif
	#ifdef ELS_CONF_TOKEN_CN
		vm_dostring(vm,LibScript_cn);
	#endif
};
