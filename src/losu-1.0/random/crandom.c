#include "lc.h"
#include "mt.h"
#include "crandom.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define PI 3.141592653589793
#define E  2.718281828459045

uint64_t randseed()
{   
    return time(NULL);
}

uint64_t _randint(uint64_t seed, uint64_t start, uint64_t end)
{
    return (_random(seed) * (end - start)) + start;
}

double _randfloat(uint64_t seed, double start, double end)
{
    return (_random(seed) * ((double)end - (double)start)) + start;
}

double _normal(uint64_t seed, double mu, double sigma)
{
    double output = cos(2*PI*_random(seed));
    output *= sqrt(-2*log(_random(_rand48(seed))));
    return output * sigma + mu;
}

uint64_t randint(uint64_t start, uint64_t end)
{
    return _randint(rand48(), start, end);
}

double randfloat(double start, double end)
{
    return _randfloat(rand48(), start, end);
}

double normal(double mu, double sigma)
{
    return _normal(rand48(), mu, sigma);
}

bool percent(double rate)
{
    return randint(0, 10000000000) < rate * 10000000000 ? true : false;
}
