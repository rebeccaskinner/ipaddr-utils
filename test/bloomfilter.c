/* Copyright Tim Skinner 2012
 * This source code is covered under the GNU Lesser General Public License.
 * See the LICENSE file for the complete text of the license covering this
 * file.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bloomfilter.h"

void bloomfilter_init(bloomfilter_t* bf, int val)
{
    memset(bf->bf_data,val,BLOOMFILTER_NUM_INTS * sizeof(uint32_t));
}

static int bloomfilter_get_bit(bloomfilter_t* bf, uint32_t bit)
{
    return 0;
}

void bloomfilter_show(bloomfilter_t* bf)
{
    for(uint32_t i = 0; i < BLOOMFILTER_NUM_INTS; ++i)
    {
        printf("%u\n",bf->bf_data[i]);
    }
}

static uint32_t murmur_hash(uint32_t k, uint32_t seed)
{
    const uint32_t m = 0x5bd1e995;
    const uint32_t r = 24;
    uint32_t h = seed ^ 4;
    k *= m;
    k ^= k >> r;
    k *= m;
    h *= m;
    h ^= k;
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h % BLOOMFILTER_NUM_BITS;
}

static void make_k_hashes(uint32_t addr, uint32_t* hashes, uint32_t k)
{
    uint32_t h1 = murmur_hash(addr,0);
    for(unsigned i = 0; i < k; i++)
    {
        h1 = murmur_hash(addr, h1);
        hashes[i] = h1;
    }
}

static void bloomfilter_insert_k(bloomfilter_t* bf, uint32_t addr, uint32_t k)
{
    uint32_t hashes[BLOOMFILTER_NUM_HASHES];
    make_k_hashes(addr,hashes,k);
    for(unsigned i = 0; i < k; i++)
    {
        BLOOMFILTER_SET_BIT(bf,hashes[i]);
    }
}

void bloomfilter_insert(bloomfilter_t* bf, uint32_t addr)
{
    bloomfilter_insert_k(bf, addr, BLOOMFILTER_NUM_HASHES);
}

int bloomfilter_check_k(bloomfilter_t* bf, uint32_t addr, uint32_t k)
{
    uint32_t hashes[BLOOMFILTER_NUM_HASHES];
    make_k_hashes(addr,hashes,k);

    for(unsigned i = 0; i < k; i++)
    {
        if(!BLOOMFILTER_GET_BIT(bf,hashes[i]))
        {
            return 0;
        }
    }
    return 1;
}

int bloomfilter_check(bloomfilter_t* bf, uint32_t addr)
{
    return bloomfilter_check_k(bf, addr, BLOOMFILTER_NUM_HASHES);
}

