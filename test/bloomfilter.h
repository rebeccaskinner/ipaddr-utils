/* Copyright Tim Skinner 2012
 * This source code is covered under the GNU Lesser General Public License.
 * See the LICENSE file for the complete text of the license covering this
 * file.
 */
#ifndef __BLOOMFILTER_H__
#define __BLOOMFILTER_H__
#include <stdint.h>

#define BLOOMFILTER_NUM_INTS(bf)   ((bf)->bf_num_ints)
#define BLOOMFILTER_NUM_HASHES(bf) ((bf)->bf_num_hashes)
#define BLOOMFILTER_NUM_BYTES(bf)  (((bf)->bf_num_ints) * sizeof(uint32_t))
#define BLOOMFILTER_NUM_BITS(bf)   ((BLOOMFILTER_NUM_BYTES((bf))) * 8U)
#define BLOOMFILTER_ELEM_IDX(x)    ((x) >> 5U)
#define BLOOMFILTER_BIT_IDX(x)     ((x)&31U)

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
    uint32_t bf_num_ints;
    uint32_t bf_num_hashes;
    uint32_t bf_data[];
} __attribute__((aligned));

typedef struct bloomfilter bloomfilter_t;

bloomfilter_t* bloomfilter_new(int val, uint32_t size, uint32_t hashes);
void bloomfilter_show(bloomfilter_t*);
void bloomfilter_insert(bloomfilter_t*, uint32_t);
int  bloomfilter_check(bloomfilter_t*, uint32_t);

#endif
