#include "lc.h"

#define MUT 134775813
#define ADD 1
#define MOD 4294967296
#define RAND_MAX_ MOD

uint64_t lcseed = 33;

inline uint64_t _rand48(uint64_t seed)
{
    /*
        线性同余算法
        符合均匀分布
        模数取2^48
    */
    return ((seed * MUT + ADD) % MOD);
}


inline double _random(uint64_t seed)
{
    return (double)_rand48(seed) / (double)RAND_MAX_;
}

void lc_srand(uint64_t seed)
{
    lcseed = seed;
}

inline uint64_t rand48()
{
    return (lcseed = _rand48(lcseed));
}

inline double random()
{
    return _random(rand48());
}
