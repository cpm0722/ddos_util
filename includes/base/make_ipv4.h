#ifndef MAKE_IPV4

#define MAKE_IPV4

/**
	@fn struct iphdr prepare_empty_ipv4(void);
	@brief function: get empty iphdr
	@date 2021/04/03
	@param void
	@return struct iphdr
*/
struct iphdr prepare_empty_ipv4(void);

/**
	@fn struct iphdr ipv4_set_protocol(struct iphdr ip_head, __u8 protocol)
	@brief function: set iphdr's protocol
	@date 2021/04/03
	@param struct iphdr ip_head, __u8 protocol: protocol value
	@return struct iphdr
*/
struct iphdr ipv4_set_protocol(struct iphdr ip_head, __u8 protocol);

/**
	@fn struct iphdr ipv4_set_daddr(struct iphdr ip_head, __u32 daddr)
	@brief function: set iphdr's dest address
	@date 2021/04/03
	@param struct iphdr ip_head, __u32 daddr: dest address value
	@return struct iphdr
*/
struct iphdr ipv4_set_daddr(struct iphdr ip_head, __u32 daddr);

/**
	@fn struct iphdr ipv4_set_saddr(struct iphdr ip_head, __u32 saddr)
	@brief function: set iphdr's src address
	@date 2021/04/03
	@param struct iphdr ip_head, __u32 saddr: src address value
	@return struct iphdr
*/
struct iphdr ipv4_set_saddr(struct iphdr ip_head, __u32 saddr);

/**
	@fn struct iphdr ipv4_add_size(struct iphdr ip_head, __u32 data_size)
	@brief function: update iphdr's size(add data_size)
	@date 2021/04/03
	@param struct iphdr ip_head, __u32 data_size
	@return struct iphdr
*/
struct iphdr ipv4_add_size(struct iphdr ip_head, __u32 data_size);

/**
	@fn char *packet_assemble(struct iphdr ip_head, void *data, __u32 data_size)
	@brief function: assemble iphdr and data
	@date 2021/04/03
	@param struct iphdr ip_head, void *data, __u32 data_size
	@return struct iphdr
*/
char *packet_assemble(struct iphdr ip_head, void *data, __u32 data_size);

/**
	@fn int make_socket(int protocol)
	@brief function: make ipv4 raw socket
	@date 2021/04/03
	@param int protocol
	@return int: socket
*/
int make_socket(int protocol);

/**
	@fn int send_packet(int sock, struct iphdr ip_head, char *packet, int port)
	@brief function: send packet to ip_head's dest address + port
	@date 2021/04/03
	@param int sock, struct iphdr ip_head: dest's iphdr, char *packet: packet for send, int port: dest port
	@return void
*/
void send_packet(int sock, struct iphdr ip_head, char *packet, int port);

/**
	@fn __u16 in_cksum(unsigned short *ptr, int nbytes)
	@brief function: iphdr checksum value calculate
	@date 2021/04/03
	@param unsigned short *ptr: iphdr ptr, int nbytes: sizeof(iphdr)
	@return __u16: checksum value
*/
__u16 in_cksum(unsigned short *ptr, int nbytes);

#endif // ifndef MAKE_IPV4
