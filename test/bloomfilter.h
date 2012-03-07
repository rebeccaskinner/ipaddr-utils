#ifndef __BLOOMFILTER_H__
#define __BLOOMFILTER_H__
#include <stdint.h>

#ifndef BLOOMFILTER_NUM_INTS
    // #define BLOOMFILTER_NUM_INTS (4096 * 24)
    #define BLOOMFILTER_NUM_INTS (8192)
#endif

#ifndef BLOOMFILTER_NUM_HASHES
    #define BLOOMFILTER_NUM_HASHES (5)
#endif

#define BLOOMFILTER_NUM_BYTES   (BLOOMFILTER_NUM_INTS * sizeof(uint32_t))
#define BLOOMFILTER_NUM_BITS    (8U * BLOOMFILTER_NUM_BYTES)
#define BLOOMFILTER_ELEM_IDX(x) ((x) >> 5U)
#define BLOOMFILTER_BIT_IDX(x)  ((x)&31U)

#define BLOOMFILTER_SET_BIT(__bf,__bitnum) \
({ \
    bloomfilter_t* __b   = (__bf); \
    uint32_t       __bn  = (__bitnum); \
    uint32_t       __bit = BLOOMFILTER_BIT_IDX(__bitnum); \
    uint32_t*      __byte = (&((__b->bf_data)[0]))+BLOOMFILTER_ELEM_IDX(__bn); \
    *__byte |= (1U<<__bit); \
})

#define BLOOMFILTER_GET_BIT(__bf,__bitnum) \
({ \
    bloomfilter_t* __b   = (__bf); \
    uint32_t       __bn  = (__bitnum); \
    uint32_t       __bit = BLOOMFILTER_BIT_IDX(__bitnum); \
    uint32_t*      __byte = (&((__b->bf_data)[0]))+BLOOMFILTER_ELEM_IDX(__bn); \
    !!((*__byte) & (1U<<__bit));\
})

struct bloomfilter
{
    uint32_t bf_data[BLOOMFILTER_NUM_INTS];
} __attribute__((aligned));

typedef struct bloomfilter bloomfilter_t;

void bloomfilter_init(bloomfilter_t*,int val);
void bloomfilter_show(bloomfilter_t*);
void bloomfilter_insert(bloomfilter_t*, uint32_t);
int  bloomfilter_check(bloomfilter_t*, uint32_t);

#endif
