#include <netinet/tcp.h>
#include <math.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "make_ipv4.h"
#include "make_tcp.h"




struct tcphdr prepare_empty_tcp() {

	struct tcphdr tcp_head;
	memset(&tcp_head,0,sizeof(struct tcphdr));

	tcp_head.source = 0; //port
	tcp_head.dest = 0; //port
	tcp_head.seq = 0; //fill in later;
	tcp_head.ack_seq = 0; //fill in later
	tcp_head.doff = 5;

	tcp_head.ack = 0;
	tcp_head.syn = 0;
	tcp_head.fin = 0;

	tcp_head.res1 = 0;
	tcp_head.urg = 0;
	tcp_head.psh = 0;
	tcp_head.rst = 0;
	tcp_head.res2 = 0;

	tcp_head.window = htons (5840);
	tcp_head.check =0;

	return tcp_head;
}

struct tcphdr tcp_set_source(struct tcphdr tcph, __u16 src_port)
{
	tcph.source = htons(src_port);
	return tcph;
}

struct tcphdr tcp_set_dest(struct tcphdr tcph, __u16 dest_port)
{
	tcph.dest = htons(dest_port);
	return tcph;
}

struct tcphdr tcp_set_seq(struct tcphdr tcph, __u32 seq)
{
	tcph.seq = htonl(seq);
	return tcph;
}

struct tcphdr tcp_set_ack_seq(struct tcphdr tcph, __u32 ack_seq)
{
	tcph.ack_seq = htonl(ack_seq);
	return tcph;
}

struct tcphdr tcp_set_syn_flag(struct tcphdr tcph)
{
	tcph.syn =1;
	return tcph;
}

struct tcphdr tcp_get_checksum(struct iphdr ipv4h, struct tcphdr tcph, int datasize)
{
	struct pseudo_header psh;
	memset(&psh,0,sizeof(struct pseudo_header));
	psh.source_address = ipv4h.saddr;
	psh.dest_address = ipv4h.daddr;
	psh.placeholder=0;
	psh.protocol = IPPROTO_TCP;
	psh.tcp_length = htons(sizeof(struct tcphdr)+datasize);
	int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + datasize;
	char *assembled = (char *)malloc(psize);
	memcpy(assembled, (char *)&psh, sizeof(struct pseudo_header));
	memcpy(assembled + sizeof(pseudo_header),&tcph, sizeof(struct tcphdr)+datasize);

	tcph.check = in_cksum( (__u16*) assembled, psize);

	return tcph;
}

