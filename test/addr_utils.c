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
