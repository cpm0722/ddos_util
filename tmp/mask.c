#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define BIT_32_MAX_VAL 0xffffffff
#define BYTE_MAX_VAL 0xff
#define BYTE_LEN 8

typedef enum{false, true} bool;

unsigned int get_addr_val(char *str)
{
	unsigned int res = 0;
	char *ptr = strtok(str, ".");
	for(int i = 3; i >= 0; i--){
		unsigned int val = atoi(ptr);
		res |= val << (BYTE_LEN*i);
		ptr = strtok(NULL, ".");
	}
	return res;
}

char *get_addr_str(unsigned int val)
{
	char *str = (char *)malloc(sizeof(char)*16);
	unsigned int split[4] = {0, 0, 0, 0};
	for(int i = 3; i >= 0; i--){
		split[i] = (val >> (BYTE_LEN*i)) & BYTE_MAX_VAL;
	}
	sprintf(str, "%u.%u.%u.%u", split[3], split[2], split[1], split[0]);
	return str;
}

char *masking_next_ip_addr(char *ipv4, char *now, int mask)
{
	unsigned int now_addr;
	if(!now){
		unsigned int ipv4_addr = get_addr_val(ipv4);
		now_addr = ipv4_addr;
		unsigned int max_val = BIT_32_MAX_VAL;
		now_addr = (unsigned int)now_addr & (max_val << (32-mask));
		now = get_addr_str(now_addr);
		return now;
	}
	now_addr = get_addr_val(now);
	unsigned int max_val = (unsigned int)pow(2, (32-mask))-1;
	if((now_addr & max_val) == max_val) // finish
		return NULL;
	now_addr += 1;
	now = get_addr_str(now_addr);
	return now;
}

int main(int argc, char *argv[])
{
	if(argc != 3){
		fprintf(stderr, "%s [start_ip_addr] [subnet_mask(0~32)]\n", argv[0]);
		exit(1);
	}
	char *ipv4 = argv[1];
	int mask = atoi(argv[2]);
	char *now = NULL;

	while((now = masking_next_ip_addr(ipv4, now, mask))){
		printf("%s\n", now);
	}
	return 0;
}
