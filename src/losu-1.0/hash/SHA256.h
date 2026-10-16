#ifndef LOSU_SHA256_H__
#define LOSU_SHA256_H__

/*
    使用方法:
        1.构造SHA256_t
        2.传入void SHA256(SHA256_t *task)中
        3.从SHA256_t.result中得到结果
*/


#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    const unsigned char *content;
    unsigned char result[65];
} SHA256_t;

void __SHA256__(const unsigned char *data, size_t len, unsigned char *out);
void SHA256(SHA256_t *task);

#endif