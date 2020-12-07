#include "header.h"

#include "make_ipv4.h"
#include "make_tcp.h"

struct tcphdr prepare_empty_tcp() {

	struct tcphdr tcp_head;
	memset(&tcp_head, 0, sizeof(struct tcphdr));

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

	tcp_head.window = htons(5840);
	tcp_head.check = 0;

	return tcp_head;
}

struct tcphdr tcp_set_source(struct tcphdr tcph, __u16 src_port) {
	tcph.source = htons(src_port);
	return tcph;
}

struct tcphdr tcp_set_dest(struct tcphdr tcph, __u16 dest_port) {
	tcph.dest = htons(dest_port);
	return tcph;
}

struct tcphdr tcp_set_seq(struct tcphdr tcph, __u32 seq) {
	tcph.seq = htonl(seq);
	return tcph;
}

struct tcphdr tcp_set_ack_seq(struct tcphdr tcph, __u32 ack_seq) {
	tcph.ack_seq = htonl(ack_seq);
	return tcph;
}

struct tcphdr tcp_set_syn_flag(struct tcphdr tcph) {
	tcph.syn = 1;
	return tcph;
}

char *tcp_add_data(struct tcphdr tcph, void* data, int datasize)
{
	char *return_data = (char*)malloc(sizeof(struct tcphdr) + datasize);
	memcpy(return_data,&tcph,sizeof(tcph));
	memcpy(return_data+sizeof(tcph),data,datasize);
	return return_data;
}

struct tcp_pseudo_header tcp_prepare_pseudo(struct iphdr ipv4h,struct tcphdr tcph, int add_datasize)
{
	struct tcp_pseudo_header psh;
	memset(&psh, 0, sizeof(struct tcp_pseudo_header));
	psh.source_address = ipv4h.saddr;
	psh.dest_address = ipv4h.daddr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_TCP;
	psh.tcp_length = htons(sizeof(struct tcphdr) + add_datasize);
	return psh;
}

struct tcphdr tcp_get_checksum(struct iphdr ipv4h, struct tcphdr tcph,
		int datasize) {
	struct tcp_pseudo_header psh;
	psh = tcp_prepare_pseudo(ipv4h,tcph,datasize);
	int psize = sizeof(struct tcp_pseudo_header) + sizeof(struct tcphdr)+datasize;
	char *assembled = (char*) malloc(psize);
	memcpy(assembled, (char*) &psh, sizeof(struct tcp_pseudo_header));
	memcpy(assembled + sizeof(struct tcp_pseudo_header), &tcph,
			sizeof(struct tcphdr)+datasize);

	//tcph.check = in_cksum((__u16*) psh, psize);

	return tcph;
}

void compute_tcp_checksum(struct iphdr *pIph, unsigned short *ipPayload) {
    register unsigned long sum = 0;
    unsigned short tcpLen = ntohs(pIph->tot_len) - (pIph->ihl<<2);
    struct tcphdr *tcphdrp = (struct tcphdr*)(ipPayload);
    //add the pseudo header
    //the source ip
    sum += (pIph->saddr>>16)&0xFFFF;
    sum += (pIph->saddr)&0xFFFF;
    //the dest ip
    sum += (pIph->daddr>>16)&0xFFFF;
    sum += (pIph->daddr)&0xFFFF;
    //protocol and reserved: 6
    sum += htons(IPPROTO_TCP);
    //the length
    sum += htons(tcpLen);

    //add the IP payload
    //initialize checksum to 0
    tcphdrp->check = 0;
    while (tcpLen > 1) {
        sum += * ipPayload++;
        tcpLen -= 2;
    }
    //if any bytes left, pad the bytes and add
    if(tcpLen > 0) {
        //printf("+++++++++++padding, %dn", tcpLen);
        sum += ((*ipPayload)&htons(0xFF00));
    }
      //Fold 32-bit sum to 16 bits: add carrier to result
      while (sum>>16) {
          sum = (sum & 0xffff) + (sum >> 16);
      }
      sum = ~sum;
    //set computation result
    tcphdrp->check = (unsigned short)sum;
}

