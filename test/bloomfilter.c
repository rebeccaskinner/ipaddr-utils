#include <stdio.h>
#include <limits.h>
#include "bloomfilter.h"

#define BLOOMFILTER_HASH bloomfilter_hash1

void bloomfilter_init(bloomfilter_t* bf, int val)
{
    *bf = (bloomfilter_t){val,val,val,val};
}

void bloomfilter_show(bloomfilter_t bf)
{
    printf("%u:%u:%u:%u\n",bf[0], bf[1], bf[2], bf[3]);
}

bloomfilter_t octets_to_vector(uint32_t addr)
{
    bloomfilter_t bf = (bloomfilter_t){
        (addr >> 24),
        (addr >> 16) & ((2<<24)-1),
        (addr >>  8) & ((2<<16)-1),
        (addr      ) & ((2<<8)-1)
    };
    return bf;
}

uint32_t vector_to_octets(bloomfilter_t bf)
{
    return (uint32_t)(bf[0]<<24 | bf[1]<<16 | bf[2]<<8 | bf[3]);
}

/* vectorized hash function based on bob jenkins 32bit hash function */
static bloomfilter_t bloomfilter_hash1(bloomfilter_t bf)
{
    bf = ~bf + (bf << 15);
    bf =  bf ^ (bf >> 12);
    bf =  bf + (bf <<  2);
    bf =  bf ^ (bf >>  4);
    bf =  bf * (bloomfilter_t){2057,2057,2057,2057};
    bf =  bf ^ (bf >> 16);
    return bf;
}

static bloomfilter_t bloomfilter_hash2(bloomfilter_t bf)
{
    return bf;
}

void bloomfilter_insert(bloomfilter_t* bf, uint32_t addr)
{
    *bf |= BLOOMFILTER_HASH(octets_to_vector(addr));
}

int bloomfilter_check(bloomfilter_t* bf, uint32_t addr)
{
    bloomfilter_t hashed = BLOOMFILTER_HASH(octets_to_vector(addr));
    bloomfilter_t ref    = *bf & hashed;
    return ((ref[0] == hashed[0]) &&
            (ref[1] == hashed[1]) &&
            (ref[2] == hashed[2]) &&
            (ref[3] == hashed[3]));
}

