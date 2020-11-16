#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <math.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "make_ipv4.h"

struct iphdr prepare_empty_ipv4() {
	/*Prepare IP header*/
	struct iphdr ip_head;
	ip_head.ihl = 5; /*headerlength with no options*/
	ip_head.version = 4;
	ip_head.tos = 0;
	ip_head.tot_len = sizeof(struct iphdr);
	ip_head.id = htons(31337 + (rand() % 100));
	ip_head.frag_off = 0;
	ip_head.ttl = 255;
	ip_head.protocol = IPPROTO_TCP;
	ip_head.check = 0; /*Fill in later*/
	ip_head.saddr = 0;
	ip_head.daddr = 0;
	ip_head.check = in_cksum((unsigned short*) &ip_head, sizeof(struct iphdr));

	return ip_head;
}

struct iphdr ipv4_set_protocol(struct iphdr ip_head, __u8 protocol) {
	ip_head.protocol = protocol;
	ip_head.check = in_cksum((unsigned short*) &ip_head, sizeof(struct iphdr));
	return ip_head;
}

struct iphdr ipv4_set_saddr(struct iphdr ip_head, __u32 saddr) {
	ip_head.saddr = saddr;
	ip_head.check = in_cksum((unsigned short*) &ip_head, sizeof(struct iphdr));
	return ip_head;
}

struct iphdr ipv4_set_daddr(struct iphdr ip_head, __u32 daddr) {
	ip_head.daddr = daddr;
	ip_head.check = in_cksum((unsigned short*) &ip_head, sizeof(struct iphdr));
	return ip_head;
}
struct iphdr ipv4_add_size(struct iphdr ip_head, __u32 data_size) {
	ip_head.tot_len += data_size;

	ip_head.check = in_cksum((unsigned short*) &ip_head, sizeof(struct iphdr));
	return ip_head;
}

void* packet_assemble(struct iphdr ip_head, void *data, __u32 data_size) {
	void *packet = (void*) malloc(sizeof(ip_head) + data_size);

	memcpy(packet, (void*) &ip_head, sizeof(ip_head));
	memcpy((char*) packet + sizeof(ip_head), (void*) data, data_size);
	return packet;
}
void send_packet(struct iphdr ip_head, void *packet, int port) {
	struct sockaddr_in src, dest;
	int sock;
	sock = socket(PF_INET, SOCK_RAW, IPPROTO_RAW);
	if (sock < 0) {
		perror("socket() error");
		exit(-1);
	} else
		//printf("socket()-SOCK_RAW and tcp protocol is OK.\n");

	src.sin_family = AF_INET;
	dest.sin_family = AF_INET;

	src.sin_port = htons(port);
	dest.sin_port = htons(port);

	src.sin_addr.s_addr = ip_head.saddr;
	dest.sin_addr.s_addr = ip_head.daddr;

	//printf("SRC : %d, DEST : %d\n", src.sin_addr.s_addr, dest.sin_addr.s_addr);

	int one = 1;
	const int *val = &one;
	if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
		perror("setsockopt() error");
		exit(-1);
	} else
		//printf("setsockopt() is OK\n");
	//printf("Message length = %d\n", ip_head.tot_len);
	if (sendto(sock, packet, ip_head.tot_len, 0, (struct sockaddr*) &dest,
			sizeof(dest)) < 0) {
		perror("sendto() error");
		exit(-1);
	} else
		//printf(" sendto() is OK\n");

	close(sock);
}

unsigned short in_cksum(unsigned short *ptr, int nbytes) {

	register long sum; /* assumes long == 32 bits */
	u_short oddbyte;
	register u_short answer; /* assumes u_short == 16 bits */
	/*
	 * the algorithm is simple, using a 32-bit accumulator (sum),
	 * we add sequential 16-bit words to it, and at the end, fold back
	 * all the carry bits from the top 16 bits into the lower 16 bits.
	 */
	sum = 0;
	while (nbytes > 1) {
		sum += *ptr++;
		nbytes -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nbytes == 1) {
		oddbyte = 0; /* make sure top half is zero */
		*((u_char*) &oddbyte) = *(u_char*) ptr; /* one byte only */
		sum += oddbyte;
	}

	/*
	 * Add back carry outs from top 16 bits to low 16 bits.
	 */
	sum = (sum >> 16) + (sum & 0xffff); /* add high-16 to low-16 */
	sum += (sum >> 16); /* add carry */
	answer = ~sum; /* ones-complement, then truncate to 16 bits */
	return (answer);
}

void next_ip_addr(char *current, __u8 offset) {
	char *loc = current + 0;
	__u8 ips[4];

	int i = 0;

	i=0;
	char *buf = strtok(loc,".");
	ips[i++] = atoi(buf);
	while ( (buf=strtok(NULL, ".")) != NULL) {
		ips[i++] = atoi(buf);
	}

	int top=0;
	__u16 val_check;
	for(i=3;i>=0;i--)
	{
		if(i!=3 && top==0) break;
		if(top==1) ips[i]++;

		if(top==-1) ips[i]--;

		top =0;

			if(i==3)
			{
				val_check = ips[i]+offset;
				ips[i] += offset;
			}

		if(val_check>255)
		{
			 top=1;
		}

		if(val_check<0)
		{
			top=-1;
		}
		val_check=0;

	}


	char ips_c[16];


	sprintf(ips_c, "%d.%d.%d.%d", ips[0],ips[1],ips[2],ips[3]);
	strcpy(current,ips_c);

}
