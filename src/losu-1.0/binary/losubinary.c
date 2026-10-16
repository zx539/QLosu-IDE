#include "losubinary.h"

inline barray *barray_create(uint64_t len)
{
    barray *output = malloc(sizeof(barray));
    output->val = malloc(len);
    output->length = len;
    return output;
}

barray *barray_set(barray *array, uint8_t n)
{
    for (uint64_t i = 0; i < array->length; i++)
        array->val[i] = n;
    return array;
}

barray *barray_change(barray *front, uint64_t len)
{
    uint8_t *oldval = NULL;
    uint8_t *newval = NULL;
    uint64_t stopindex;
    
    if (len > front->length)
        stopindex = len;
    else if (len == front->length)
        return front;
    else
        stopindex = front->length;

    oldval = front->val;
    newval = malloc(len);
    for (uint64_t i = 0; i < stopindex; i++)
        newval[i] = front->val[i];
    free(oldval);
    front->val = newval;
    front->length = len;
    return front;
}

barray *barray_slice(barray *array, uint64_t start, uint64_t end)
{
    barray *newarray = barray_create(end - start + 1);

    start = start >= array->length ? array->length - 1 : start;
    end = end >= array->length ? array->length - 1 : end;

    for (uint64_t i = start; i <= end; i++)
        newarray->val[i - start] = array->val[i];
    
    return newarray;
}

inline uint64_t barray_length(barray *array)
{
    return array->length;
}

inline uint8_t barray_get(barray *array, uint64_t index)
{
    return array->val[index];
}

inline uint8_t *barray_getx(barray *array, uint64_t index)
{
    return array->val + index;
}

inline uint8_t barray_insert(barray* array, uint64_t index, uint8_t n)
{
    array->val[index] = n;
    return n;
}

barray *barray_copy(barray *a, barray *b)
{
    uint64_t cpy_len;

    if (b == NULL)
        b = barray_create(barray_length(a));

    cpy_len = a->length >= b->length ? b->length : a->length;

    memcpy(b->val, a->val, cpy_len);
    return b;
}

void barray_free(barray *array)
{
    free(array->val);
    free(array);
}