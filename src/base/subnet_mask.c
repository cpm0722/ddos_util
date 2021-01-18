#include "subnet_mask.h"

unsigned int get_addr_val(char *str)	// unsigned int -> str
{
	unsigned int res = 0;
	char tmp_str[16];
	strcpy(tmp_str, str);
	char *ptr = strtok(tmp_str, ".");
	for(int i = 3; i >= 0; i--){
		unsigned int val = atoi(ptr);
		res |= val << (BYTE_LEN*i);
		ptr = strtok(NULL, ".");
	}
	return res;
}

char *get_addr_str(unsigned int val)	// str -> unsigned int
{
	char *str = (char *)malloc(sizeof(char)*IPV4_STR_LEN);
	unsigned int split[4] = {0, 0, 0, 0};
	for(int i = 3; i >= 0; i--){
		split[i] = (val >> (BYTE_LEN*i)) & BYTE_MAX_VAL;
	}
	sprintf(str, "%u.%u.%u.%u", split[3], split[2], split[1], split[0]);
	return str;
}

char *masking_next_ip_addr(char *ipv4, char *now, int mask)	// get next ipv4 address from now with subnetmask
{
	unsigned int now_addr;
	if(!now){	//first call
		unsigned int ipv4_addr = get_addr_val(ipv4);
		now_addr = ipv4_addr;
		unsigned int max_val = BIT_32_MAX_VAL;
		now_addr = (unsigned int)now_addr & (max_val << (32-mask));
		return get_addr_str(now_addr);
	}
	now_addr = get_addr_val(now);
	unsigned int max_val = (unsigned int)pow(2, (32-mask))-1;
	if((now_addr & max_val) == max_val){ // finish
		unsigned int ipv4_addr = get_addr_val(ipv4);
		now_addr = ipv4_addr;
		unsigned int max_val = BIT_32_MAX_VAL;
		now_addr = (unsigned int)now_addr & (max_val << (32-mask));
		return get_addr_str(now_addr);
	}
	now_addr += 1;
	return get_addr_str(now_addr);
}
