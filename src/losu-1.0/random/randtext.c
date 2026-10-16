#include "randtext.h"
#include "crandom.h"

extern uint64_t randint(uint64_t, uint64_t);

void randstr(char *result, size_t len)
{
    for (size_t i = 0; i < len; i++)
        result[i] = LOSU_RANDOM_RANDSTR_DEFAULT_STRING[randint(0, 95)];
}

void randbytes(uint8_t *result, size_t len)
{
    for (size_t i = 0; i < len; i++)
        result[i] = rand48() % 0xFF;
}
