#include "els.h"
int ELSAPI_hash_md5(els_VmObj* vm); // md5( content:string ): string                   #md5摘要算法接口，接受一个字符串，返回对应的MD5值
int ELSAPI_hash_sha256(els_VmObj* vm); // sha256( content:string ): string                #sha256摘要算法接口，接受一个字符串，返回对应的sha256值
int ELSAPI_hash_sha384(els_VmObj* vm); // sha384( content:string ): string                #sha384摘要算法接口，接受一个字符串，返回对应的sha384值
int ELSAPI_hash_sha512(els_VmObj* vm); // sha512( content:string ): string                #sha512摘要算法接口，接受一个字符串，返回对应的sha512值
void ElsLib_hash_libinit(els_VmObj *vm){
	vm_register(vm,"sha256",ELSAPI_hash_sha256);
	vm_register(vm,"sha384",ELSAPI_hash_sha384);
	vm_register(vm,"md5",ELSAPI_hash_md5);
	vm_register(vm,"sha512",ELSAPI_hash_sha512);
};
