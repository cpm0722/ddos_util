#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include <errno.h>

#define MAX_VAL 0xff
#define BYTE_LEN 8

typedef enum{false, true} bool;

// convert struct in_addr to unsigned long array
void split_addr(struct in_addr *addr, unsigned long val[4])
{
	unsigned long byte_mask = 255;
	for(int i = 0; i < 4; i++, byte_mask <<= BYTE_LEN){
		val[i] = (addr->s_addr & byte_mask) >> BYTE_LEN*i;
	}
	return;
}

// merge unsigned long array into struct in_addr
struct in_addr merge_addr(unsigned long val[4])
{
	struct in_addr result;
	result.s_addr = 0;
	for(int i = 0; i < 4; i++){
		result.s_addr |= val[i] << BYTE_LEN*i;
	}
	return result;
}

char *masking_next_ip_addr(char *ipv4, char *now, char *mask)
{
	struct in_addr ipv4_addr, now_addr, mask_addr;
	inet_aton(ipv4, &ipv4_addr);
	inet_aton(now, &now_addr);
	inet_aton(mask, &mask_addr);
	unsigned long ipv4_val[4] = {0, 0, 0, 0};
	unsigned long now_val[4] = {0, 0, 0, 0};
	unsigned long mask_val[4] = {0, 0, 0, 0};

	split_addr(&ipv4_addr, ipv4_val);
	split_addr(&now_addr, now_val);
	split_addr(&mask_addr, mask_val);
	for(int i = 3; i >= 0; i--){
			unsigned long mask_tmp= mask_val[i];
			unsigned long ipv4_tmp = ipv4_val[i];
		for(unsigned long val = now_val[i]+1; val <= 255; val++){
			unsigned long b = 1;
			bool correct = true;
			for(int i = 0; i < 8; i++, b <<= 1){
				if((mask_tmp & b) && ((val & b) != (ipv4_tmp & b))){
					correct = false;
					break;
				}
			}
			if(!correct){
				continue;
			}
			else{
				now_val[i] = val;
				struct in_addr result_addr = merge_addr(now_val);
				return inet_ntoa(result_addr);
			}
		}
		now_val[i] = ipv4_val[i];
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	if(argc != 3){
		fprintf(stderr, "%s [start_ip_addr] [subnet_mask]\n", argv[0]);
		exit(1);
	}
	char *ipv4 = argv[1];
	char *mask = argv[2];
	char *now = ipv4;
	while(now){
		printf("%s\n", now);
		now = masking_next_ip_addr(ipv4, now, mask);
	}
	return 0;
}
