#include "els.h"
// # 常量定义
int ELSAPI_lsmath_PI(els_VmObj* vm); // PI():                   float             返回π的值
int ELSAPI_lsmath_E(els_VmObj* vm); // E():                    float             返回e的值 
int ELSAPI_lsmath_GAMMA(els_VmObj* vm); // GAMMA():                float             返回γ（欧拉常数）的值
// #标准数学函数
//     #杂项函数
int ELSAPI_lsmath_abs(els_VmObj* vm); // abs( num )                                返回数的绝对值
int ELSAPI_lsmath_mod(els_VmObj* vm); // mod( a, b )                               返回a mod b的结果
int ELSAPI_lsmath_sqrt(els_VmObj* vm); // sqrt( num:number ):     float             返回num的平方根
int ELSAPI_lsmath_anyroot(els_VmObj* vm); // anyroot( num:number, n:int ):  float      返回对num开n次方的一个近似根
int ELSAPI_lsmath_gcd(els_VmObj* vm); // gcd( a:int, b:int ): int                  返回整数a和b的最大公约数
//     #三角函数
int ELSAPI_lsmath_acos(els_VmObj* vm); // acos( x )                                 返回以弧度表示的x的反余弦
int ELSAPI_lsmath_asin(els_VmObj* vm); // asin( x )                                 返回以弧度表示的x的反正弦
int ELSAPI_lsmath_atan(els_VmObj* vm); // atan( x )                                 返回以弧度表示的x的反正切
int ELSAPI_lsmath_cos(els_VmObj* vm); // cos( x )                                  返回x的余弦值
int ELSAPI_lsmath_sin(els_VmObj* vm); // sin( x )                                  返回x的正弦值
int ELSAPI_lsmath_tan(els_VmObj* vm); // tan( x )                                  返回x的正切值
//     #对数函数
int ELSAPI_lsmath_exp(els_VmObj* vm); // exp( x )                                  返回e的x次幂的值
int ELSAPI_lsmath_log(els_VmObj* vm); // log( a, N )                               返回以a为第N的对数
int ELSAPI_lsmath_ln(els_VmObj* vm); // ln( N )                                   返回以e为底，N的对数
int ELSAPI_lsmath_lg(els_VmObj* vm); // lg( N )                                   返回以10为底，N的对数
//     #取整函数
int ELSAPI_lsmath_ceil(els_VmObj* vm); // ceil( num:float ):      float             对num向上取整
int ELSAPI_lsmath_floor(els_VmObj* vm); // floor( num:float ):     float             对num向下取整
int ELSAPI_lsmath_round(els_VmObj* vm); // round( num:float ):     float             将小数四舍五入成整数               
int ELSAPI_lsmath_cut(els_VmObj* vm); // cut( num:float, x:int): float             截取小数到若干位
//     #位运算
int ELSAPI_lsmath_move_left(els_VmObj* vm); // move_left( num:int, n:int ): int          对num左移n位
int ELSAPI_lsmath_move_right(els_VmObj* vm); // move_right( num:int, n:int ): int         对num右移n位
int ELSAPI_lsmath_bitwise_and(els_VmObj* vm); // bitwise_and( a:int, b:int ): int          返回对a进行b的按位与运算结果
int ELSAPI_lsmath_bitwise_or(els_VmObj* vm); // bitwise_or( a:int, b:int ): int           返回对a进行b的按位或运算结果
int ELSAPI_lsmath_bitwise_not(els_VmObj* vm); // bitwise_not( a:int ): int                 返回对a按位取反运算结果
int ELSAPI_lsmath_bitwise_xor(els_VmObj* vm); // bitwise_xor( a:int, b:int ): int          返回对a进行b的按位异或运算结果
//     #数组运算
int ELSAPI_lsmath_sum(els_VmObj* vm); // sum( a:unit ): number                     返回a中所有数字的和
int ELSAPI_lsmath_even(els_VmObj* vm); // even( a:unit ): number                    返回a中所有数字的平均数
//     #角度运算
int ELSAPI_lsmath_degree(els_VmObj* vm); // degree( x:number ): number                将x从弧度制转化为角度制
int ELSAPI_lsmath_radian(els_VmObj* vm); // radian( x:number ): number                将x从角度制转化为弧度制
#ifdef ELS_CONF_TOKEN_EN
static const char LibScript[]={
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
0};
#endif
void ElsLib_lsmath_libinit(els_VmObj *vm){
	vm_register(vm,"even",ELSAPI_lsmath_even);
	vm_register(vm,"sqrt",ELSAPI_lsmath_sqrt);
	vm_register(vm,"bitwise_or",ELSAPI_lsmath_bitwise_or);
	vm_register(vm,"round",ELSAPI_lsmath_round);
	vm_register(vm,"gcd",ELSAPI_lsmath_gcd);
	vm_register(vm,"bitwise_not",ELSAPI_lsmath_bitwise_not);
	vm_register(vm,"sum",ELSAPI_lsmath_sum);
	vm_register(vm,"bitwise_xor",ELSAPI_lsmath_bitwise_xor);
	vm_register(vm,"floor",ELSAPI_lsmath_floor);
	vm_register(vm,"move_right",ELSAPI_lsmath_move_right);
	vm_register(vm,"acos",ELSAPI_lsmath_acos);
	vm_register(vm,"mod",ELSAPI_lsmath_mod);
	vm_register(vm,"E",ELSAPI_lsmath_E);
	vm_register(vm,"bitwise_and",ELSAPI_lsmath_bitwise_and);
	vm_register(vm,"radian",ELSAPI_lsmath_radian);
	vm_register(vm,"abs",ELSAPI_lsmath_abs);
	vm_register(vm,"sin",ELSAPI_lsmath_sin);
	vm_register(vm,"asin",ELSAPI_lsmath_asin);
	vm_register(vm,"atan",ELSAPI_lsmath_atan);
	vm_register(vm,"cut",ELSAPI_lsmath_cut);
	vm_register(vm,"ceil",ELSAPI_lsmath_ceil);
	vm_register(vm,"cos",ELSAPI_lsmath_cos);
	vm_register(vm,"GAMMA",ELSAPI_lsmath_GAMMA);
	vm_register(vm,"ln",ELSAPI_lsmath_ln);
	vm_register(vm,"log",ELSAPI_lsmath_log);
	vm_register(vm,"move_left",ELSAPI_lsmath_move_left);
	vm_register(vm,"anyroot",ELSAPI_lsmath_anyroot);
	vm_register(vm,"tan",ELSAPI_lsmath_tan);
	vm_register(vm,"degree",ELSAPI_lsmath_degree);
	vm_register(vm,"exp",ELSAPI_lsmath_exp);
	vm_register(vm,"lg",ELSAPI_lsmath_lg);
	vm_register(vm,"PI",ELSAPI_lsmath_PI);
	#ifdef ELS_CONF_TOKEN_EN
		vm_dostring(vm,LibScript);
	#endif
};
