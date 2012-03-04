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

    bloomfilter_show(*bf);
    for(uint32_t i = 0; i < found_addrs; i++)
    {
        printf("hasing %s\n",addr_to_string(addrs[i]));
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


int test_conversions(ip_tree_t* tree, uint32_t num_addrs)
{
    uint32_t*  addrs;
    uint32_t   found_addrs;
    char real_string[512];
    char converted_string[512];

    addrs = malloc(num_addrs * sizeof(uint32_t));
    found_addrs = iptree_get_sorted(tree,addrs);
    for(uint32_t i = 0; i < found_addrs; i++)
    {
        strncpy(real_string,addr_to_string(addrs[i]),512);
        strncpy(converted_string,
                addr_to_string(
                    vector_to_octets(
                        octets_to_vector(addrs[i]))),512);
        if(0 != strcmp(real_string,converted_string))
        {
            printf("conversion mismatch!\n");
            printf("real string: %s",real_string);
            printf("converted string: %s\n",converted_string);
            printf("addr: %u\n",addrs[i]);
            return 1;
        }
        else
        {
            printf("validated %s\n",real_string);
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

    // printf("Testing conversions... ");
    // if(test_conversions(tree,num_addrs))
    // {
    //     printf("Failed!\n");
    //     return 1;
    // }
    // else
    //     printf("Success\n");

    // printf("Testing bloomfilter insertion... ");
    // if(test_hashing(tree,num_addrs,&bf))
    // {
    //     printf("Failed!\n");
    //     return 1;
    // }
    // else
    //     printf("Success\n");

    uint32_t collisions = test_collisions(tree,num_addrs,tree2,tree2_size,&bf);

    printf("%u collisions out of %u (%lf percent collision rate)\n",
            collisions, tree2_size, ((double)tree2_size/(double)collisions));

    return 0;
}
