/* Copyright Rebecca Skinner 2012
 * This source code is covered under the GNU Lesser General Public License.
 * See the LICENSE file for the complete text of the license covering this
 * file.
 */
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
#include "addr_utils.h"
#include "iptree.h"

static int output_fd;
#define BUFFER_SIZE 5000

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

ip_tree_t* make_nonintersecting_tree(ip_tree_t* tree1,
                                     uint32_t tree1_size,
                                     uint32_t* tree2_size)
{
    ip_tree_t* tree2 = NULL;
    uint32_t   expected_tree_size = *tree2_size;
    *tree2_size = 0;
    if((tree1_size + *tree2_size) > (uint32_t)-1)
        goto cleanup;

    tree2 = iptree_new();
    while(expected_tree_size)
    {
        if(0 == expected_tree_size % 10000)
            printf("%u addresses left to generate\n",expected_tree_size);
        uint32_t addr = make_noncolliding_addr(0,0,tree1);
        if(iptree_add_addr(addr,tree2))
        {
            --expected_tree_size;
            (*tree2_size)++;
        }
    }

cleanup:
    return tree2;
}

static void buffered_writer(uint32_t addr, int flush)
{
    static uint32_t buffer [BUFFER_SIZE] __attribute__((aligned));
    static const int elems = sizeof(buffer) / sizeof(uint32_t);
    static int entries = 0;
    if(__builtin_expect(1 == flush, 0))
    {
        goto flush;
    }
    buffer[entries++] = addr;
    if(elems == entries)
    {
flush:
        write(output_fd,buffer,entries * sizeof(uint32_t));
        entries = 0;
    }
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    if(argc != 4 || !argv || !argv[1] || !argv[2] || !argv[3])
    {
        printf("usage: makenoncolliding <input> <output> <number of addresses>\n");
        return 1;
    }

    output_fd = open(argv[2], O_TRUNC | O_CREAT | O_WRONLY,
                     S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    uint32_t tree1_size;
    uint32_t tree2_size = atoi(argv[3]);
    uint32_t* addrs = malloc(tree2_size * sizeof(uint32_t));
    ip_tree_t* tree1 = load_tree(argv[1],&tree1_size);
    ip_tree_t* tree2 = make_nonintersecting_tree(tree1,tree1_size,&tree2_size);
    uint32_t real_size = iptree_get_sorted(tree2,addrs); 
    write(output_fd,&real_size,sizeof(uint32_t));
    for(uint32_t i = 0; i < real_size; i++)
    {
        buffered_writer(addrs[i],0);
    }
    buffered_writer(0,1);

    close(output_fd);

    return 0;
}
