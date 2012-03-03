#ifndef __ADDR_UTILS_H__
#define __ADDR_UTILS_H__

char* addr_to_string(const uint32_t addr) __attribute__((pure));
uint32_t string_to_addr(const char const *restrict str) __attribute__((pure));
uint32_t apply_subnet(uint32_t addr, uint8_t subnet) __attribute__((pure));
uint8_t parse_subnet(char* addr) __attribute__((pure));
uint32_t make_random_addr(uint32_t mask, uint8_t subnet);
uint32_t subnet_size(uint8_t subnet) __attribute__((pure));

#endif
