#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "bloomfilter.h"
#include "addr_utils.h"
#include "iptree.h"

#define COMPLEMENT_TREE_SIZE 100000

int test_hashing(ip_tree_t* tree, uint32_t num_addrs, bloomfilter_t* bf)
{
    uint32_t*  addrs;
    uint32_t   found_addrs;
    bloomfilter_init(bf,0);
    addrs = malloc(num_addrs * sizeof(uint32_t));
    found_addrs = iptree_get_sorted(tree,addrs);

    for(uint32_t i = 0; i < found_addrs; i++)
    {
        bloomfilter_insert(bf,addrs[i]);
        if(!bloomfilter_check(bf,addrs[i]))
        {
            printf("Failed to validated address insertion for %s (%u)\n",
                    addr_to_string(addrs[i]),addrs[i]);
            return 1;
        }
    }

    return 0;
}

uint32_t test_collisions(ip_tree_t* tree1,
                         uint32_t tree1_size,
                         ip_tree_t* tree2,
                         uint32_t tree2_size,
                         bloomfilter_t* bf)
{
    uint32_t* addrs1 = malloc(tree1_size * sizeof(uint32_t));
    uint32_t* addrs2 = malloc(tree2_size * sizeof(uint32_t));
    tree1_size = iptree_get_sorted(tree1,addrs1);
    tree2_size = iptree_get_sorted(tree2,addrs2);
    uint32_t collisions = 0;

    printf("Adding reference list to bloom filter (size=%u)\n",tree1_size);
    for(uint32_t i = 0; i < tree1_size; ++i)
    {
        bloomfilter_insert(bf,addrs1[i]);
    }

    printf("checking for collisions (size=%u)...\n",tree2_size);
    for(uint32_t i = 0; i < tree2_size; ++i)
    {
        if(bloomfilter_check(bf,addrs2[i]))
        {
            collisions++;
        }
    }

    return collisions;
}

static void test_bloomfilter_bit_setting(bloomfilter_t* bf)
{
    for(int i = 0; i < 43; i++)
    {
        BLOOMFILTER_SET_BIT(bf,i);
        if(!BLOOMFILTER_GET_BIT(bf,i))
        {
            printf("Error: bit was set but shows as 0\n");
        }
    }

    for(unsigned i = 0; i < BLOOMFILTER_NUM_BITS; i++)
    {
        printf("Bit %u is%s set\n",i,BLOOMFILTER_GET_BIT(bf,i)?"":" not");
    }
}

ip_tree_t* load_tree(const char* fname, uint32_t* num_addrs)
{
    ip_tree_t* tree;
    tree = load_addr_file(fname,num_addrs);
    if(!num_addrs)
    {
        printf("load file error\n");
        return NULL;
    }

    return tree;
}

int validate_tree_uniqueness(ip_tree_t* tree, ip_tree_t* t2, uint32_t t2_size)
{
    int rv = 1;
    uint32_t* addrs = malloc(t2_size * sizeof(uint32_t));
    uint32_t num_addrs = iptree_get_sorted(t2,addrs);

    for(unsigned i = 0; i < num_addrs; i++)
    {
        if(iptree_addr_exists(addrs[i],tree)) goto cleanup;
    }

    rv = 0;

cleanup:
    free(addrs);
    return rv;
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    bloomfilter_t bf;
    bloomfilter_init(&bf,0);

    if(argc != 3 || !argv || !argv[1] || !argv[2])
    {
        printf("usage: bftest <filename> <filename>\n");
        return 1;
    }

    uint32_t num_addrs;
    uint32_t tree2_size;
    ip_tree_t* tree  = load_tree(argv[1],&num_addrs);
    ip_tree_t* tree2 = load_tree(argv[2],&tree2_size);

    // printf("checking uniqueness in ip lists...\n");
    // if(validate_tree_uniqueness(tree,tree2,tree2_size))
    // {
    //     printf("error: ip lists are not unique\n");
    //     return 1;
    // }
    // printf("success\n");

    // printf("bloomfilter size: %lu bits\n",BLOOMFILTER_NUM_BITS);

    // printf("Testing bloomfilter insertion\n");
    // if(test_hashing(tree,num_addrs,&bf))
    // {
    //     printf("Bloomfilter insertion Failed!\n");
    //     return 1;
    // }
    // else
    //     printf("Bloomfilter insertion Success\n");

    uint32_t collisions = test_collisions(tree,num_addrs,tree2,tree2_size,&bf);

    printf("%u collisions out of %u (%lf percent collision rate)\n",
           collisions, tree2_size, 100.0*((double)collisions/(double)tree2_size));

    return 0;
}
