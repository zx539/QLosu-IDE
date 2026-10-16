#!/bin/owls.wiki.losu.tech

# 4-4-2-洛书对象-LosuObj
## 4-4-2-1-概述
+ 对象API是以 obj_ 为前缀的一系列函数。
+ 对象API提供了在 C 中访问 脚本 对象的一系列接口。在模块开发中占用很高的使用频率。
+ 对象API本身也是使用面向对象的思想设计的，这些函数的参数排列都是基于 els_VmObj 与 被操作对象

## 4-4-2-2-数据类型
```C++
typedef struct 
    {
        // 建议使用 API 访问内部成员，以获取最大向后兼容性
    } StackObj;
typedef StackObj LosuObj;
```
## 4-4-2-3-对象API

### 4-4-2-3-1-转换型 API
+ 这一类 API 可以将 LosuObj 转换为 Ctype，实现 洛书 到 C 的数据传递
+ 获取 LosuObj 的内部类型
    - `ELS_API int             obj_type(els_VmObj* vm,LosuObj* o);`
    - 返回值与类型的参考详见 `els.h`中 ELS_API_TYPE_ 变量

+ 转换函数

```C++
ELS_API const char*     obj_tostr(els_VmObj* ,LosuObj *);
ELS_API Number          obj_tonum(els_VmObj* ,LosuObj* );
ELS_API char*           obj_toptr(els_VmObj*,LosuObj*);
ELS_API VmCapi          obj_tofunction(els_VmObj*,LosuObj*);
ELS_API const char*     obj_tobyte(els_VmObj *,LosuObj *);
```
### 4-4-2-3-2-创建型 API
+ 这一类 API 可以将 Ctype 转换为 LosuObj，实现 C 到 洛书 的数据传递
+ 该类 API 会创建 LosuObj 到虚拟机中，但其不会被标记，如果不被绑定到某一脚本结点上，其会在下一个周期被回收

```C++
ELS_API LosuObj         obj_newstr(els_VmObj* ,char* );
ELS_API LosuObj         obj_newnum(els_VmObj* ,Number );
ELS_API LosuObj         obj_newfunction(els_VmObj*,els_C_API_function );
ELS_API LosuObj         obj_newnull(els_VmObj* );
ELS_API LosuObj         obj_newunit(els_VmObj* );
ELS_API LosuObj         obj_newptr(els_VmObj*,char* );
ELS_API LosuObj         obj_newbyte(els_VmObj*,char*,size_t );

```

### 4-4-2-3-3-索引型API
+ 这一类 API 主要是针对 对象与数组 的成员 访问与修改

```C++
ELS_API LosuObj*        obj_indexunit(els_VmObj*vm,LosuObj o ,LosuObj key); // 获取 o[key] 或 o.key
ELS_API void            obj_setunit(els_VmObj*vm,LosuObj o,LosuObj key,LosuObj v); // o.key = v
```
+ 为了更方便地进行索引，同时支持
```C++
ELS_API LosuObj*        obj_indexunitbynum(els_VmObj*vm,LosuObj unit,Number i);
ELS_API LosuObj*        obj_indexunitbystr(els_VmObj*vm,LosuObj unit,char* s);
ELS_API void            obj_setunitbynum(els_VmObj*,LosuObj unit,Number key ,LosuObj value);
ELS_API void            obj_setunitbystr(els_VmObj*,LosuObj unit,char* key ,LosuObj value);
```

### 4-4-2-3-4-unit遍历API

+ 用以遍历数组或对象型的unit

```C++
ELS_API Node *          obj_unit_first(els_VmObj*vm,LosuObj unit);  // 获取第一个结点
ELS_API Node            obj_unit_location(els_VmObj*vm,LosuObj unit,LosuObj key);    // 根据 key 值定位到结点  
ELS_API Node *          obj_unit_next(els_VmObj*vm,LosuObj unit,Node*n);    // 获取下一个结点
ELS_API LosuObj         obj_unit_nodekey(els_VmObj*vm,Node* n);             // 获取 key
ELS_API LosuObj         obj_unit_nodevalue(els_VmObj*vm,Node* n);           // 获取 value
```

+ 样例

```C++
int ELSAPI_test_test(els_VmObj* vm){ //对应脚本函数 test()
    LosuObj u = *arg_get(vm,1);
    Node * n = obj_unit_first(vm,u);
    while (n!=NULL)
    {
        LosuObj k = obj_unit_nodekey(vm,n),v = obj_unit_nodekey(vm,n);
        printf("key: %s \t val: %s\n",obj_tostr(vm,&k),obj_tostr(vm,&v));
        n = obj_unit_next(vm,u,n);
    }
    return 0;   
}

```