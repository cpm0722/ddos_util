#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define BIT_32_MAX_VAL 0xffffffff
#define BYTE_MAX_VAL 0xff
#define BYTE_LEN 8
#define IPV4_STR_LEN 20

typedef enum{false, true} bool;

unsigned int get_addr_val(char *str);
void get_addr_str(unsigned int val, char str[IPV4_STR_LEN]);
void masking_next_ip_addr(char *ipv4, char now[IPV4_STR_LEN], int mask);
