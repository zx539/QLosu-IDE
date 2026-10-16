#ifndef RANDOMTEXT_H_
#define RANDOMTEXT_H_

#include "lc.h"

#define LOSU_RANDOM_RANDSTR_DEFAULT_STRING "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
#define LOSU_RANDOM_RANDSTR_DEFAULT_STRING_LENGTH 95

void randstr(char *result, size_t len);
void randbytes(uint8_t *result, size_t len);

#endif