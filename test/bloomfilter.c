/* Copyright Tim Skinner 2012
 * This source code is covered under the GNU Lesser General Public License.
 * See the LICENSE file for the complete text of the license covering this
 * file.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bloomfilter.h"

bloomfilter_t* bloomfilter_new(int val, uint32_t size, uint32_t hashes)
{
    bloomfilter_t* bf = malloc((2*sizeof(uint32_t)) + size);
    bf->bf_num_ints   = size / sizeof(uint32_t);
    bf->bf_num_hashes = hashes;
    memset(bf->bf_data,val,size);
    return bf;
}

void bloomfilter_show(bloomfilter_t* bf)
{
    for(uint32_t i = 0; i < BLOOMFILTER_NUM_INTS(bf); ++i)
    {
        printf("%u\n",bf->bf_data[i]);
    }
}

static uint32_t murmur_hash(uint32_t k, uint32_t seed, uint32_t num_bits)
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
    return h % num_bits;
}

static void make_k_hashes(uint32_t addr, uint32_t* hashes, uint32_t k, uint32_t num_bits)
{
    uint32_t h1 = murmur_hash(addr,0, num_bits);
    for(unsigned i = 0; i < k; i++)
    {
        h1 = murmur_hash(addr, h1, num_bits);
        hashes[i] = h1;
    }
}

static void bloomfilter_insert_k(bloomfilter_t* bf, uint32_t addr, uint32_t k)
{
    uint32_t hashes[BLOOMFILTER_NUM_HASHES(bf)];
    make_k_hashes(addr,hashes,k, BLOOMFILTER_NUM_BITS(bf));
    for(unsigned i = 0; i < k; i++)
    {
        BLOOMFILTER_SET_BIT(bf,hashes[i]);
    }
}

void bloomfilter_insert(bloomfilter_t* bf, uint32_t addr)
{
    bloomfilter_insert_k(bf, addr, BLOOMFILTER_NUM_HASHES(bf));
}

int bloomfilter_check_k(bloomfilter_t* bf, uint32_t addr, uint32_t k)
{
    uint32_t hashes[BLOOMFILTER_NUM_HASHES(bf)];
    make_k_hashes(addr,hashes,k, BLOOMFILTER_NUM_BITS(bf));

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
    return bloomfilter_check_k(bf, addr, BLOOMFILTER_NUM_HASHES(bf));
}

