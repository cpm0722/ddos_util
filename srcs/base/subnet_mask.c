#include "base/subnet_mask.h"

unsigned int get_addr_val(char *str)	// unsigned int -> str
{
	unsigned int res = 0;
	char tmp_str[IPV4_STR_LEN];
	memset(tmp_str, '\0', IPV4_STR_LEN);
	strcpy(tmp_str, str);
	char *ptr = strtok(tmp_str, ".");
	for (int i = 3; i >= 0; i--) {
		unsigned int val = atoi(ptr);
		res |= val << (BYTE_LEN * i);
		ptr = strtok(NULL, ".");
	}
	return res;
}

void get_addr_str(unsigned int val, char str[IPV4_STR_LEN])	// str -> unsigned int
{
	unsigned int split[4] = { 0, 0, 0, 0 };
	for (int i = 3; i >= 0; i--) {
		split[i] = (val >> (BYTE_LEN * i)) & BYTE_MAX_VAL;
	}
	sprintf(str, "%u.%u.%u.%u", split[3], split[2], split[1], split[0]);
	return;
}

int masking_next_ip_addr(char *ipv4, char now[IPV4_STR_LEN], int mask)	// get next ipv4 address from now with subnetmask
{
	unsigned int now_addr;
	if (!strlen(now)) {	//first call
		unsigned int ipv4_addr = get_addr_val(ipv4);
		now_addr = ipv4_addr;
		unsigned int max_val = BIT_32_MAX_VAL;
		now_addr = (unsigned int) now_addr & (max_val << (32 - mask));
		get_addr_str(now_addr, now);
		return -1;
	}
	now_addr = get_addr_val(now);
	unsigned int max_val = (unsigned int) pow(2, (32 - mask)) - 1;
	if ((now_addr & max_val) == max_val) { // finish
		unsigned int ipv4_addr = get_addr_val(ipv4);
		now_addr = ipv4_addr;
		unsigned int max_val = BIT_32_MAX_VAL;
		now_addr = (unsigned int) now_addr & (max_val << (32 - mask));
		get_addr_str(now_addr, now);
		return 1;
	}
	now_addr++;
	if (now_addr && 0xff == 0) {
		now_addr++;
	}
	get_addr_str(now_addr, now);
	return 0;
}

int get_mask_from_ip_addr(char *ipv4)
{
	int mask = 32;
	char *char_ptr = ipv4 + 7;

	int i = 0;
	while (char_ptr[i] != '/') {
		i++;
		if (i>strlen(ipv4))
			return 32;
	}

	char mask_string[4];

	i++;
	strcpy(mask_string, char_ptr + i);
	mask = atoi(mask_string);

	return mask;

}

void get_ip_from_ip_addr(char *ipv4, char *now)
{
	char *char_ptr = ipv4;
	int i = 0;
	while (char_ptr[i] != '/') {
		i++;
		if (i>strlen(ipv4)){
			strcpy(now, ipv4);
			return;
		}
	}

	memcpy(now,ipv4,sizeof(char)*i);

	return;
}

int split_ip_mask_port(char *argv[], char src_ipv4[IPV4_STR_LEN], char dest_ipv4[IPV4_STR_LEN], int *src_mask, int *dest_mask, int *port_start, int *port_end)
{
	char *src = argv[0];
	char *dest = argv[1];
	char *port = argv[2];
	get_ip_from_ip_addr(src, src_ipv4);
	get_ip_from_ip_addr(dest, dest_ipv4);
	*src_mask = get_mask_from_ip_addr(src);
	*dest_mask = get_mask_from_ip_addr(dest);

	char *ptr = strtok(port, "-");
	*port_start = atoi(ptr);
	ptr = strtok(NULL, "-");
	if(ptr) {
		*port_end = atoi(ptr);
	}
	else {
		*port_end = *port_start;
	}
	return 0;
}

int generator(char *src_ipv4, char *dest_ipv4, int src_mask, int dest_mask, int port_start, int port_end, char src_now[IPV4_STR_LEN], char dest_now[IPV4_STR_LEN], int *port_now)
{
	if(masking_next_ip_addr(dest_ipv4, dest_now, dest_mask)){ // last dest_ipv4 or first(blank)
		int res;
		if((res = masking_next_ip_addr(src_ipv4, src_now, src_mask)) > 0){ // last src_ipv4
			(*port_now)++;
			if(*port_now > port_end){
				*port_now = port_start;
			}
		}
		else if(res < 0){
			*port_now = port_start;
		}
	}
	return 0;
}