#ifndef MAKE_IPV4

#define MAKE_IPV4

struct iphdr prepare_empty_ipv4();

struct iphdr ipv4_set_protocol(struct iphdr ip_head, __u8 protocol);
struct iphdr ipv4_set_daddr(struct iphdr ip_head, __u32 daddr);
struct iphdr ipv4_set_saddr(struct iphdr ip_head, __u32 saddr);
struct iphdr ipv4_add_size(struct iphdr ip_head, __u32 data_size);

char* packet_assemble(struct iphdr ip_head, void *data, __u32 data_size);

int make_socket(int PROTOCOL);
void send_packet(int sock, struct iphdr ip_head, char *packet, int port);
__u16 in_cksum(unsigned short *ptr, int nbytes);

void next_ip_addr(char *current, __u8 offset);

#endif
