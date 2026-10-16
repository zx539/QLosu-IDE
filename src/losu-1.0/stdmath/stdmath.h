#include "els.h"
// # 常量定义
int ELSAPI_stdmath_PI(els_VmObj* vm); // PI():                   float             #返回π的值
int ELSAPI_stdmath_E(els_VmObj* vm); // E():                    float             #返回e的值 
int ELSAPI_stdmath_GAMMA(els_VmObj* vm); // GAMMA():                float             #返回γ（欧拉常数）的值
// # 标准数学函数
int ELSAPI_stdmath_abs(els_VmObj* vm); // abs( num )                                #返回数的绝对值
int ELSAPI_stdmath_mod(els_VmObj* vm); // mod( a, b )                               #返回a mod b的结果
int ELSAPI_stdmath_acos(els_VmObj* vm); // acos( x )                                 #返回以弧度表示的x的反余弦
int ELSAPI_stdmath_asin(els_VmObj* vm); // asin( x )                                 #返回以弧度表示的x的反正弦
int ELSAPI_stdmath_atan(els_VmObj* vm); // atan( x )                                 #返回以弧度表示的x的反正切
int ELSAPI_stdmath_cos(els_VmObj* vm); // cos( x )                                  #返回x的余弦值
int ELSAPI_stdmath_sin(els_VmObj* vm); // sin( x )                                  #返回x的正弦值
int ELSAPI_stdmath_tan(els_VmObj* vm); // tan( x )                                  #返回x的正切值
int ELSAPI_stdmath_exp(els_VmObj* vm); // exp( x )                                  #返回e的x次幂的值
int ELSAPI_stdmath_log(els_VmObj* vm); // log( a, N )                               #返回以a为第N的对数
int ELSAPI_stdmath_ln(els_VmObj* vm); // ln( N )                                   #返回以e为底，N的对数
int ELSAPI_stdmath_lg(els_VmObj* vm); // lg( N )                                   #返回以10为底，N的对数
int ELSAPI_stdmath_sqrt(els_VmObj* vm); // sqrt( num:number ):     float             #返回num的平方根
int ELSAPI_stdmath_ceil(els_VmObj* vm); // ceil( num:float ):      float             #对num向上取整
int ELSAPI_stdmath_floor(els_VmObj* vm); // floor( num:float ):     float             #对num向下取整
int ELSAPI_stdmath_round(els_VmObj* vm); // round( num:float ):     float             #将小数四舍五入成整数               
int ELSAPI_stdmath_cut(els_VmObj* vm); // cut( num:float, x:int): float             #截取小数到若干位
#ifdef ELS_CONF_TOKEN_CN
static const char LibScript_cn[]={
10,
-27,-100,-122,-27,-111,-88,-25,-114,-121,32,61,32,80,73,10,
-24,-121,-86,-25,-124,-74,-27,-72,-72,-26,-107,-80,32,61,32,69,10,
-26,-84,-89,-26,-117,-119,-27,-72,-72,-26,-107,-80,32,61,32,71,65,77,77,65,10,
10,
-25,-69,-99,-27,-81,-71,-27,-128,-68,32,61,32,97,98,115,10,
-27,-113,-106,-26,-88,-95,32,61,32,109,111,100,10,
-27,-113,-115,-28,-67,-103,-27,-68,-90,32,61,32,97,99,111,115,10,
-27,-113,-115,-26,-83,-93,-27,-68,-90,32,61,32,97,115,105,110,10,
-27,-113,-115,-26,-83,-93,-27,-120,-121,32,61,32,97,116,97,110,10,
-28,-67,-103,-27,-68,-90,32,61,32,99,111,115,10,
-26,-83,-93,-27,-68,-90,32,61,32,115,105,110,10,
-26,-83,-93,-27,-120,-121,32,61,32,116,97,110,10,
-27,-81,-71,-26,-107,-80,32,61,32,108,111,103,10,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,35,101,120,112,-24,-75,-73,-27,-112,-115,-25,-87,-70,-25,-68,-70,10,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,35,108,110,-24,-75,-73,-27,-112,-115,-25,-87,-70,-25,-68,-70,10,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,35,108,103,-24,-75,-73,-27,-112,-115,-25,-87,-70,-25,-68,-70,10,
-27,-68,-128,-27,-71,-77,-26,-106,-71,32,61,32,115,113,114,116,10,
-27,-112,-111,-28,-72,-118,-27,-113,-106,-26,-107,-76,32,61,32,99,101,105,108,10,
-27,-112,-111,-28,-72,-117,-27,-113,-106,-26,-107,-76,32,61,32,102,108,111,111,114,10,
-27,-101,-101,-24,-120,-115,-28,-70,-108,-27,-123,-91,32,61,32,114,111,117,110,100,10,
-28,-65,-99,-25,-107,-103,-28,-67,-115,-26,-107,-80,32,61,32,99,117,116,10,
10,
0};
#endif
void ElsLib_stdmath_libinit(els_VmObj *vm){
	vm_register(vm,"mod",ELSAPI_stdmath_mod);
	vm_register(vm,"E",ELSAPI_stdmath_E);
	vm_register(vm,"sqrt",ELSAPI_stdmath_sqrt);
	vm_register(vm,"abs",ELSAPI_stdmath_abs);
	vm_register(vm,"cut",ELSAPI_stdmath_cut);
	vm_register(vm,"GAMMA",ELSAPI_stdmath_GAMMA);
	vm_register(vm,"asin",ELSAPI_stdmath_asin);
	vm_register(vm,"atan",ELSAPI_stdmath_atan);
	vm_register(vm,"exp",ELSAPI_stdmath_exp);
	vm_register(vm,"cos",ELSAPI_stdmath_cos);
	vm_register(vm,"round",ELSAPI_stdmath_round);
	vm_register(vm,"ln",ELSAPI_stdmath_ln);
	vm_register(vm,"floor",ELSAPI_stdmath_floor);
	vm_register(vm,"ceil",ELSAPI_stdmath_ceil);
	vm_register(vm,"log",ELSAPI_stdmath_log);
	vm_register(vm,"acos",ELSAPI_stdmath_acos);
	vm_register(vm,"lg",ELSAPI_stdmath_lg);
	vm_register(vm,"sin",ELSAPI_stdmath_sin);
	vm_register(vm,"tan",ELSAPI_stdmath_tan);
	vm_register(vm,"PI",ELSAPI_stdmath_PI);
	#ifdef ELS_CONF_TOKEN_CN
		vm_dostring(vm,LibScript_cn);
	#endif
};
