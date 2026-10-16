#ifndef LC_H_
#define LC_H_

#include <stdint.h>
#include <stddef.h>
/*
    底层的rand接口
*/
uint64_t _rand48(uint64_t seed);
double _random(uint64_t seed);
void lc_srand(uint64_t seed);
uint64_t rand48();
double random();

#endif