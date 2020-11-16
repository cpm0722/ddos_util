
#include "asm_types.h"
unsigned short in_cksum(unsigned short *ptr, int nbytes);
struct iphdr prepare_empty_ipv4();

struct iphdr ipv4_set_protocol(struct iphdr ip_head,__u8 protocol);
struct iphdr ipv4_set_daddr(struct iphdr ip_head, __u32 daddr);
struct iphdr ipv4_set_saddr(struct iphdr ip_head, __u32 saddr);
struct iphdr ipv4_add_size(struct iphdr ip_head,__u32 data_size);

void *packet_assemble(struct iphdr ip_head, void* data, __u32 data_size);

void send_packet(struct iphdr ip_head,void *packet, int port);

void next_ip_addr(char *current, __u8 offset);


