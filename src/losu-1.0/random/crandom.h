#ifndef RANDOM_H_
#define RANDOM_H_

/*
随机数模块
作者：Matriller
创建时间：2023-9-28
维护者：暂无
*/

#include "lc.h" //线性同余
#include "mt.h" //梅森旋转
#include "randtext.h" //随机文本和字节

#include <stdint.h>
#include <stdbool.h>

/*
    返回8字节的随机种子
*/
uint64_t randseed();

/*
    底层randint
    seed是随机种子，一般是由rand48或_rand48生成
*/
uint64_t _randint(uint64_t seed, uint64_t start, uint64_t end);

/*
    底层randfloat
*/
double _randfloat(uint64_t seed, double start, double end);

/*
    底层normal
*/
double _normal(uint64_t seed, double mu, double sigma);

/*
    生成[start, end)的随机整数
*/
uint64_t randint(uint64_t start, uint64_t end);

/*
    生成[start, end)的随机实数
*/
double randfloat(double start, double end);

/*
    生成方差为sigma，数学期望为mu的生态分布随机数
*/
double normal(double mu, double sigma);

/*
    以rate的几率返回true
*/
bool percent(double rate);

#endif
