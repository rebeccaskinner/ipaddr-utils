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

static void init() __attribute__((constructor));

static int output_fd;

#define BUFFER_SIZE 5000

static void init()
{
    srand(time(NULL));
}
static int parse_args(int argc, char** argv, uint32_t* restrict addr, uint8_t* restrict subnet)
{
    if(argc != 4 || !argv || !argv[1] || !argv[2] || !argv[3])
    {
        printf("usage: make_addrs <subnet> <count> <output file>\n");
        return -1;
    }

    char* addr_s = argv[1];
    *subnet = parse_subnet(addr_s);
    uint32_t count = atoi(argv[2]);
    *addr = string_to_addr(addr_s);
    *addr = apply_subnet(*addr, *subnet);
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    output_fd = open(argv[3],O_CREAT | O_WRONLY, mode);
    if(-1 == output_fd)
    {
        printf("Error: could not open output file: %s\n",strerror(errno));
        return -1;
    }

    return count;
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
    uint32_t addr;
    uint8_t  subnet;
    int count = parse_args(argc, argv, &addr, &subnet);
    if(-1 == count) return 1;

    printf("size of a /%hhu subnet is %d addresses\n",subnet,subnet_size(subnet));

    while(count--)
    {
        buffered_writer(make_random_addr(addr,subnet),0);
    }

    buffered_writer(0,1);

    return 0;
}
