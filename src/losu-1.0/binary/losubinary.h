#ifndef LOSU_BINARY_H__
#define LOSU_BINARY_H__


/*
    用于为洛书提供二进制操作
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t *val;
    uint64_t length;
} barray;

barray *barray_create(uint64_t len);
barray *barray_set(barray *array, uint8_t n);
barray *barray_change(barray *front, uint64_t len);
barray *barray_slice(barray *array, uint64_t start, uint64_t end);
uint64_t barray_length(barray* array);
uint8_t barray_get(barray *array, uint64_t index);
uint8_t *barray_getx(barray* array, uint64_t index);
uint8_t barray_insert(barray* array, uint64_t index, uint8_t n);
barray *barray_copy(barray *a, barray *b);
void barray_free(barray *array);

#endif