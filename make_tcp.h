#ifndef MAKE_TCP

#define MAKE_TCP

struct tcp_pseudo_header
{
__u32 source_address;
__u32 dest_address;
__u8 placeholder;
__u8 protocol;
__u16 tcp_length;
};

struct tcphdr prepare_empty_tcp();

struct tcphdr tcp_set_source(struct tcphdr tcph, __u16 src_port);

struct tcphdr tcp_set_dest(struct tcphdr tcph, __u16 dest_port);

struct tcphdr tcp_set_seq(struct tcphdr tcph, __u32 seq);

struct tcphdr tcp_set_ack_seq(struct tcphdr tcph, __u32 ack_seq);
struct tcphdr tcp_set_syn_flag(struct tcphdr tcph);
struct tcphdr tcp_set_ack_flag(struct tcphdr tcph);
__u32 tcp_get_seq(struct tcphdr tcph);
__u32 tcp_get_source(struct tcphdr tcph);
__u32 tcp_get_dest(struct tcphdr tcph);

char *tcp_add_data(struct tcphdr tcph, void * data, int datasize);

struct tcp_pseudo_header tcp_prepare_pseudo(struct iphdr ipv4h,struct tcphdr tcph, int add_datasize);
struct tcphdr tcp_get_checksum(struct iphdr ipv4h, struct tcphdr tcph, void *data,int datasize);

int tcp_make_connection(__u32 src_ip, __u32 dest_ip, int src_port, int dest_port);
#endif
