#ifndef SUBNET_MASK

#define SUBNET_MASK

#define BIT_32_MAX_VAL 0xffffffff
#define BYTE_MAX_VAL 0xff
#define BYTE_LEN 8
#define IPV4_STR_LEN 20

/**
	@fn unsigned int get_addr_val(char *str)
	@brief function: change ipv4 format: str(XXX.XXX.XXX.XXX) -> __u32
	@date 2021/04/03
	@param __uc *str: str(XXX.XXX.XXX.XXX)
	@return __u32
*/
__u32 get_addr_val(__uc *str);

/**
	@fn void get_addr_str(unsigned int val, char str[IPV4_STR_LEN])
	@brief function: change ipv4 format: __32 -> str(XXX.XXX.XXX.XXX)
	@date 2021/04/03
	@param __u32 val, __uc str[IPV4_STR_LEN]
	@return void
*/
void get_addr_str(__u32 val, __uc str[IPV4_STR_LEN]);

/**
	@fn int masking_next_ip_addr(char *ipv4, char now[IPV4_STR_LEN], int mask)
	@brief function: update now ipv4 to next ipv4 with subnet masking value
	@date 2021/04/03
	@param __uc *ipv4: initial input of ipv4, __uc *now[IPV4_STR_LEN]: now ipv4 for update, __u32 mask: subnet masking value
	@return int: first case(now param was empty): -1, last case(there is no next ip with subnet masking): 1, else: 0
*/
int masking_next_ip_addr(__uc *ipv4, __uc now[IPV4_STR_LEN], __u32 mask);

/**
	@fn int get_mask_from_ip_addr(char *ipv4)
	@brief function: get subnet masking value from masking ipv4 str(XXX.XXX.XXX.XXX/XX)
	@date 2021/04/03
	@param __uc *ipv4
	@return __u32: masking value
*/
__u32 get_mask_from_ip_addr(__uc *ipv4);

/**
	@fn void get_ip_from_ip_addr(char *ipv4, char *now)
	@brief function: get ipv4 str(XXX.XXX.XXX.XXX) from masking ipv4 str(XXX.XXX.XXX.XXX/XX)
	@date 2021/04/03
	@param __uc *ipv4: masking ipv4 str, __uc *now: ipv4 str
	@return void
*/
void get_ip_from_ip_addr(__uc *ipv4, __uc *now);

/**
	@fn int split_ip_mask_port(char *argv[], char src_ipv4[IPV4_STR_LEN], char dest_ipv4[IPV4_STR_LEN], int *src_mask, int *dest_mask, int *port_start, int *port_end)
	@brief function: split argv to arguments(ipv4, masking, port)
	@date 2021/04/03
	@param char *argv[], __uc src_ipv4[IPV4_STR_LEN], __uc dest_ipv4[IPV4_STR_LEN], __u32 *src_mask, __u32 *dest_mask, __u32 *port_start, __u32 *port_end
	@return int 0: SUCCESS, -1: FAIL
*/
int split_ip_mask_port(char *argv[], __uc src_ipv4[IPV4_STR_LEN], __uc dest_ipv4[IPV4_STR_LEN], __u32 *src_mask, __u32 *dest_mask, __u32 *port_start, __u32 *port_end);

/**
	@fn int generator(char *src_ipv4, char *dest_ipv4, int src_mask, int dest_mask, int port_start, int port_end, char src_now[IPV4_STR_LEN], char dest_now[IPV4_STR_LEN], int *port_now)
	@brief function: get next arguments(ipv4, masking, port) from now arguments
	@date 2021/04/03
	@param __uc *src_ipv4, unsigend char *dest_ipv4, __u32 src_mask, __u32 dest_mask, __u32 port_start, __u32 port_end, __uc src_now[IPV4_STR_LEN], __uc dest_now[IPV4_STR_LEN], __u32 *port_now
	@return int 0: SUCCESS, -1: FAIL
*/
int generator(__uc *src_ipv4, __uc *dest_ipv4, __u32 src_mask, __u32 dest_mask, __u32 port_start, __u32 port_end, __uc src_now[IPV4_STR_LEN], __uc dest_now[IPV4_STR_LEN], __u32 *port_now);

int argv_to_input_arguments(char *argv[], InputArguments *input);
int get_masking_arguments(InputArguments *input, MaskingArguments *now);

#endif	// ifndef SUBNET_MASK
