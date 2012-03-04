#ifndef __BLOOMFILTER_H__
#define __BLOOMFILTER_H__
#include <stdint.h>

/* make the bloom filter a 128-bit wide vector of 4 32-bit integers */
typedef unsigned int v4si __attribute__((vector_size (16)));
typedef v4si bloomfilter_t;

void bloomfilter_init(bloomfilter_t*,int val);
void bloomfilter_show(bloomfilter_t);
void bloomfilter_insert(bloomfilter_t*, uint32_t);
int  bloomfilter_check(bloomfilter_t*, uint32_t);
bloomfilter_t octets_to_vector(uint32_t addr) __attribute__((pure));
uint32_t vector_to_octets(bloomfilter_t bf) __attribute__((pure));

#endif
