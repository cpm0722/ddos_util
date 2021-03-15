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
int masking_next_ip_addr(char *ipv4, char now[IPV4_STR_LEN], int mask);

//ybj
int get_mask_from_ip_addr(char *ipv4);
void get_ip_from_ip_addr(char *ipv4, char *now);

//hsk
int split_ip_mask_port(char *argv[], char src_ipv4[IPV4_STR_LEN], char dest_ipv4[IPV4_STR_LEN], int *src_mask, int *dest_mask, int *port_start, int *port_end);
int generator(char *src_ipv4, char *dest_ipv4, int src_mask, int dest_mask, int port_start, int port_end, char src_now[IPV4_STR_LEN], char dest_now[IPV4_STR_LEN], int *port_now);
