#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "iptree.h"

char* addr_to_string(uint32_t addr)
{
    struct in_addr a = {addr};
    return inet_ntoa(a);
}

uint32_t string_to_addr(const char const * restrict str)
{
    static struct in_addr a;
    inet_aton(str, &a);
    return a.s_addr;
}

uint32_t make_random_addr(uint32_t mask, uint8_t subnet)
{
    uint32_t addr;
    addr = random();
    return (ntohl(addr >> subnet) | mask);
}

uint32_t make_noncolliding_addr(uint32_t mask, uint8_t subnet, ip_tree_t* tree)
{
    uint32_t addr = make_random_addr(mask, subnet);
    while(iptree_addr_exists(addr,tree))
        addr = make_random_addr(mask, subnet);

    return addr;
}

uint8_t parse_subnet(char* addr)
{
    char* slash = strrchr(addr,'/');
    uint8_t subnet = 32;
    if(!slash) goto cleanup;
    *(slash++) = '\0';
    subnet = atoi(slash);
cleanup:
    return subnet;
}

uint32_t apply_subnet(uint32_t addr, uint8_t subnet)
{
    if(subnet == 32) return addr;
    uint32_t mask = (2<<subnet) - 1;
    mask = ntohl(mask << (32 - subnet));
    return (addr & mask);
}

uint32_t subnet_size(uint8_t subnet)
{
    return (2 << (31 - subnet)) - 2; // subtract one for broadcast
}

ip_tree_t* load_addr_file(const char* path, uint32_t* count)
{
    int fd = open(path,O_RDONLY);
    ip_tree_t* tree = iptree_new();;
    uint32_t* mapped_addrs = NULL;
    uint32_t num_elems = 0;
    struct stat st;
    if(count) *count = 0;
    if(-1 == fd)
    {
        printf("Error opening file: %s\n",strerror(errno));
        goto cleanup;
    }

    fstat(fd,&st);
    mapped_addrs = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(MAP_FAILED == mapped_addrs)
    {
        printf("Error mapping file:%s\n",strerror(errno));
        goto cleanup;
    }
    num_elems=mapped_addrs[0];
    for(size_t i = 1; i <= num_elems; i++)
    {
        iptree_add_addr(mapped_addrs[i],tree);
    }

    if(count) *count = num_elems;
cleanup:
    if(-1 != fd) close(fd);
    if(mapped_addrs) munmap(mapped_addrs,(num_elems * sizeof(uint32_t)));
    return tree;
}
