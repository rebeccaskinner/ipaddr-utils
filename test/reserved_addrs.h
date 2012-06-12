#ifndef _RESERVED_ADDRS_H_
#define _RESERVED_ADDRS_H_

struct res_addr
{
    uint32_t host;
    uint32_t subnet;
}

struct reservation
{
    char*           reservation_name;
    struct res_addr addrs[];
};

typedef struct reservation reservation_t;
typedef struct res_addr res_addr_t;

#endif
