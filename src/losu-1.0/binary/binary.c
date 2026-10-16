#include "binary.h"
#include "losubinary.h"

#include <ctype.h>

#ifdef LOSU_WINDOWS

static void __print(const char *s)
{
    /*
        为Windows设计
        会进行utf8到gbk的编码转化
    */
    char *tmp = vm_win_togbk(s);
    printf("%s", tmp);
    free(tmp);
}

#else
#ifdef LOSU_LINUX

static inline void __print(const char *s)
{
    /*
        为Linux设计
        会进行utf8到gbk的编码转化
    */

    printf("%s", s);
}

#endif
#endif

int ELSAPI_binary_binary_create(els_VmObj* vm)
{
    /*
        创建一块二进制内存储存区

        # 参数
            1. size:int 要创建的储存区的大小
        
        # 返回值
            1. 返回创建的储存区的指针
    */

    uint64_t size = arg_getnum(vm, 1);

    arg_returnptr(vm, (void *)barray_create(size));
    return 1;
}

int ELSAPI_binary_binary_free(els_VmObj* vm)
{
    /*
        释放为储存区分配的内存

        # 参数
            1. p:ptr 储存区的指针

        #返回值
            无
    */

    barray *array = (void*)arg_getptr(vm, 1);

    barray_free(array);
    return 1;
}

int ELSAPI_binary_binary_writef(els_VmObj* vm)
{
    /*
        将二进制储存区的数据写入指定的文件中

        # 参数
            1. p:ptr 二进制储存区的指针
            2. fname:str 要写入的文件的名称
            3. len:int   要写入的字节数，缺省时为储存的数据的大小

        # 返回值
            1. 写入的实际字节数
    */

    barray *a = (void*)arg_getptr(vm, 1);
    const char *fname = arg_getstr(vm, 2);
    uint64_t len;
    FILE *fp = fopen(fname, "wb");

    if (fp == NULL)
    {
        arg_returnnull(vm);
        return 1;
    }

    if (arg_gettype(vm, 3) == ELS_API_TYPE_NUMBER)
        len = arg_getnum(vm, 3);
    else
        len = a->length;

    if (len >= a->length)
        len = a->length;

    fseek(fp, 0, SEEK_SET);
    fwrite(a->val, 1, len, fp);
    
    fclose(fp);
    arg_returnnum(vm, len);

    return 1;
}

int ELSAPI_binary_binary_writem(els_VmObj* vm)
{
    /*
        将储存区的数据以指定字节数写入内存中

        # 参数
            1. p:ptr 储存区的指针
            2. m:ptr 要写入的内存的指针
            3. len:int 要写入的字节数
        
        # 返回值
            1. 写入的实际字节数
            2. 当len<0时,返回null
    */
    barray *a = (void*)arg_getptr(vm, 1);
    uint8_t *p = (void*)arg_getptr(vm, 2);
    uint64_t len = arg_getnum(vm, 3);

    if (len >= a->length)
        len = a->length;

    else if (len < 0)
    {
        arg_returnnull(vm);
        return 1;
    }

    for (uint64_t i = 0; i < len; i++)
        p[i] = a->val[i];
    
    arg_returnnum(vm, len);

    return 1;
}

int ELSAPI_binary_binary_readf(els_VmObj* vm)
{
    /*
        从文件中读取指定字节数
        原本的数据会被释放

        # 参数
            1. p:ptr 储存区指针
            2. fname:str 要读取的文件名
            3. len:int 要读取的字节数

            当len缺省时,会被设置为文件大小
        
        # 返回值
            1. 新的储存区的指针
            2. 当打开文件失败时返回null
    */
    barray *a = (void*)arg_getptr(vm, 1);
    const char *fname = arg_getstr(vm, 2);
    uint64_t len;
    FILE *fp = fopen(fname, "rb");
    uint64_t flen;

    if (fp == NULL)
    {
        arg_returnnull(vm);
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    flen = ftell(fp);

    if (arg_gettype(vm, 3) == ELS_API_TYPE_NUMBER)
        len = (len = arg_getnum(vm, 3)) <= 0 ? flen : len;
    else
        len = flen;

    if (flen < len)
        len = flen;

    free(a->val);
    a->val = malloc(len);

    fseek(fp, 0, SEEK_SET);
    fread(a->val, 1, len, fp);

    fclose(fp);
    a->length = len;
    arg_returnptr(vm, (void*)a);
    return 1;
}

int ELSAPI_binary_binary_readm(els_VmObj* vm)
{
    /*
        从内存中读取指定字节的数据

        # 参数
            1. p:ptr 储存区的指针
            2. m:ptr 要读取的内存的指针
            3. len:int 要读取的字节数
        
        # 返回值
            1. 新的储存区的指针
            2. 当len的值<=0、m为0或NULL时,返回null
    */
    barray *a = (void*)arg_getptr(vm, 1);
    uint8_t *p = (void*)arg_getptr(vm, 2);
    uint64_t len = arg_getnum(vm, 3);

    if (len <= 0 || p == 0 || p == NULL)
    {
        arg_returnnull(vm);
        return 1;
    }

    if (!(len==a->length))
        a = barray_change(a, len);
    
    for (uint64_t i = 0; i < len; i++)
        a->val[i] = p[i];

    arg_returnptr(vm, (void*)a);

    return 1;
}

int ELSAPI_binary_binary_get(els_VmObj* vm)
{
    /*
        从储存区的指定位置读取指定个数的字节

        # 参数
            1. p:ptr 储存区指针
            2. i:int 索引
            3. size:int 读取的字节个数
        
        # 返回值
            1. 读取的字节值
            2. 当索引越界时，返回null
    */
    barray *a = (void*)arg_getptr(vm, 1);
    uint64_t index = arg_getnum(vm, 2);
    uint64_t size = arg_getnum(vm, 3);
    uint64_t output = 0;
    uint64_t alen = a->length;

    if (index <0 || index + size > alen)
        arg_returnnull(vm);
    else
    {
        memcpy(&output, a->val + index, size);
        arg_returnnum(vm, output);
    }

    return 1;
}

int ELSAPI_binary_binary_insert(els_VmObj* vm)
{
    /*
        向储存区的指定位置插入指定个数的字节
        会替换掉原本的字节

        # 参数
            1. p:ptr 储存区的指针
            2. i:int 索引
            3. d:要插入的数据
            4. size:int 要写入的数据的大小，单位为字节
        
        # 返回值
            1. 实际插入的数据
    */
    barray *a = (void*)arg_getptr(vm, 1);
    uint64_t index = arg_getnum(vm, 2);
    uint64_t n = arg_getnum(vm, 3);
    uint8_t size = arg_getnum(vm, 4);
    uint64_t alen = a->length;

    if (index <0 || index + size > alen)
        arg_returnnull(vm);
    else
    {
        memcpy(a->val + index, &n, size);
        arg_returnnum(vm, n);
    }

    return 1;
}

int ELSAPI_binary_binary_insertp(els_VmObj* vm)
{
    /*
        向储存区的指定位置插入指定个数的字节
        会替换掉原本的字节

        # 参数
            1. a:ptr 储存区的指针
            2. i:int 索引
            3. p:要插入的数据
            4. size:int 要写入的数据的大小，单位为字节
        
        # 返回值
            1. 实际插入的数据
    */
    barray *a = (void*)arg_getptr(vm, 1);
    uint64_t index = arg_getnum(vm, 2);
    void *p = arg_getptr(vm, 3);
    uint64_t size = arg_getnum(vm, 4);
    uint64_t alen = a->length;

    if (index <0 || index + size > alen)
        arg_returnnull(vm);
    else
    {
        memcpy(a->val + index, p, size);
        arg_returnptr(vm, p);
    }

    return 1;
}

int ELSAPI_binary_binary_cat(els_VmObj* vm)
{
    /*
        将b附加到a尾
        这会改变a的值，不会改变b的值

        # 参数
            1. a:ptr 被附加的储存区指针
            2. b:ptr 用于附加的储存区指针
        
        # 返回值
            1. 返回a的新指针值
    */
    barray *a = (void*)arg_getptr(vm, 1);
    barray *b = (void*)arg_getptr(vm, 2);
    barray *n = barray_create(a->length + b->length);

    memcpy(n->val, a->val, a->length);
    memcpy(n->val + a->length, b->val, b->length);
    barray_free(a);

    arg_returnptr(vm, (void*)n);
    return 1;
}

int ELSAPI_binary_binary_replace(els_VmObj* vm)
{
    /*
        将b中的数据从start处开始赋盖a中的数据，会覆盖len个字节
        覆盖到a的结尾处后会停止覆盖
    
       # 参数
            1. a:ptr 被覆盖的储存区指针
            2. b:ptr 用于覆盖的储存区指针
            3. start 覆盖的起始位置
            4. len 覆盖的字节数
        
        # 返回值
            1. 返回a的新指针值
    */
    barray *a = (void*)arg_getptr(vm, 1);
    barray *b = (void*)arg_getptr(vm, 2);
    uint64_t start = arg_getnum(vm, 3);
    uint64_t len = arg_getnum(vm, 4);
    
    if (start < 0 || start > a->length || len <= 0)
    {
        arg_returnptr(vm, (void* )a);
        return 1;
    }

    if (start + len > a->length)
        len = a->length - start;

    memcpy(a->val + start, b->val, len);
    arg_returnptr(vm, (void*)a);
    return 1;
}

int ELSAPI_binary_binary_length(els_VmObj* vm)
{
    /*
        返回储存区的大小

        # 参数
            无
        
        # 返回值
            1. 储存区的大小
    */
    barray * a = (void*)arg_getptr(vm, 1);
    arg_returnnum(vm, a->length);
    return 1;
}

int ELSAPI_binary_binary_change(els_VmObj* vm)
{
    /*
        改变储存区的大小
        在扩大后，数据会被全部复制
        在缩小后，数据会被截断

        # 参数
            1. p:ptr 储存区的指针
            2. len:int 指定的大小
        
        # 返回值
            1. 新的储存区指针
    */
    barray *a = (void*)arg_getptr(vm, 1);
    uint64_t len = arg_getnum(vm, 2);
    barray_change(a, len);

    arg_returnptr(vm, (void*)a);

    return 1;
}

int ELSAPI_binary_binary_copy(els_VmObj* vm)
{
    /*
        将储存区复制
        不会改动原储存区的数据

        # 参数
            1. p:ptr 要复制的储存区指针
        
        # 返回值
            1. 新创建的储存区指针
    */
    barray *a = (void*)arg_getptr(vm, 1);
    barray *newa =barray_create(a->length);
    
    for (uint64_t i = 0; i < a->length; i++)
        newa->val[i] = a->val[i];
    
    arg_returnptr(vm, (void*)newa);

    return 1;
}

int ELSAPI_binary_binary_set(els_VmObj* vm)
{
    /*
        将储存区内的所有数据设置为n
        
        # 参数
            1. p:ptr 储存区的指针
            2. n:int 要设置的数据
        
        # 返回值
            1. 修改后的储存区指针
    */
    barray *a = (void*)arg_getptr(vm, 1);
    uint8_t n = (long long)arg_getnum(vm, 2);
    barray_set(a, n);
    arg_returnnum(vm, 1);
    return 1;
}

int ELSAPI_binary_binary_slice(els_VmObj* vm)
{
    /*
        将储存区切片
        start和end处的数据都将被包括

        # 参数
            1. p:ptr 储存区指针
            2. start:int 切片起始处
            3. end:int 切片终止处
        
        # 返回值
            1. 新创建的储存区对象
    */
    barray *a = (void*)arg_getptr(vm, 1);
    uint64_t start = arg_getnum(vm, 2);
    uint64_t end = arg_getnum(vm, 3);

    if (start <0 || end < start || start > a->length)
    {
        arg_returnnull(vm);
        return 1;
    }

    if (start == end)
    {
        barray *b = barray_create(1);
        b->val[0] = a->val[start];
        arg_returnptr(vm, (void*)b);
        return 1;
    }
    
    barray *b = barray_slice(a, start, end);
    arg_returnptr(vm, (void*)b);

    return 1;
}

int ELSAPI_binary_binary_tounit(els_VmObj* vm)
{
    /*
        将储存区中的数据按字节转化为洛书number类型数据并储存到unit中返回
        返回的unit从1开始索引

        # 参数
            1. p:ptr 储存区的指针
        
        # 返回值
            1. 转化而成的unit
    */

    barray *a = (void*)arg_getptr(vm, 1);
    LosuObj unit = obj_newunit(vm);
    uint64_t i;

    for (i = 0; i < a->length; i++)
        obj_setunit(vm, unit, obj_newnum(vm, i + 1), obj_newnum(vm, a->val[i]));
    obj_setunit(vm, unit, obj_newnum(vm, i + 1), obj_newnull(vm));
    
    arg_return(vm, unit);

    return 1;
}

int ELSAPI_binary_binary_print(els_VmObj* vm)
{
    /*
        将储存区中的数据按字节以16进制大写的格式打印到控制台

        # 参数
            1. p:ptr 储存区的指针
        
        # 返回值
            1. 打印的字节数
    */
    barray *a = (void *)arg_getptr(vm, 1);
    uint64_t i, j;
    char buffer[16];

    for (i = 0; i + 8 < a->length; i += 8)
    {
        for (j = 0; j < 7; j ++)
        {
            sprintf(buffer, "0x%-3X", a->val[i + j]);
            __print(buffer);
        }
        sprintf(buffer, "0x%-3X\n", a->val[i + j]);
        __print(buffer);
    }

    for (; i < a->length; i++)
    {
        sprintf(buffer, "0x%-3X", a->val[i]);
        __print(buffer);
    }

    __print("\n");
    arg_returnnum(vm, i);
    return 1;
}

int ELSAPI_binary_binary_printf(els_VmObj* vm)
{
    /*
        将储存区中的数据按字节以16进制大写的格式写入文件

        # 参数
            1. p:ptr 储存区的指针
            2. fname:str 要写入的文件的名称
        
        # 返回值
            1. 写入的字节数
    */

    barray *a = (void *)arg_getptr(vm, 1);
    const char *fname = arg_getstr(vm, 2);
    uint64_t i, j;
    FILE *fp = fopen(fname, "w");

    if (fp == NULL)
    {
        arg_returnnull(vm);
        return 1;
    }

    for (i = 0; i + 8 < a->length; i += 8)
    {
        for (j = 0; j < 7; j ++)
            fprintf(fp, "0x%-3X", a->val[i + j]);
        fprintf(fp, "0x%-3X\n", a->val[i + j]);
    }

    for (; i < a->length; i++)
        fprintf(fp, "0x%-3X", a->val[i]);

    fprintf(fp, "\n");
    fclose(fp);
    arg_returnnum(vm, i);
    return 1;
}

int ELSAPI_binary_binary_prints(els_VmObj* vm)
{/*
        将储存区中的数据按字节以16进制大写的格式转化成字符串并返回

        # 参数
            1. p:ptr 储存区的指针
        
        # 返回值
            1. 转化后的字符串
    */
    barray *a = (void *)arg_getptr(vm, 1);
    uint64_t i, j;
    char *buffer = malloc(a->length * (6 * sizeof(char)));
    char tmp[16];
    uint64_t index = 0;

    for (i = 0; i + 8 < a->length; i += 8)
    {
        for (j = 0; j < 7; j ++)
        {
            sprintf(tmp, "0x%-3X", a->val[i + j]);
            strcpy(buffer + index, tmp);
            index += strlen(tmp);
        }
        sprintf(tmp, "0x%-3X\n", a->val[i + j]);
        strcpy(buffer + index, tmp);
        index += strlen(tmp);
    }

    for (; i < a->length; i++)
    {
        sprintf(tmp, "0x%-3X", a->val[i]);
        strcpy(buffer + index, tmp);
        index += strlen(tmp);
    }

    arg_returnstr(vm, buffer);
    free(buffer);
    return 1;
}

int ELSAPI_binary_binary_scans(els_VmObj* vm)
{
    /*
        接受一个字符串，里面储存着十六进制数字，相互以空白字符分割，返回生成的二进制储存区

        # 参数
            1. p:ptr 二进制储存区的指针
            2. s:str 要处理的字符串
        
        # 返回值
            1. 二进制储存区的指针
    */
    barray *a = (void *)arg_getptr(vm, 1);
    const char *s = arg_getstr(vm, 2);
    char ch;
    uint64_t index = 0;
    uint32_t n;
    uint64_t slen = strlen(s);

    for (uint64_t i = 0; i < slen; i++)
    {
        if ((ch = s[i]) == '\0')
            break;
        else if (isspace(ch)==0)
        {
            if (index > a->length)
                a = barray_change(a, a->length+1024);
            sscanf(s+i, "0x%X", &n);
            a->val[index] = (uint8_t)n;

            for (;i < slen; i++)
                if (isspace((ch = s[i])) != 0)
                    break;
            index++;
        }
    }

    a->length = index;
    arg_returnptr(vm, (void *)a);
    return 1;
}

int ELSAPI_binary_binary_fromunit(els_VmObj* vm)
{
    /*
        接受一个unit，从1处开始索引，直至索引到null。将索引范围内所有属于number类型的数据储存到储存区中。
        对储存区中的数据每8字节转化一个number型数据

        将会覆盖储存区原有的数据

        # 参数
            1. p:ptr 储存区指针
            2. u:unit 要转化的unit
        
        # 返回值
            1. 新的储存区指针
    */
    if (!(arg_gettype(vm, 1) == ELS_API_TYPE_UNIT) && !(arg_gettype(vm, 2) ==  ELS_API_TYPE_UNIT))
    {
        arg_returnnull(vm);
        return 1;
    }

    barray *old = (void*)arg_getptr(vm, 1);
    barray *n = NULL;
    LosuObj *unit = arg_get(vm, 2);
    uint64_t unitlen = 0;
    uint64_t newlen = 1024;

    n = barray_create(newlen);

    //释放之前的内存
    barray_free(old);
    
    //获取unit的大小
    for (uint64_t i = 1; ; i++)
    {
        LosuObj *val = obj_indexunit(vm, *unit, obj_newnum(vm, i));
        int type = obj_type(vm, val);

        if (type == ELS_API_TYPE_NUMBER)
        {   
            if (unitlen >= newlen - 1)
                n = barray_change(n, newlen + 1024);

            barray_insert(n, unitlen, obj_tonum(vm, val));
            unitlen++;
            continue;
        }
        
        break;
    }
    n->length = unitlen;
    arg_returnptr(vm, (void*)n);

    return 1;
}

int ELSAPI_binary_binary_tostr(els_VmObj* vm)
{
    /*
        将储存区的数据直接以字符串形式返回
        会在末尾附加'\0'

        # 参数
            1. p:ptr 储存区指针
        
        # 返回值
            1. 转化后的字符串
    */
    barray *a = (void*)arg_getptr(vm, 1);
    char *tmp = malloc(a->length + sizeof(char));
    
    memcpy(tmp, a->val, a->length);
    tmp[a->length / sizeof(char) + 1] = '\0';
    arg_returnstr(vm, tmp);
    free(tmp);

    return 1;
}

int ELSAPI_binary_binary_fromstr(els_VmObj* vm)
{
    /*
        将字符串直接转化为储存区数据
        会去除末尾附加的'\0'

        # 参数
            1. p:ptr 储存区指针
            2. s:str 被转化的字符串
        
        # 返回值
            1. 新的储存区指针
    */
    barray *a = (void*)arg_getptr(vm, 1);
    const char *str = arg_getstr(vm, 2);
    uint64_t slen = strlen(str);
    uint64_t cpylen = slen * sizeof(char);

    free(a->val);
    a->val = malloc(cpylen);
    memcpy(a->val, str, cpylen);
    a->length = cpylen;

    arg_returnptr(vm, (void*)a);

    return 1;
}

int ELSAPI_binary_binary_tonum(els_VmObj* vm)
{
    /*
        将储存区数据转化为洛书number类型，结果储存在unit中，从1开始索引

        # 参数
            1. p:ptr 储存区指针

        # 返回值
            1. 返回储存结果的unit
    */
    barray *a = (void*)arg_getptr(vm, 1);
    LosuObj unit = obj_newunit(vm);

    for (uint64_t i = 0; i < a->length; i += sizeof(double))
    {
        obj_setunit(vm, unit, obj_newnum(vm, i / sizeof(double)), obj_newnum(vm, *(double*)(void*)(a->val+i)));
    }

    arg_return(vm, unit);

    return 1;
}

int ELSAPI_binary_binary_fromnum(els_VmObj* vm)
{
    /*
        将储存在unit中的洛书number型数据储存在储存区中
        从unit的1处开始索引，直至索引到null
        每个number型数据占8字节

        # 参数
            1. p:ptr 储存区指针
            2. u:unit 储存着洛书numbe型数据的单元

        # 返回值
            1. 新的储存区指针
    */
    barray *old = (void*)arg_getptr(vm, 1);
    barray *n = NULL;
    LosuObj *unit = arg_get(vm, 2);
    uint64_t unitlen = 0;
    uint64_t newlen = 1024;

    n = barray_create(newlen);

    //释放之前的内存
    barray_free(old);
    
    for (uint64_t i = 1; ; i++)
    {
        LosuObj *val = obj_indexunit(vm, *unit, obj_newnum(vm, i));
        int type = obj_type(vm, val);

        if (type == ELS_API_TYPE_NUMBER)
        {   
            if (unitlen >= newlen - 1)
                n = barray_change(n, newlen + 1024);

            *(double *)(void*)(n->val+unitlen) = obj_tonum(vm, val);
            unitlen += sizeof(double);
            continue;
        }
        
        break;
    }
    n->length = unitlen;
    arg_returnptr(vm, (void*)n);

    return 1;
}

int ELSAPI_binary_binary_toptr(els_VmObj* vm)
{
    /*
        将储存区数据转化为洛书pointer类型，结果储存在unit中，从1开始索引

        # 参数
            1. p:ptr 储存区指针

        # 返回值
            1. 返回储存结果的unit
    */

    barray *a = (void*)arg_getptr(vm, 1);
    LosuObj unit = obj_newunit(vm);

    for (uint64_t i = 0; i < a->length; i += sizeof(void*))
    {
        obj_setunit(vm, unit, obj_newnum(vm, i / sizeof(void*)), obj_newptr(vm, (void*)a->val+i));
    }

    arg_return(vm, unit);
    return 1;
}
